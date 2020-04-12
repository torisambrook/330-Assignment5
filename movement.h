/*
    CS 330 ASSIGNMENT 5:
    This program illistrates certain programming skills learned in CS 330 such as system file calls and threading. The program 
    contains a relativly simple attacking game in which the user moves a player through different levels in order to reach the 
    end goal position. If a monster reaches the player first they lose the game.
    
    movement.h: Contains the header files, structure definitions, global and pre-defined variables, and function prototypes for
    implementing the movement of the monster and player.
    
    Programmer: Tori Sambrook
    Last Updated: April 11, 2020    
*/


#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "level.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/stat.h>  
#include <fcntl.h>     
#include <errno.h>
#include <string>
#include <pthread.h>
#include <cmath>
#include <stdio.h>

using namespace std;

// Player struct holds the current row and column of the player in the level
struct Player
{
    int row;
    int column;
};

// Monster struct holds the row, column, and nextMove of a monster in the level
struct Monster 
{
    int row;
    int column;
    char nextMove;
};

extern const int COLUMNS;       // Used for reading and writing to files
extern const int NUM_THREADS;   // Holds the number of threads/levels in the program
extern const int NUM_MONSTERS;  // Holds the number of monsters in each level

extern Player Player1;          // Shared variable for the player in the game
extern Monster monsters[3];     // Shared variable containing the data for the monsters in the program

/*
    movePlayer

    Purpose: To move the player to the position indicated by nextMove. It uses file system calls to read 
             from the level file and write to the temp file.
    Parameters: The nextMove and the level.
    Return: None
*/
void movePlayer(char, string);

/*
    moveMonsters

    Purpose: To move a monster from its current position to the next closest position to the player.
             It uses file system calls to read from the level file and write to the temp file.
    Parameters: The level file.
    Return: None
*/
void moveMonsters(string);

/*
    findNextMonsterMove

    Purpose: To find the next closest position for a monster and store the position in monsters[i].nextmove. 
    Parameters: The level file.
    Return: None
*/
void findNextMonsterMove(string);

/*
    calculateDistance

    Purpose: It calculates the distance between the player and the possible moves the monster could make and 
             then determines which move has the shortest distance. 
    Parameters: The player's row, player's column, monster's row, monster's column.
    Return: The nextMove that the monster should make which moves it closer to the player.
*/
char calculateDistance(int, int, int, int);

#endif
