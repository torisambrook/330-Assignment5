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
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
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

#define COLUMNS 61
bool gotoNextLevel = false;
bool win = false;
const int NUM_MONSTERS = 3;
Player Player1;
//Thread shared memory, holds 3 monster positions
Monster monsters[NUM_MONSTERS];
// monCount keeps track of which monster threads are working on
int monCount = 0;
pthread_mutex_t output_lock;

void printMap(string);
void movePlayer(char, string);
void updateMap(string);
bool checkPosition(char, string);
bool isValidPosition(char);
bool isValidInput(char);
void * moveMonsters(void *);
char calculateDistance(int, int, int, int);
void findNextMonsterMove(string);

int main()
{
    string levelsList[3] = {"level1.txt", "level2.txt", "level3.txt"};
    printMap(levelsList[0]);
    findNextMonsterMove(levelsList[0]);
    char nextMove = ' ';
    int status;   
    pthread_t *thread_ids = new pthread_t[NUM_MONSTERS];

    //Use unbuffered output on stdout
    setvbuf(stdout, (char *) NULL, _IONBF, 0);

    //Set up an output lock so that threads wait their turn to speak.
    if (pthread_mutex_init(&output_lock, NULL)!=0)
    {
        perror("Could not create mutex for output: ");
        return 1;
    }    

    cout << "The goal of the game is to avoid the monster and get to the * in the last level. \n" << 
	    "If the monster reaches you first the game is over. Advance through the levels by \n" <<
	    "making it to the @ in each level. Enter N, S, E, or W to move the player, and Q to quit." << endl;
    
    cout << "Enter next move: ";
    cin >> nextMove;
    nextMove = tolower(nextMove);

    while(nextMove != 'q')
    {
        // Check if it is valid input
        if(isValidInput(nextMove) && checkPosition(nextMove, levelsList[0]))
        {
            // Pass move to movePlayer() function to update players position and map;
            movePlayer(nextMove, levelsList[0]);

            // generate threads 
            for (int i = 0; i < NUM_MONSTERS; i++)
            {
                if( pthread_create(&thread_ids[i], NULL, moveMonsters, &levelsList[0]) > 0)
                {
                    perror("creating thread:");
                    return 2;
                }
            }

            // join threads and print their return values
            for (int i = 0; i < NUM_MONSTERS; i++)
            {
                if (pthread_join(thread_ids[i], NULL) != 0)
                {
                    perror("trouble joining thread: ");
                    return 3;
                }
            }
            // delete dynamically allocated thread array
            delete [] thread_ids;

            // Update the map to the new level or position of player
            updateMap(levelsList[0]);
        }
        
        findNextMonsterMove(levelsList[0]);
        printMap(levelsList[0]);

        cout << "Enter next move: ";
        cin >> nextMove;
        nextMove = tolower(nextMove);
    }
    
    cout << "\nGAME OVER\n";
		
    return 0;
}

void * moveMonsters(void *lev)
{
    // Set up lock around critical section
    if (pthread_mutex_lock(&output_lock) != 0)
    {
        perror("Could not lock output: ");
        exit(4); //something horrible happened - exit whole program with error
    }

    int map, item, temp, mrow, mcol;
    char buffer[COLUMNS];
    int row = 0;
    int size = COLUMNS;
    string level = (*(string*)lev);
    mrow = monsters[monCount].row;
    mcol = monsters[monCount].column;
    
    map = open(level.c_str(), O_RDONLY); 
    if(map == -1)
    {
        perror("\nmap file open errror: ");
        exit(1);
    }

    temp = open("temp.txt", O_WRONLY);
    if(temp == -1)
    {
        perror("\ntemp file open errror: ");
        exit(1);
    }
    
    while((item=read(map, buffer, size))!=0)
    {
        if(row == 0)
            size++;
        row++;

        switch(monsters[monCount].nextMove)
        {
        case 'n':
            if(row == (mrow - 1))
            {            
                if(isValidPosition(buffer[mcol]))  
                    buffer[mcol] = 'M';
                else
                    break;
            }
            if(row == mrow)
                buffer[mcol] = '.';
            break;
        case 's':
            if(row == mrow)
            {
                if(isValidPosition(buffer[mcol]))  
                    buffer[mcol] = '.';
                else
                    break;         
            }
            if (row == (Player1.row + 1))
            {
                buffer[mcol] = 'M';                
            }
            break;
        case 'e':
            if(row == mrow)
            {
                if(isValidPosition(buffer[mcol])) 
                {
                    buffer[mcol] = '.';
                    buffer[mcol + 1] = 'M';
                }
                else
                    break;
            }
            break;
        case 'w':
            if(row == mrow)
            {
                if(isValidPosition(buffer[mcol])) 
                {
                    buffer[mcol] = '.';
                    buffer[mcol - 1] = 'M';
                }
                else
                    break;
            }
            break;
        }
        item = write(temp, buffer, item);
    }
    close(map);
    close(temp);

    // Increase monster count for next thread
    monCount++;

    // Unlock critical section
    if (pthread_mutex_unlock(&output_lock) != 0)
    {
        perror("Could not unlock output: ");
        exit(5); //something horrible happened - exit whole program with error
    }
    return NULL;
}

void findNextMonsterMove(string level)
{
    cout << "FINDNEXTMONETERMOVE CALLED\n";
    int map, item;
    char buffer[COLUMNS];
    int row = 0;
    int size = COLUMNS;
    char move;
    int count = 0;
    
    map = open(level.c_str(), O_RDONLY); 
    if(map == -1)
    {
        perror("\nmap file open errror: ");
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
                cout << "MONSTER "<< count << " NEXT MOVE IS: " << monsters[count].nextMove << endl;
            }
            count++;
        }
    }
    close(map);
}

char calculateDistance(int mrow, int mcolumn, int prow, int pcolumn)
{
    cout << "CALCULATE DISTANCE CALLED\n";
    double north, south, east, west, x, y, min;
    // calculate distance by moving monster north
    x = (prow - mrow - 1);
    y = (pcolumn - mcolumn);
    north = sqrt((x*x)+(y*y));
    min = north;

    // calculate distance by moving monster south
    x = (prow - mrow + 1);
    y = (pcolumn - mcolumn);
    south = sqrt((x*x)+(y*y));
    if(min > south)
        min = south;

    // calculate distance by moving monster east
    x = (prow - mrow);
    y = (pcolumn - mcolumn + 1);
    east = sqrt((x*x)+(y*y));
    if(min > east)
        min = east;


    // calculate distance by moving monster west
    x = (prow - mrow);
    y = (pcolumn - mcolumn - 1);
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

bool checkPosition(char nextMove, string level)
{
    int map, item;
    char buffer[COLUMNS];
    int row = 0;
    int size = COLUMNS;
    
    map = open(level.c_str(), O_RDONLY); 
    if(map == -1)
    {
        perror("\nmap file open errror: ");
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
            if(row == (Player1.row - 1))
            {
                if(isValidPosition(buffer[Player1.column]))
                    return true;
                else
                {
                    close(map);
                    return false;
                }       
            }
            break;
        case 's':
            if(row == (Player1.row + 1))
            {
                if(isValidPosition(buffer[Player1.column]))
                    return true;
                else
                {
                    close(map);
                    return false;
                }       
            }
            break;
        case 'e':
            if(row == Player1.row)
            {
                if(isValidPosition(buffer[Player1.column + 1]))
                    return true;
                else
                {
                    close(map);
                    return false;
                }       
            }
            break;
        case 'w':
            if(row == Player1.row)
            {
                if(isValidPosition(buffer[Player1.column - 1]))
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
        perror("\nmap file open errror: ");
        exit(1);
    }

    temp = open("temp.txt", O_WRONLY);
    if(temp == -1)
    {
        perror("\ntemp file open errror: ");
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
        perror("\nmap file open errror: ");
        exit(1);
    }

    temp = open("temp.txt", O_RDONLY);
    if(temp == -1)
    {
        perror("\ntemp file open errror: ");
        exit(1);
    }

    while((item=read(temp, buffer, COLUMNS))!=0)
    {
        // Update the map by copying the map from temp.txt to the level file
        item = write(map, buffer, item);
    }
    close(temp);
    close(map);
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
        perror("\nmap file open errror: ");
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

        // Update Player location, monster locations, and map format
        for(int i = 0; i < COLUMNS; i++)
        {
            if(buffer[i] == 'P')
            {
                Player1.row = row;
                Player1.column = i;
            }
            else if(buffer[i] == 'M')
            {
                monsters[count].row = row;
                monsters[count].column = i;
                count++;
            }
            else if (buffer[i] == '.')
            {
                buffer[i] == ' ';
            }
        }
        
        // Print the map to the screen using the write() system call
        item = write(1, buffer, item);
    }

    cout << endl << "Number of rows: " << row << endl << "Player position: " << Player1.row <<  ", " << Player1.column << endl;
    cout << "Monster positions: \n" << monsters[0].row << ", " << monsters[0].column << endl << monsters[1].row << ", " << monsters[1].column << endl <<monsters[2].row << ", " << monsters[2].column << endl;
    cout << "Monster next moves: \n" << monsters[0].nextMove << " " << monsters[1].nextMove << " " << monsters[2].nextMove << endl;
    close (map);
}