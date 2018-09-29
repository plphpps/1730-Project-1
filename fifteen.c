/**************************************************************************
 * This work originally copyright David J. Malan of Harvard University, or-
 * iginally released under:
 * CC BY-NC-SA 3.0  http://creativecommons.org/licenses/by-nc-sa/3.0/
 * licensing.
 *
 * It has been adapted for use in csci 1730.  Per the share-alike
 * clause of this license, this document is also released under the
 * same license.
 **************************************************************************/
/**
 * fifteen.c
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
int move(int tile);
int won(void);

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE* file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();

    // accept moves until game is won
    while (1)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won())
        {
            printf("win!\n");
            break;
        }

        // prompt for move
        printf("Tile to move (0 to exit): ");
        int tile;
	scanf("%d", &tile);
	getchar();
        
        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(50000);
        }

        // sleep thread for animation's sake
        usleep(50000);
    }
    
    // close log
    fclose(file);

    // success
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(200000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).  
 */
void init(void)
{
  int count = d * d - 1;

  // Starting from d*d - 1 (the highest value on the board) assign the value to each element on the board. The blank space on the board is represented by a zero in this case.
  for(int row = 0; row < d; row++){
    for(int col = 0; col < d; col++){
      // If the board contains an odd number of tiles (i.e. the height and width of the board are even), switch the positions of the values 1 and 2.
      if(d % 2 == 0 && count == 2){
	board[row][col] = 1;
	board[row][col+1] = 2;
	board[row][col+2] = 0;
	break;
      }
      board[row][col] = count;
      count--;
    }
  }
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
  // Go through each element array printing out the values. If the value is equal to zero (the value representing the blank space), then print a underscore instead of the value.
  for(int row = 0; row < d; row++){
    for(int col = 0; col < d; col++){
      if(board[row][col] == 0)
	printf(" _ ");
      else
	printf("%2d ", board[row][col]);
    }
    putchar('\n');
  }
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
int move(int tile)
{
  if(tile <= 0 && tile >= d*d)
    return 0;

  for(int row = 0; row < d; row++){
    for(int col = 0; col < d; col++){
      // Check to see if the element at (row,col) is equal to the user's tile
      if(board[row][col] == tile){
	// Check to see if the selected tile's adjacent tiles contains the blank space.
	// If it does then swap the values of the blank space and the tile.

	// Above
	if(row > 0 && board[row - 1][col] == 0){
	  board[row - 1][col] = board[row][col];
	  board[row][col] = 0;
	  return 1;
	}
	
	// Below
	if(row + 1 < d && board[row + 1][col] == 0){
	  board[row + 1][col] = board[row][col];
	  board[row][col] = 0;
	  return 1;
	}
	
	// Left
	if(col > 0 && board[row][col - 1] == 0){
	  board[row][col - 1] = board[row][col];
	  board[row][col] = 0;
	  return 1;
	}

	// Right
	if(col + 1 < d && board[row][col + 1] == 0){
	  board[row][col + 1] = board[row][col];
	  board[row][col] = 0;
	  return 1;
	}
      }
    }
  }

  // Return false if the tile provided was not adjacent to a blank space.
  return 0;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
int won(void)
{
  int count = 1;

  // Check to see if the board has been correctly sorted by comparing the value at each element of the board to the count.
  for(int row = 0; row < d; row++){
    for(int col = 0; col < d; col++){
      // If we've made it to the final element of the board (the blank space)
      // set the count to zero so it can be checked against the board's value.
      if(count == d*d)
	count = 0;
      
      // If value is not equal to the count, then the board is not sorted correctly and the user has not won.
      if(board[row][col] != count)
	return 0;

      // Increment the count to match what the next value of the solved board should be.
      count++;
    }
  }

  // Return true if the user has correctly sorted the board.
  return 1;
}
