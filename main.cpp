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
    bool moved = false;
};

#define COLUMNS 61
bool gotoNextLevel = false;
bool win = false;
const int NUM_MONSTERS = 3;
//Thread chared memory, holds 3 monster positions
Monster monsters[NUM_MONSTERS];

void printMap(string, Player&, Monster[]);
void movePlayer(char, string, Player&);
void updateMap(string);
bool checkPosition(char, string, Player);
bool isValidPosition(char);
bool isValidInput(char);

int main()
{
    Player Player1;
    string levelsList[3] = {"level1.txt", "level2.txt", "level3.txt"};
    printMap(levelsList[0], Player1, monsters);
    char nextMove = ' ';
    int status;   
    
    
    // while the monsters haven't reached the player, ask user for next player move. Move the player and monsters. 
    // if monster is adjacent to player than game over and player loses. else if player has reached '@' then move to next level. 
    // else if player has reached '*' then player wins and game over. Print level at end of each loop. 

    cout << "The goal of the game is to avoid the monster and get to the * in the last level. \n" << 
	    "If the monster reaches you first the game is over. Advance through the levels by \n" <<
	    "making it to the @ in each level. Enter N, S, E, or W to move the player, and Q to quit." << endl;
    
    cout << "Enter next move: ";
    cin >> nextMove;
    nextMove = tolower(nextMove);

    //while(nextMove != 'q')
   // {
        // Check if it is valid input
        if(isValidInput(nextMove) && checkPosition(nextMove, levelsList[0], Player1))
        {
                // Pass move to movePlayer() function to update players position and map;
                movePlayer(nextMove, levelsList[0], Player1);
                
                // Update the map to the new level or position of player
                updateMap(levelsList[0]);
            }

        printMap(levelsList[0], Player1, monsters);
        cout << "Enter next move: ";
        cin >> nextMove;
        nextMove = tolower(nextMove);
    //}
    
    cout << "\nGAME OVER\n";
		
    return 0;
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

bool checkPosition(char nextMove, string level, Player Player1)
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
void movePlayer(char nextMove, string level, Player& Player1)
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
void printMap(string level,  Player& Player1, Monster monsters[])
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
    close (map);
}