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

Player Player1;
void printMap(string);
void findPlayer(Player&);



int main()
{
    
    string levelsList[3] = {"level1.txt", "level2.txt", "level3.txt"};
    printMap(levelsList[0]);
    
    // while the monsters haven't reached the player, ask user for next player move. Move the player and monsters. 
    // if monster is adjacent to player than game over and player loses. else if player has reached '@' then move to next level. 
    // else if player has reached '*' then player wins and game over. Print level at end of each loop. 

    findPlayer(Player1);

    return 0;
}
void findPlayer(Player& player1)
{
    
}

// Function printMap opens the level file, and prints it to the screen.
void printMap(string level)
{
    int map, item;
    char buffer[COLUMNS];
    
    map = open(level.c_str(), O_RDONLY, S_IRUSR); 
    if(map == -1)
    {
        perror("\nomap file open errror: ");
        exit(1);
    }
    int row = 0;
    int size = COLUMNS;
    // For each row of characeters in the map, execute the read() system call
   while((item=read(map, buffer, size))!=0)
    {
        if(row == 0)
            size++;
        // Update Player location and map format
       // cout << "Items read: " << item;
        row++;
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
        //cout << "Items written: " << item;
    }

    cout << endl << "Number of rows: " << row << endl << "Player position: " << Player1.row <<  ", " << Player1.column << endl;
    close (map);
}