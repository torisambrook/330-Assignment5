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
#include <string>
using namespace std;

#define COLUMNS 61

struct Player
{
    int row = 0;
    int column = 0;
    bool alive = true;
};

struct Monster 
{
    int row = 0;
    int column = 0;
};

void printMap(string, Player&);
void movePlayer(char, string, Player&);
void updateMap(string);

int main()
{
    Player Player1;
    string levelsList[3] = {"level1.txt", "level2.txt", "level3.txt"};
    printMap(levelsList[0], Player1);
    char nextMove = ' ';
    
    // while the monsters haven't reached the player, ask user for next player move. Move the player and monsters. 
    // if monster is adjacent to player than game over and player loses. else if player has reached '@' then move to next level. 
    // else if player has reached '*' then player wins and game over. Print level at end of each loop. 
    
    // while the player is not dead and user hasn't quit
    cout << "Enter next move: ";
    cin >> nextMove;

    // Check if valid move for player
    
    // Pass move to movePlayer() function to update players position and map;
    movePlayer(nextMove, levelsList[0], Player1);
    
    printMap(levelsList[0], Player1);


    return 0;
}

// Function movePlayer updates the map to hold the new position of the player
void movePlayer(char nextMove, string level, Player& Player1)
{
    nextMove = tolower(nextMove);
    int map, item, temp;
    char buffer[COLUMNS];
    
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
    int row = 0;
    int size = COLUMNS;
    
    
    while((item=read(map, buffer, size))!=0)
    {
        if(row == 0)
            size++;
        
        row++;
        //NORTH AND SOUTH NOT WORKING
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

    updateMap(level);
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
void printMap(string level,  Player& Player1)
{
    int map, item;
    char buffer[COLUMNS];
    
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

        // Update Player location and map format
        for(int i = 0; i < COLUMNS; i++)
        {
            if(buffer[i] == 'P')
            {
                Player1.row = row;
                Player1.column = i;
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
    close (map);
}