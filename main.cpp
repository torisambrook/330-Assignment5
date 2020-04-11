/*
This game is concept is based on the Attacking games created from CS115, 
but adapted to be implemented by using operating system oriented programming.
Monsters will be implemented using threads, and new processes will be created to 
implement multiple levels. The goal of the player is to make it through the levels
without dying. 

Functions I need to add: 
importing the map from text file using system I/O
printing the map to the screen
creating mosters using threads
function for threads to calculate next move
function to test if player alive or dead
function for player to complete next move
function to create fork to next level -- implement later
*/

#include <iostream>
#include <unistd.h>
#include <sys/types.h> //needed for open
#include <sys/stat.h>  //needed for open
#include <fcntl.h>     //needed for open
#include <errno.h>
//#include <sys/wait.h>
#include <string>
#include <pthread.h>
#include <cmath>

using namespace std;

struct Player
{
    int row;
    int column;
};

struct Monster 
{
    int row;
    int column;
    char nextMove;
};

const int COLUMNS = 61;
const int NUM_THREADS = 2;
const int NUM_MONSTERS = 3;
bool gotoNextLevel = false;
bool win = false;
bool quit = false;

Player Player1;
Monster monsters[NUM_MONSTERS];
int monsterCount = 0;   // monCount keeps track of which monster threads are working on
pthread_mutex_t output_lock;

void printMap(string);
void movePlayer(char, string);
void updateMap(string);
bool checkPosition(char, string, int, int);
bool isValidPosition(char);
bool isValidInput(char);
void * gamePlay(void *);
void moveMonsters(string );
char calculateDistance(int, int, int, int);
void findNextMonsterMove(string);
bool isDead();

int main()
{
    string levelsList[2] = {"level1.txt", "level2.txt"}; 
    pthread_t *thread_ids = new pthread_t[NUM_THREADS];

    //Use unbuffered output on stdout
    setvbuf(stdout, (char *) NULL, _IONBF, 0);

    //Set up an output lock so that threads wait their turn to speak.
    if (pthread_mutex_init(&output_lock, NULL)!=0)
    {
        perror("Could not create mutex for output: ");
        return 1;
    } 
    cout << "WELCOME TO THE GAME! HERE ARE THE RULES:" << endl;
    cout << "The goal of the game is to move the player 'P' and win by reaching the '*' in the last level. \n" << 
	        "If a monster 'M' reaches you first the game is over. Advance through the levels by making it \n" << 
            "to the '@' in each level. Watch out for other symbols as they might be obsticals! Enter N, S,\n" << 
            "E, or W to move the player, and Q to quit." << endl << endl;
     
    // generate threads 
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if((pthread_create(&thread_ids[i], NULL, gamePlay, &levelsList[i]) > 0))
        {
            perror("creating thread:");
            return 2;
        }
    }

    // join threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_join(thread_ids[i], NULL) != 0)
        {
            perror("trouble joining thread: ");
            return 3;
        }
    }

    if(quit)
    {
        cout << endl << endl << "YOU QUIT THE GAME." << endl;
    }
    if(isDead())
    {
        cout << endl << endl << "THE PLAYER HAS DIED. GAME OVER." << endl;
    }
    if(win)
    {
        cout << endl << endl << "CONGRADULATIONS! YOU HAVE WON THE GAME." << endl;
    }
		
    return 0;
}

void * gamePlay(void * lev)
{
    char nextMove;
    string level = (*(string*)lev);
    
    // Lock critical section
    if (pthread_mutex_lock(&output_lock) != 0)
    {
        perror("Could not lock output: ");
        return NULL; //something horrible happened - exit whole program with error
    }
    win = false;
    gotoNextLevel = false;
    
    if(quit)
    {
        // Unlock critical section
        if (pthread_mutex_unlock(&output_lock) != 0)
        {
            perror("Could not unlock output: ");
            return NULL; //something horrible happened - exit whole program with error
        }

        return NULL;
    }
    
    printMap(level);
    findNextMonsterMove(level); 

    cout << endl << "Please enter your next move: ";
    cin >> nextMove;
    nextMove = tolower(nextMove);
    
    while(nextMove != 'q')
    {
        int prow = Player1.row;
        int pcol = Player1.column;
        // Check if it is valid input
        if(isValidInput(nextMove))
        {            
            // If the nextMove is possible, move the player there
            if(checkPosition(nextMove, level, prow, pcol))
            {
                // If the player reaches the '@' exit and go to the next level
                if(gotoNextLevel)
                    break;
                // Pass move to movePlayer() function to update players position and map;
                movePlayer(nextMove, level);
                updateMap(level);
            }
            
            while(monsterCount < NUM_MONSTERS)
            {
                int mrow = monsters[monsterCount].row;
                int mcol = monsters[monsterCount].column;
                if(checkPosition(monsters[monsterCount].nextMove, level, mrow, mcol))
                {
                    moveMonsters(level);
                    updateMap(level);
                }
                else
                {
                    monsterCount++;
                }
            }
            monsterCount = 0;           
        }
        
        updateMap(level);
        printMap(level);
        findNextMonsterMove(level); 

        // If the player has reached the '*' or is dead, exit
        if(win || isDead())
            break;

        cout << endl << "Please enter your next move: ";
        cin >> nextMove;
        nextMove = tolower(nextMove);
    }

    // Unlock critical section
    if (pthread_mutex_unlock(&output_lock) != 0)
    {
        perror("Could not unlock output: ");
        return NULL; //something horrible happened - exit whole program with error
    }

    return NULL;
}

bool isDead()
{
    int mrow, mcol;
    bool result = false;
    for(int i = 0; i < NUM_MONSTERS; i++)
    {
        mrow = monsters[i].row;
        mcol = monsters[i].column;
        // If the player is north of the monster
        if((Player1.row == mrow + 1) && (Player1.column == mcol))
            result = true;
        // If the player is south of the monster
        else if((Player1.row == mrow - 1) && (Player1.column == mcol))
            result = true;
        // If the player is east of the monster
        else if((Player1.row == mrow) && (Player1.column == mcol + 1))
            result = true;
        // If the player is west of the monster
        else if((Player1.row == mrow) && (Player1.column == mcol - 1))
            result = true;
    }
    
    if(result == true)
        quit = true;

    return result;
}

void moveMonsters(string level)
{    
    int map2, item2, temp2, mrow, mcol;
    char buffer[COLUMNS];
    int row = 0;
    int size = COLUMNS;
    mrow = monsters[monsterCount].row;
    mcol = monsters[monsterCount].column;

    if(!checkPosition(monsters[monsterCount].nextMove, level, mrow, mcol))
    {
        monsterCount++;
        return;
    }
    
    map2 = open(level.c_str(), O_RDONLY); 
    if(map2 == -1)
    {
        perror("\nMove Monsters map file open errror: ");
        return;
    }

    temp2 = open("temp.txt", O_WRONLY);
    if(temp2 == -1)
    {
        perror("\nMove Monsters temp file open errror: ");
        return;
    }
    
    while((item2=read(map2, buffer, size))!=0)
    {
        if(row == 0)
            size++;
        row++;
        
        switch(monsters[monsterCount].nextMove)
        {
        case 'n':
            if(row == (mrow - 1))
            {            
                 buffer[mcol] = 'M';
            }
            if(row == mrow)
            {
                buffer[mcol] = '.';
            }
            break;
        case 's':
            if(row == mrow)
            {
                buffer[mcol] = '.';  
            }
            if (row == (mrow + 1))
            {
                buffer[mcol] = 'M';                
            }
            break;
        case 'e':
            if(row == mrow)
            {
                buffer[mcol + 1] = 'M';
                buffer[mcol] = '.';
            }
            break;
        case 'w':
            if(row == mrow)
            {
                buffer[mcol - 1] = 'M';
                buffer[mcol] = '.';
            }
            break;
        }        
        item2 = write(temp2, buffer, item2);
    }
    
    close(map2);
    close(temp2);

    // Increase monster count for next thread
    monsterCount++;
    updateMap(level);
   
    return;
}

void findNextMonsterMove(string level)
{
    int map, item;
    char buffer[COLUMNS];
    int row = 0;
    int size = COLUMNS;
    char move;
    int count = 0;
    
    map = open(level.c_str(), O_RDONLY); 
    if(map == -1)
    {
        perror("\nFind next monster move map file open errror: ");
        exit(1);
    }
    
    while((item=read(map, buffer, size))!=0)
    {
        if(row == 0)
            size++;
        row++;

        for(int i = 0; i < COLUMNS; i++)
        {
            if(buffer[i] == 'M')
            {
                monsters[count].nextMove = calculateDistance(row, i, Player1.row, Player1.column);
                count++;
            }
        }
    }
    close(map);
}

char calculateDistance(int mrow, int mcolumn, int prow, int pcolumn)
{
    double north, south, east, west, x, y, min;
    // calculate distance by moving monster north
    x = (prow - (mrow - 1));
    y = (pcolumn - mcolumn);
    north = sqrt((x*x)+(y*y));
    min = north;

    // calculate distance by moving monster south
    x = (prow - (mrow + 1));
    y = (pcolumn - mcolumn);
    south = sqrt((x*x)+(y*y));
    if(min > south)
        min = south;

    // calculate distance by moving monster east
    x = (prow - mrow);
    y = (pcolumn - (mcolumn + 1));
    east = sqrt((x*x)+(y*y));
    if(min > east)
        min = east;

    // calculate distance by moving monster west
    x = (prow - mrow);
    y = (pcolumn - (mcolumn - 1));
    west = sqrt((x*x)+(y*y));
    if(min > west)
        min = west;
    
    // return the character corresponding to minimum distance
    if(min == north)
        return 'n';
    else if(min == south)
        return 's';
    else if(min == east)
        return 'e';
    else
        return 'w';
}


bool isValidInput(char input)
{
    if(input == 'n' || input == 's' || input == 'e' || input == 'w')
        return true;
    else if(input == 'q')
    {
        quit = true;
        return false;
    }
    else
    {
        cout << "INVALID INPUT. Please enter N, S, E, or W." << endl;
        return false;
    }
}

bool isValidPosition(char item)
{
    if(item == '.' || item == '~')
    {
        return true;
    }
    else if (item == '@')
    {
        cout << "ABOUT TO BE NEXT LEVEL\n";
        gotoNextLevel = true;
        return true;
    }
    else if(item == '*')
    {
        win = true;
        return true;
    }
    else
    {
        return false;
    }  
}

bool checkPosition(char nextMove, string level, int testRow, int testCol)
{
    int map, item;
    char buffer[COLUMNS];
    int row = 0;
    int size = COLUMNS;
    
    map = open(level.c_str(), O_RDONLY); 
    if(map == -1)
    {
        perror("\nCHECK POSITION map file open errror: ");
        exit(1);
    }
    
    while((item=read(map, buffer, size))!=0)
    {
        if(row == 0)
            size++;
        
        row++;

        switch(nextMove)
        {
        case 'n':
            if(row == (testRow - 1))
            {
                if(isValidPosition(buffer[testCol]))
                    return true;
                else
                {
                    close(map);
                    return false;
                }       
            }
            break;
        case 's':
            if(row == (testRow + 1))
            {
                if(isValidPosition(buffer[testCol]))
                    return true;
                else
                {
                    close(map);
                    return false;
                }       
            }
            break;
        case 'e':
            if(row == testRow)
            {
                if(isValidPosition(buffer[testCol + 1]))
                    return true;
                else
                {
                    close(map);
                    return false;
                }       
            }
            break;
        case 'w':
            if(row == testRow)
            {
                if(isValidPosition(buffer[testCol - 1]))
                    return true;
                else
                {
                    close(map);
                    return false;
                }       
            }
            break;
        }
    } 
    close(map);
    return false;
}

// Function movePlayer updates the map to hold the new position of the player
void movePlayer(char nextMove, string level)
{
    int map, item, temp;
    char buffer[COLUMNS];
    int row = 0;
    int size = COLUMNS;
    
    map = open(level.c_str(), O_RDONLY); 
    if(map == -1)
    {
        perror("\nMOVE PLAYER map file open errror: ");
        exit(1);
    }

    temp = open("temp.txt", O_WRONLY);
    if(temp == -1)
    {
        perror("\nMOVE PLAYER temp file open errror: ");
        exit(1);
    }
    
    while((item=read(map, buffer, size))!=0)
    {
        if(row == 0)
            size++;
        
        row++;
        
        switch (nextMove)
        {
        case 'n': 
            // if the Player is in the next row, write a 'P' in the current row 
            if(row == (Player1.row - 1))
            {                
                buffer[Player1.column] = 'P';
            }
            // if the current row has the 'P', replace it with a ' '
            if (row == Player1.row)
            {
                buffer[Player1.column] = '.';
            }
            break;
        case 's': 
            // if the current row has the 'P', replace it with a ' ' 
            if(row == Player1.row)
            {
                buffer[Player1.column] = '.';
            }
            // if the current row is after the row with the Player, write a 'P' in the current row
            if (row == (Player1.row + 1))
            {
                buffer[Player1.column] = 'P';                
            }
            break;
        case 'e':
            // Move the player 1 space to the right
            if(row == Player1.row)
            {
                buffer[Player1.column] = '.';
                buffer[Player1.column + 1] = 'P';
            }
            break;
        case 'w':
            // Move the player 1 space to the left
            if(row == Player1.row)
            {
                buffer[Player1.column] = '.';
                buffer[Player1.column - 1] = 'P';
            }
            break;
        }
        item = write(temp, buffer, item);
    }
    close(map);
    close(temp);
}

// Function updateMap uses file system calls to copy the map from the temp file to the level file
void updateMap(string level)
{
    int map, item, temp;
    char buffer[COLUMNS];
    
    map = open(level.c_str(), O_WRONLY); 
    if(map == -1)
    {
        perror("\nUPDATE MAP map file open errror: ");
        exit(1);
    }

    temp = open("temp.txt", O_RDONLY);
    if(temp == -1)
    {
        perror("\nUPDATE MAP temp file open errror: ");
        exit(1);
    }

    while((item=read(temp, buffer, COLUMNS))!=0)
    {
        
        // Update the map by copying the map from temp.txt to the level file
        item = write(map, buffer, item);
    }
    close(map);
    close(temp);
}

// Function printMap opens the level file, and prints it to the screen.
void printMap(string level)
{
    int map, item;
    char buffer[COLUMNS];
    int count = 0;
    
    map = open(level.c_str(), O_RDONLY); 
    if(map == -1)
    {
        perror("\n PRINT MAP map file open errror: ");
        exit(1);
    }
    int row = 0;
    int size = COLUMNS;
    // For each row of characeters in the map, execute the read() system call
    while((item=read(map, buffer, size))!=0)
    {
        if(row == 0)
            size++;
        
        row++;

        //Update Player location, monster locations, and map format
        for(int i = 0; i < COLUMNS; i++)
        {
            if(buffer[i] == 'P')
            {
                Player1.row = row;
                Player1.column = i;
            }
            if(buffer[i] == 'M')
            {
                monsters[count].row = row;
                monsters[count].column = i;
                count++;
            }
            if (buffer[i] == '.')
            {
                buffer[i] = ' ';
            }
        }
        
        // Print the map to the screen using the write() system call
        item = write(1, buffer, item);
    }
    /*
    cout << endl << "Number of rows: " << row << endl << "Player position: " << Player1.row <<  ", " << Player1.column << endl;
    cout << "Monster positions: \n" << monsters[0].row << ", " << monsters[0].column << endl << monsters[1].row << ", " << monsters[1].column << endl <<monsters[2].row << ", " << monsters[2].column << endl;
    cout << "Monster next moves: \n" << monsters[0].nextMove << " " << monsters[1].nextMove << " " << monsters[2].nextMove << endl;
    */
    close (map);
}