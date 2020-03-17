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
function for monsters to attack
function for player to complete next move
function to create fork to next level -- implement later
function to damage player or monster
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

struct player
{
    int row = 0;
    int column = 0;
    int health = 50;
};

struct Monster 
{
    int row = 0;
    int column = 0;
    int health = 0;
};

void printMap(string);

int main()
{
    string levels[3] = {"level1.txt", "level2.txt", "level3.txt"};
    printMap(levels[0]);

    return 0;
}

// Function printMap opens the level file, and prints it to the screen.
void printMap(string level)
{
    int map, item;
    char buffer[60];
    
    map = open(level.c_str(), O_RDONLY, S_IRUSR); 
    if(map == -1)
    {
        perror("\nomap file open errror: ");
        exit(1);
    }
    while( (item=read(map, buffer, 60))!=0)
    {
            item=write(1,buffer,item);
    }
    close (map);
}