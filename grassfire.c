/*********************************************************************
* Name: grassfire.c
* Description: Implement the grassfire path planning algorithm to
*	find the shortest path in an MxN grid from a starting cell to a
*	destination cell (or determine that there is no path to the
*	destination cell). The grid is initialized by randomizing the
*	locations of obstacles, as well as randomly selecting starting
*	and destination cells.
*
*	Output is printed to the terminal, with the destination cell
*	represented by "D", the starting cell represented by "0", the
*	path cells represented by integer values in # of cells from the
*	start cell, the free cells represented by "*", and obstacles
*	represented by "x".	
*
* Author:	Najam Syed
* Date:		2017-09-19
* Version:	1
*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define NUM_ROWS 36
#define NUM_COLS 20

/* Initialize grid as external variable */
int grid[NUM_ROWS][NUM_COLS];

int destinationRow = 0;
int destinationCol = 0;

int currentDepth = 0;

/* Seed random number generator */
void initialize_number_generator(void)
{
	srand((unsigned) time(NULL));
}

void randomize_grid()
{
	initialize_number_generator();

	int startCellIndex, endCellIndex;
	/* Each cell in the grid will contain an int.
		0 is reserved for starting cell.
		-1 for an "empty" (unvisited) cell.
		-2 for the destination cell.
		-3 for an obstacle.
	*/

	startCellIndex = rand() % (NUM_ROWS * NUM_COLS);
	do {
		endCellIndex = rand() % (NUM_ROWS * NUM_COLS);
	} while(endCellIndex == startCellIndex);

	int *grid1D = &grid[0][0];
	grid1D[startCellIndex] = 0;
	grid1D[endCellIndex] = -2;

	int i,j;
	for (i = 0; i < (NUM_ROWS * NUM_COLS); i++) {
		if (i != startCellIndex && i != endCellIndex) {
			if ((rand() % 3) == 0) {
				grid1D[i] = -3;
			} else {
				grid1D[i] = -1;
			}
		}
	}
}

void print_grid(void)
{
	int i, j;
	for (i = 0; i < NUM_ROWS; i++) {
		for (j = 0; j < NUM_COLS; j++) {
			switch(grid[i][j]) {
				case -3 :
					// obstacle
					printf("x\t");
					break;
				case -2 :
					// destination
					printf("D\t");
					break;
				case -1 :
					// unvisited cell
					printf("*\t");
					break;
				case 0 :
					// starting cell
					printf("0\t");
					break;
				default :
					printf("%d\t", grid[i][j]);
			}
		}
		printf("\n");
	}
}

// Get identity of a cell. Return identity to caller.
int get_cell_info(int row, int col)
{
	// row and col represent coordinates of cell to check.
	// Check if invalid cell. If not, return its value.
	if (row >= NUM_ROWS || row < 0 || col >= NUM_COLS || col < 0) {
		return -4;	// -4 signifies this cell is invalid.
	} else {
		return grid[row][col];
	}
}

// Check adjacent cells; modify them with new depth if possible.
int modify_adjacent(int row, int col)
{
	// row, col represent coords of parent (current) cell, not cell
	// to be checked.
	bool adjacentModifiable = false;

	int cellRetVal;

	// Check right cell.
	cellRetVal = get_cell_info(row, col+1);
	if (cellRetVal == -2) {
		destinationRow = row;
		destinationCol = col+1;
		return -2;
	} else if (cellRetVal == -1) {
		grid[row][col+1] = currentDepth + 1;
		adjacentModifiable = true;
	}
		
	// Check top cell.
	cellRetVal = get_cell_info(row-1, col);
	if (cellRetVal == -2) {
		destinationRow = row-1;
		destinationCol = col;
		return -2;
	} else if (cellRetVal == -1) {
		grid[row-1][col] = currentDepth + 1;
		adjacentModifiable = true;
	}

	// Check left cell.
	cellRetVal = get_cell_info(row, col-1);
	if (cellRetVal == -2) {
		destinationRow = row;
		destinationCol = col-1;
		return -2;
	} else if (cellRetVal == -1) {
		grid[row][col-1] = currentDepth + 1;
		adjacentModifiable = true;
	}

	// Check bottom cell.
	cellRetVal = get_cell_info(row+1, col);
	if (cellRetVal == -2) {
		destinationRow = row+1;
		destinationCol = col;
		return -2;
	} else if (cellRetVal == -1) {
		grid[row+1][col] = currentDepth + 1;
		adjacentModifiable = true;
	}

	if (adjacentModifiable) {
		return 1;
	} else {
		return 0;
	}
}

// Backtrack and scrub the grid of paths not leading to destination.
void scrub_depth(int rowToKeep, int colToKeep)
{
	int i,j;
	for (i = 0; i < NUM_ROWS; i++) {
		for (j = 0; j < NUM_COLS; j++) {
			if (!(i == rowToKeep && j == colToKeep) && grid[i][j] == currentDepth) {
				grid[i][j] = -1;
			}
		}
	}
}

void backtrack_grid(void)
{
	int i, j;
	int currentRow, currentCol;
	currentRow = destinationRow;
	currentCol = destinationCol;

	scrub_depth(currentRow, currentCol);
	while (currentDepth > 0) {
		// Check right cell.
		if (get_cell_info(currentRow, currentCol+1) == currentDepth) {
			scrub_depth(currentRow, currentCol+1);
			currentCol += 1;
		// Check top cell.
		} else if (get_cell_info(currentRow+1, currentCol) == currentDepth) {
			scrub_depth(currentRow+1, currentCol);
			currentRow += 1;
		// Check left cell.
		} else if (get_cell_info(currentRow, currentCol-1) == currentDepth) {
			scrub_depth(currentRow, currentCol-1);
			currentCol -= 1;
		// Check bottom cell.
		} else if (get_cell_info(currentRow-1, currentCol) == currentDepth) {
			scrub_depth(currentRow-1, currentCol);
			currentRow -= 1;
		}
		currentDepth -= 1;
	}
}

int main(void)
{
	clock_t startTime;
	double runTime;
	startTime = clock();

	randomize_grid();

	bool continueChecking = true;
	bool destinationReached = false;
	int i, j, modifyAdjacentRetVal;
	while (continueChecking) {
		continueChecking = false;

		// Check all cells at current depth
		for (i = 0; i < NUM_ROWS; i++) {
			if (destinationReached) {
				break;
			}
			for (j = 0; j < NUM_COLS; j++) {
				if (destinationReached) {
					break;
				}
				if (grid[i][j] == currentDepth) {
					modifyAdjacentRetVal = modify_adjacent(i, j);
					if (modifyAdjacentRetVal == -2) {
						destinationReached = true;
						continueChecking = false;
					} else if (modifyAdjacentRetVal == 1) {
						continueChecking = true;
					}
				}
			}
		}
		
		currentDepth += 1;
	}

	if (destinationReached) {
		printf("Path found.");
		backtrack_grid();
	} else {
		printf("No path found.");
	}

	runTime = 1000 * (clock() - startTime) / CLOCKS_PER_SEC;
	printf(" (%.0f ms).\n\n", runTime);
	print_grid();
}
