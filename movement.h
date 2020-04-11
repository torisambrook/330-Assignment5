/*
    CS 330 ASSIGNMENT 5:
    This program illistrates certain programming skills learned in CS 330 such as system file calls and threading. The program 
    contains a relativly simple attacking game in which the user moves a player through different levels in order to reach the 
    end goal position. If a monster reaches the player first they lose the game.

    movement.cpp: Contains the header files, structure definitions, global and pre-defined variables, and function prototypes for
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

extern const int COLUMNS;
extern const int NUM_THREADS;
extern const int NUM_MONSTERS;

extern Player Player1;
extern Monster monsters[3];



void movePlayer(char, string);
void moveMonsters(string );
char calculateDistance(int, int, int, int);
void findNextMonsterMove(string);


#endif