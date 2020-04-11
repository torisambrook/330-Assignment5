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


extern bool gotoNextLevel;
extern bool win;
extern bool quit;
extern int monsterCount;   // monCount keeps track of which monster threads are working on
extern pthread_mutex_t output_lock;

void * gamePlay(void *);
void printMap(string);
void updateMap(string);
bool isDead();
bool checkPosition(char, string, int, int);
bool isValidPosition(char);
bool isValidInput(char);
void resetLevel(string level);
void storeLevel(string level);
void removeTemp();


#endif