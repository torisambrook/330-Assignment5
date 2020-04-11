/*
    CS 330 ASSIGNMENT 5:
    This program illistrates certain programming skills learned in CS 330 such as system file calls and threading. The program 
    contains a relativly simple attacking game in which the user moves a player through different levels in order to reach the 
    end goal position. If a monster reaches the player first they lose the game.

    main.cpp: Contains the main and thread functions for implementing the attacking game.

    Programmer: Tori Sambrook
    Last Updated: April 11, 2020
    
*/

#include "level.h"
#include "movement.h"

int main()
{
    string levelsList[2] = {"level1.txt", "level2.txt"}; 
    pthread_t *thread_ids = new pthread_t[NUM_THREADS];

    //Use unbuffered output on stdout
    setvbuf(stdout, (char *) NULL, _IONBF, 0);

    //Set up an output lock so that threads wait their turn to speak.
    if (pthread_mutex_init(&output_lock, NULL)!=0)
    {
        perror("Could not create mutex for output: ");
        return 1;
    } 
    cout << "\nWELCOME TO THE GAME! HERE ARE THE RULES:" << endl;
    cout << "The goal of the game is to move the player 'P' and win by reaching the '*' in the last level. \n" << 
	        "If a monster 'M' reaches you first the game is over. Advance through the levels by making it \n" << 
            "to the '@' in each level. Watch out for other symbols as they might be obsticals! Enter N, S,\n" << 
            "E, or W to move the player, and Q to quit." << endl << endl;
     
    // generate threads 
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if((pthread_create(&thread_ids[i], NULL, gamePlay, &levelsList[i]) > 0))
        {
            perror("creating thread:");
            return 2;
        }
    }

    // join threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_join(thread_ids[i], NULL) != 0)
        {
            perror("trouble joining thread: ");
            return 3;
        }
    }
    removeTemp();

    if(isDead())
    {
        cout << endl << "THE PLAYER HAS DIED." << endl;
    }
    else if(quit)
    {
        cout << endl << "YOU QUIT THE GAME." << endl;
    }
    else if(win)
    {
        cout << endl << "CONGRADULATIONS! YOU HAVE WON THE GAME." << endl;
    }
    cout << "GAME OVER." << endl << endl;
		
    return 0;
}