/*
    CS 330 ASSIGNMENT 5:
    This program illistrates certain programming skills learned in CS 330 such as system file calls and threading. The program 
    contains a relativly simple attacking game in which the user moves a player through different levels in order to reach the 
    end goal position. If a monster reaches the player first they lose the game.
    
    level.h: Contains the header files, structure definitions, global and pre-defined variables, and function prototypes for
    implementing the level map of the game.
    
    Programmer: Tori Sambrook
    Last Updated: April 11, 2020    
*/

#ifndef LEVEL_H
#define LEVEL_H

#include <iostream>
using namespace std;

#include "movement.h"

extern bool gotoNextLevel;          // determines when the player should move to the next level
extern bool win;                    // tracks when the player has won
extern bool quit;                   // determines if the player has quit the game yet or not
extern int monsterCount;            // Keeps rack of which monster threads are working on
extern pthread_mutex_t output_lock; // thread shared memory for mutex synchronization

/*
    gamePlay

    Purpose: This thread function controls all the processes that occur for a level. This includes
             moving the player and monsters, checking validity of positions and input, checking 
             if player has won, quit, or moved to next level, and reseting the level file for the 
             next round. It is accessed by threads so its critical section is locked using mutex's. 
    Parameters: The level file the user is currently on.
    Return: None
*/
void * gamePlay(void *);

/*
    printMap

    Purpose: Prints the contents of a level file to the screen using sytem file calls.
    Parameters: The level file to print.
    Return: None
*/
void printMap(string);

/*
    updateMap

    Purpose: Copies the contents of the temp.txt file to a level file currently in use. This is done 
             using system file calls.
    Parameters: The level file to print.
    Return: None
*/
void updateMap(string);

/*
    idsDead

    Purpose: Copies the contents of the temp.txt file to a level file currently in use. This is done 
             using read() and write() system file calls.
    Parameters: The level file to print.
    Return: None
*/
bool isDead();

/*
    checkPosition

    Purpose: To check if position is valid or not depending on the nextMove.
    Parameters: The nextMove, the level, and the current row and column of the monster or player.
    Return: boolean indicating if the move is possible or not.
*/
bool checkPosition(char, string, int, int);

/*
    isValidPosition

    Purpose: To check if a character in the level is valid or not. If it an obstical character then it 
             returns false, otherwise true. For the symbol '@' it sets gotoNextLevel to true and for '*' it sets win to true.
    Parameters: The nextMove, the level, and the current row and column of the monster or player.
    Return: boolean indicating if the move is possible or not.
*/
bool isValidPosition(char);

/*
    isValidInput

    Purpose: To check if the user input is valid or not. If it is anything other than 
             [Nn], [Ss], [Ee], [Ww], or [Qq] then it returns false, otherwise true.
    Parameters: The user input. 
    Return: boolean indicating if the input is valid or not.
*/
bool isValidInput(char);

/*
    storeLevel

    Purpose: To store the original level (before any movement) into the file tempLevel.txt. It reads the
             original contents from the level file and writes the content to the temp file using file system calls.
    Parameters: The level file to read from.
    Return: None
*/
void storeLevel(string);

/*
    resetLevel

    Purpose: To store the original level (before any movement) back into the corresponding level text file. It copys the 
             original level from tempLevel.txt and stores it in the appropriate file using file system calls.
    Parameters: The level file to write to.
    Return: None
*/
void resetLevel(string);

/*
    removeTemp

    Purpose: To delete the tempLevel.txt file which was created for temporarily storing the original level files.
    Parameters: None
    Return: None
*/
void removeTemp();


#endif