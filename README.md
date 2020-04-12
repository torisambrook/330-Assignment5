# CS 330 INTRODUCTION TO OPERATING SYSTEMS
## Assignment 5

### **Name:** Tori Sambrook
### **Student Number:** 200386684

#### Description of game:
The point of this game is to move the player (represented by 'P') through the level map in order to advance levels and win the game. To advance through a level, they must reach the '@' in the map. To win the game, they must reach the "\*' in the map. However, there are 3 monsters (represented by 'M') in each level that will move closer to the player on each move. If the monster reaches a player, the player loses the game. Also, there are obstacles/obstructions that restrict player and monster movement in certain spots of the level. To move the player you must enter N, S, E, or W. To quit the game, enter Q.

#### Topic:
For this assignment, I wanted to implement some of the programming skills learned through this course in a way that wouldn't typically use these coding techniques. To do so, I created a program that uses a game concept similar to the one used in CS 115 which centered around an attacking game. My project expands from this concept by using file system calls and threading to implement the game.

This project uses file system calls such as read() and write() to open level files, modify, make copies, move objects around in the contents, and to test for different scenarios such as finding distances between monsters and the player. Threading is used to process the different levels of the game. The game has the ability to implement as many levels as there are threads. From the main, the threads are created and joined, and each thread executes the function gamePlay which controls all the processes of a level (input, movement, testing flag variables, and reading/writing from specified files).

#### Challenges:
A program such as this would be more efficient if it used more object-oriented design techniques such as classes and 2D arrays instead of opening and closing files so often. However, the purpose of using so many file system calls was to demonstrate how these operating system related coding techniques could be implemented in uncommon ways. A limitation of this program is that it requires the user have the temp.txt file already existing in the same directory when running the program otherwise an error occurs. This limitation could be removed by creating some functionality that creates the file if it doesn't already exist, and deletes it when the program finishes (similar to the tempLevel.txt file). 

#### Reproducing the project:
To reproduce the project, you need to have the following files in the directory of execution:
- main.cpp
- movement.cpp
- level.cpp
- movement.h
- level.h
- level1.txt
- level2.txt
- temp.txt

Then, compile, link, and run all the files using your preferred compiler. Once the program is running, a description of the game will appear and the level map will be printed to the screen. Input your next move and press enter until you win, quit, or die by being reached by a monster.
