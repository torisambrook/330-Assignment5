/*
    CS 330 ASSIGNMENT 5:
    This program illistrates certain programming skills learned in CS 330 such as system file calls and threading. The program 
    contains a relativly simple attacking game in which the user moves a player through different levels in order to reach the 
    end goal position. If a monster reaches the player first they lose the game.
    
    level.cpp: Contains global variables and function definitions for implementing the level map of the game.
    
    Programmer: Tori Sambrook
    Last Updated: April 11, 2020    
*/

#include "movement.h"
#include "level.h"

bool gotoNextLevel = false;
bool win = false;
bool quit = false;
int monsterCount = 0;   // monCount keeps track of which monster threads are working on
pthread_mutex_t output_lock;


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
    storeLevel(level);

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
    
    win = false;
    gotoNextLevel = false;
    
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

        printMap(level);
        findNextMonsterMove(level); 

        // If the player has reached the '*' or is dead, exit
        if(win || isDead())
            break;

        cout << endl << "Please enter your next move: ";
        cin >> nextMove;
        nextMove = tolower(nextMove);
    }

    if(nextMove == 'q')
        quit = true;

    resetLevel(level);

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

bool isValidInput(char input)
{
    if(input == 'n' || input == 's' || input == 'e' || input == 'w')
    {
        return true;
    }
    else if(input == 'q')
    {
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
    cout << endl;
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

void removeTemp()
{
    if(remove("tempLevel.txt") != 0)
    {
        perror("\nError removing tempLevel.txt: ");
        exit(1);
    }
}

void storeLevel(string level)
{
    int map, item, temp;
    char buffer[COLUMNS];
    
    map = open(level.c_str(), O_RDONLY); 
    if(map == -1)
    {
        perror("\nIn storeLevel map file open errror: ");
        exit(1);
    }

    // If tempLevel.txt doesn't exist, create it
    if(access("tempLevel.txt", F_OK) == -1)
    {
        temp = open("tempLevel.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        if(temp == -1)
        {
            perror("\nIn storeLevel tempLevel file open errror: ");
            exit(1);
        }
    }
    // If it already exists, open it to write to
    else
    {
        temp = open("tempLevel.txt", O_WRONLY);
        if(temp == -1)
        {
            perror("\nIn storeLevel tempLevel file open errror: ");
            exit(1);
        }

    }
    
    cout << "STORING LEVEL IN TEMPLEVEL\n";
    while((item=read(map, buffer, COLUMNS))!=0)
    {
        //Store the original map in tempLevel.txt
        item = write(temp, buffer, item);
    }

    close(map);
    close(temp);
}

void resetLevel(string level)
{
    int map, item, temp;
    char buffer[COLUMNS];
    
    map = open(level.c_str(), O_WRONLY); 
    if(map == -1)
    {
        perror("\nIn resetLevel map file open errror: ");
        exit(1);
    }

    temp = open("tempLevel.txt",  O_RDONLY);
    if(temp == -1)
    {
        perror("\nIn resetLevel tempLevel file open errror: ");
        exit(1);
    }

    cout << "STORING LEVEL IN LEVEL FROM TEMPLEVEL\n";
    while((item=read(temp, buffer, COLUMNS))!=0)
    {
        //Store the original map in the level file
        item = write(map, buffer, item);
    }
    close(map);
    close(temp);
}