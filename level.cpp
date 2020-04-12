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
        return NULL;
    }

    // Store the original level in tempLevel.txt
    storeLevel(level);

    // If the user has indicated they want to quit, unlock the critical section and return
    if(quit)
    {
        // Unlock critical section
        if (pthread_mutex_unlock(&output_lock) != 0)
        {
            perror("Could not unlock output: ");
            return NULL;
        }

        return NULL;
    }
    
    // Reset variables as user hasn't won and they are starting a new level
    win = false;
    gotoNextLevel = false;
    
    // Print the starting map to the screen and find the next monster moves according to user current position
    printMap(level);
    findNextMonsterMove(level); 

    // Prompt the user to enter move
    cout << endl << "Please enter your next move: ";
    cin >> nextMove;
    nextMove = tolower(nextMove);
    
    // While the user doesn't want to quit
    while(nextMove != 'q')
    {
        int prow = Player1.row;
        int pcol = Player1.column;
        
        // If the user entered valid input
        if(isValidInput(nextMove))
        {            
            // If the nextMove is possible, move the player there
            if(checkPosition(nextMove, level, prow, pcol))
            {
                // If the player reaches the '@' in the next move, exit and go to the next level
                if(gotoNextLevel)
                    break;

                movePlayer(nextMove, level);
                updateMap(level);
            }
            
            //For each of the monsters in the level
            while(monsterCount < NUM_MONSTERS)
            {
                int mrow = monsters[monsterCount].row;
                int mcol = monsters[monsterCount].column;

                //If the nextMove is possible for that monster
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
            // Reset the monsterCount for next move by the player
            monsterCount = 0;           
        }

        // Print the level map to the screen and find the next monster moves according to user current position
        printMap(level);
        findNextMonsterMove(level); 

        // If the player has reached the '*' or is dead, exit
        if(win || isDead())
            break;

        // Prompt the user for their next move
        cout << endl << "Please enter your next move: ";
        cin >> nextMove;
        nextMove = tolower(nextMove);
    }

    // if the user enetered 'q' after first iteration
    if(nextMove == 'q')
        quit = true;

    // Reset the contents of the level file back to the original map which is stored in tempLevel.txt
    resetLevel(level);

    // Unlock critical section
    if (pthread_mutex_unlock(&output_lock) != 0)
    {
        perror("Could not unlock output: ");
        return NULL; //something horrible happened - exit whole program with error
    }

    return NULL;
}

void printMap(string level)
{
    cout << endl;
    int map, item;
    char buffer[COLUMNS];
    int count = 0;
    int row = 0;
    int size = COLUMNS;
    
    // Open the level file for reading
    map = open(level.c_str(), O_RDONLY); 
    if(map == -1)
    {
        perror("\nIn printMap map file open errror: ");
        exit(1);
    }

    // Read the contents from the level file and write to the screen
    while((item=read(map, buffer, size))!=0)
    {
        if(row == 0)
            size++;
        row++;

        //Update player location, monster locations, and map format
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
        
        item = write(1, buffer, item);
    }
    close (map);
}

void updateMap(string level)
{
    int map, item, temp;
    char buffer[COLUMNS];
    
    // Open the level file for writing
    map = open(level.c_str(), O_WRONLY); 
    if(map == -1)
    {
        perror("\nIn updateMap map file open errror: ");
        exit(1);
    }

    // Open the temp file for reading
    temp = open("temp.txt", O_RDONLY);
    if(temp == -1)
    {
        perror("\nIn updateMap temp file open errror: ");
        exit(1);
    }

    // Read the contents from the temp file and write to the level file
    while((item=read(temp, buffer, COLUMNS))!=0)
    {
        item = write(map, buffer, item);
    }
    close(map);
    close(temp);
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
    
    // If the player is dead, set quit to true for convenience
    if(result == true)
        quit = true;

    return result;
}

bool checkPosition(char nextMove, string level, int testRow, int testCol)
{
    int map, item;
    char buffer[COLUMNS];
    int row = 0;
    int size = COLUMNS;
    
    // Open the level file for reading
    map = open(level.c_str(), O_RDONLY); 
    if(map == -1)
    {
        perror("\nIn checkPosition map file open errror: ");
        exit(1);
    }
    
    // Read the contents from the level file to check
    while((item=read(map, buffer, size))!=0)
    {
        if(row == 0)
            size++;
        row++;

        // Switch cases indicate what to do according to nextMove
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

bool isValidPosition(char item)
{
    if(item == '.' || item == '~')
    {
        return true;
    }
    // if the user is about to move to the next level
    else if (item == '@')
    {
        gotoNextLevel = true;
        return true;
    }
    // If the user has won
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

void storeLevel(string level)
{
    int map, item, temp;
    char buffer[COLUMNS];
    
    // Open the level file for reading
    map = open(level.c_str(), O_RDONLY); 
    if(map == -1)
    {
        perror("\nIn storeLevel map file open errror: ");
        exit(1);
    }

    // If tempLevel.txt doesn't exist, create it and open it for writing
    if(access("tempLevel.txt", F_OK) == -1)
    {
        temp = open("tempLevel.txt", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        if(temp == -1)
        {
            perror("\nIn storeLevel tempLevel file open errror: ");
            exit(1);
        }
    }
    // If it already exists, open it for writing
    else
    {
        temp = open("tempLevel.txt", O_WRONLY);
        if(temp == -1)
        {
            perror("\nIn storeLevel tempLevel file open errror: ");
            exit(1);
        }

    }
    
    // Read from contents of the level file and write to tempLevel.txt
    while((item=read(map, buffer, COLUMNS))!=0)
    {
        item = write(temp, buffer, item);
    }

    close(map);
    close(temp);
}

void resetLevel(string level)
{
    int map, item, temp;
    char buffer[COLUMNS];
    
    // Open the level file for writing
    map = open(level.c_str(), O_WRONLY); 
    if(map == -1)
    {
        perror("\nIn resetLevel map file open errror: ");
        exit(1);
    }

    // Open the tempLevel file for reading
    temp = open("tempLevel.txt",  O_RDONLY);
    if(temp == -1)
    {
        perror("\nIn resetLevel tempLevel file open errror: ");
        exit(1);
    }

    // Read the contents from tempLevel.txt and write to the level file
    while((item=read(temp, buffer, COLUMNS))!=0)
    {
        item = write(map, buffer, item);
    }
    close(map);
    close(temp);
}

void removeTemp()
{
    if(remove("tempLevel.txt") != 0)
    {
        perror("\nError removing tempLevel.txt: ");
        exit(1);
    }
}