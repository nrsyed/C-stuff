/********************************************************************
 * Name: knightstour.c
 * Description: Program that determines the path of a knight across an
 *		MxN chessboard. "Knight's tour" refers to an exercise in
 *		which a knight must land on every square of a chessboard
 *		without landing on any square more than once. This program
 *		uses a brute force approach to determine the knight's path
 *		from a designated starting square, and prints the chessboard
 *		to the console with each square represented by an integer
 *		indicating the order of the knight's moves (ie, "1" represents the
 *		starting square, "2" represents the next square, and so on).
 *		If no solution is found, a message to that effect will be printed.
 *		
 *		Since the numbers of ranks and files are set separately, this
 *		program will solve asymmetric board shapes.
 *
 *		NOTE: Being a brute force approach, this algorithm will take
 *			a prohibitively long time to solve standard or large
 *			boards. It is better suited to smaller board sizes.
 *
 * Author:	Najam Syed
 * Date:	2016-Oct-30
 * Version:	1
 ********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define NUM_FILES 7
#define NUM_RANKS 7

// Rank and file indices range from 0 to N-1
#define INITIAL_FILE 0
#define INITIAL_RANK 0

// Array to determine possible knight moves; declared outside functions
// to quickly allow all instances of the "jump" function to access without
// creating local arrays within each recursive instance.
int jumpCoords[4] = {1, -1, 2, -2};

// Function to check if intended jump is valid. Returns false if not, otherwise true.
bool jumpValid(int visitedSpaces[][NUM_RANKS], int currentFile, int currentRank, int jumpFiles, int jumpRanks)
{
	int newFile = currentFile + jumpFiles;
	int newRank = currentRank + jumpRanks;

	// Return false if square has already been visited, or if it lies outside the bounds of the board
	if (visitedSpaces[newFile][newRank] != 0 || newFile < 0 || newFile >= NUM_FILES
	 || newRank < 0 || newRank >= NUM_RANKS)
		return false;

	return true;
}

// Function to check whether all squares have been visited.
bool allVisited(int visitedSpaces[][NUM_RANKS])
{
	int file, rank;
	for (file = 0; file < NUM_FILES; file++) {
		for (rank = 0; rank < NUM_RANKS; rank++) {
			if (visitedSpaces[file][rank] == 0)
				return false;
		}
	}
	return true;
}

// Function that prints the board of visited spaces.
void printBoard(int visitedSpaces[][NUM_RANKS])
{
	int file, rank;

	// Note: prints the ranks backwards so the board is printed like an
	// actual board viewed from white's perspective, ie square A1 at
	// lower left corner, A8 at lower right, etc.
	for (rank = NUM_RANKS-1; rank >= 0; rank--) {
		for (file = 0; file < NUM_FILES; file++) {
			printf("%d\t", visitedSpaces[file][rank]);
		}
		printf("\n");
	}
	printf("\n");
}

int jump(int visitedSpaces[][NUM_RANKS], int lastDepth, int lastFile, int lastRank,
	int filesJumped, int ranksJumped)
{
	int depth = lastDepth + 1;
	int currentFile = lastFile + filesJumped;
	int currentRank = lastRank + ranksJumped;
	visitedSpaces[currentFile][currentRank] = depth;
	
	if (allVisited(visitedSpaces) == true)
		return depth;
	
	int returnDepth;	// Depth returned by next recursive instance of jump(...).
				// Will be either the final depth (total number of squares)
				// or -1; if -1, signals the calling instance of jump(...)
				// to backtrack.

	// Try all possible knight moves by doubly iterating through the
	// jumpCoords array, which contains {1, -1, 2, -2}; absolute value is used
	// to skip combinations like (1,1), (1,-1), (-2,-2), etc.
	int filesToJump, ranksToJump;
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			filesToJump = jumpCoords[i];
			ranksToJump = jumpCoords[j];
			if (abs(filesToJump) == abs(ranksToJump)) {
				continue;
			} else {
				if (jumpValid(visitedSpaces, currentFile, currentRank, filesToJump, ranksToJump))
				{
					returnDepth = jump(visitedSpaces, depth, currentFile, currentRank,
						filesToJump, ranksToJump);
					if (returnDepth > 0)
						return returnDepth;
				}
			}
		}
	}
	
	visitedSpaces[currentFile][currentRank] = 0;	// Backtracking
	return -1;	// Return val -1 informs calling instance that this path failed
}

int main(void)
{
	clock_t start_time, end_time;
	double run_time;
	start_time = clock();

	int visitedSpaces[NUM_FILES][NUM_RANKS];
	int file, rank;

	// Initialize visitedSpaces[][] to zeros.
	for (file = 0; file < NUM_FILES; file++) {
		for (rank = 0; rank < NUM_RANKS; rank++) {
			visitedSpaces[file][rank] = 0;
		}
	}

	int currentFile = INITIAL_FILE;
	int currentRank = INITIAL_RANK;
	int numMoves;

	numMoves = jump(visitedSpaces, 0, currentFile, currentRank, 0, 0);

	if (numMoves > 0) {
		printBoard(visitedSpaces);
	} else {
		printf("No solution found.\n");
	}

	end_time = clock();
	run_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
	printf("Run time: %f\n", run_time);

	return 0;
}
