/*
    CS 330 ASSIGNMENT 5:
    This program illistrates certain programming skills learned in CS 330 such as system file calls and threading. The program 
    contains a relativly simple attacking game in which the user moves a player through different levels in order to reach the 
    end goal position. If a monster reaches the player first they lose the game.
    
    movement.cpp: Contains global variables and function definitions for implementing the movement of the monster and player.
    
    Programmer: Tori Sambrook
    Last Updated: April 11, 2020    
*/

#include "movement.h"

const int COLUMNS = 61;
const int NUM_THREADS = 2;
const int NUM_MONSTERS = 3;

Player Player1;
Monster monsters[NUM_MONSTERS];


void movePlayer(char nextMove, string level)
{
    int map, item, temp;
    char buffer[COLUMNS];
    int row = 0;
    int size = COLUMNS;
    
    // Open level file for reading
    map = open(level.c_str(), O_RDONLY); 
    if(map == -1)
    {
        perror("\nIn movePlayer map file open errror: ");
        exit(1);
    }

    // Open temp file for writing
    temp = open("temp.txt", O_WRONLY);
    if(temp == -1)
    {
        perror("\nIn movePlayer temp file open errror: ");
        exit(1);
    }
    
    // Read the content from the level file and write to the temp file
    while((item=read(map, buffer, size))!=0)
    {
        if(row == 0)
            size++;
        row++;
        
        // Change the position of the player according to nextMove
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

void moveMonsters(string level)
{    
    int map2, item2, temp2, mrow, mcol;
    char buffer[COLUMNS];
    int row = 0;
    int size = COLUMNS;
    mrow = monsters[monsterCount].row;
    mcol = monsters[monsterCount].column;

    // If the nextMove for the monster is invalid, return and deal with the next monster
    if(!checkPosition(monsters[monsterCount].nextMove, level, mrow, mcol))
    {
        monsterCount++;
        return;
    }
    
    // Open the level file for reading
    map2 = open(level.c_str(), O_RDONLY); 
    if(map2 == -1)
    {
        perror("\nIn moveMonsters map file open errror: ");
        return;
    }

    // Open the temp file for writing
    temp2 = open("temp.txt", O_WRONLY);
    if(temp2 == -1)
    {
        perror("\nIn moveMonsters temp file open errror: ");
        return;
    }
    
    // Read from the level file and write to the temp file
    while((item2=read(map2, buffer, size))!=0)
    {
        if(row == 0)
            size++;
        row++;
        
        // Change the position of the monster according to their nextMove
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

    // Increase monsterCount for next monster
    monsterCount++;
   
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
    
    // Open the level file for reading
    map = open(level.c_str(), O_RDONLY); 
    if(map == -1)
    {
        perror("\nIn findNextMonsterMove map file open errror: ");
        exit(1);
    }
    
    // Read from the level file to find nextMove
    while((item=read(map, buffer, size))!=0)
    {
        if(row == 0)
            size++;
        row++;

        // For each monster, call calculateDistance to find move with shortest distance to player
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