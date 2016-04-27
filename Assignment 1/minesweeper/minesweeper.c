/*
*	comp2129 - Assignment 1
*   Minesweeper.c
*
*   Dylan Oldfield
*   University of Sydney - 440310604
*
*   This program is an adpation of the game minesweeper 
*   Game is intialised with g <width> <height>
*   10 Bombs are first place using the b <x> <y> 
*   Phase play where u <x> <y> uncovers squares and f <x> <y> flags bombs
*   Game is won if all non-bomb squares are uncovered and all bombs are flagged
*   game is lost if bomb is uncovered. Error occurs if, you try and use a move
*   on a changed square, if you run out of flags(10), if you don't put commands
*   in correctly. 
*
*   Tech Notes: Utilises a single array instead of multidimensional array
*   see attached "mine-notes.txt" for specifics on implementation
*
*   //!TEST PRINTS -> used for debugging purposes only 
*   
*/


#include <stdio.h> 
#include <stdlib.h> 
#define MAX 100
#define MIN 1

// dimension check prototype
int dcheck (int width, int height, char com, char* mine, int x, int y, int flagcount);
// state of board prototype
void state(char* mine, int width, int height, char board[]);
// bomb counter prototype
int bombs(char* mine, int width, int height, int x , int y);
// board intialisation prototype
void boardy(char* board, int width, int height);

int main(void)
{
    //fixed dimensions for board
   static int width, height; 
    // dimension markers 
    int x, y;
    // flag counter
    int flagcount = 0;
    // command character
    char com; 
    
    //check for proper number of arguments and correct data types
    if( scanf(" %c%d%d",&com, &width, &height) != 3 ) 
    {
        //! TEST PRINT
        //printf("initial scanf failure width:%d, height:%d, com:%c\n", width, height, com);
        printf("error\n");
        exit(0); 
    }
    // check for valid initial character 'g'
    if( com !='g')
    {
        //! TEST PRINT
        //printf("game initialisation command wrong com: %c\n", com);
        printf("error\n");
        exit(0); 
    }
    
    //check for valid dimensions 
    if( width < MIN || width > MAX || height < MIN || height > MAX || height * width < 10)
    {
        //! TEST PRINT
        //printf("initial dimension failure width: %d , height: %d\n", width, height);
        printf("error\n");
        exit(0); 
    }
    // print game statement
    printf("%c %d %d\n", com, width, height);
    //initialize minesweeper matrix 
    char mine[width * height];
    //creates border for board
    char board[(width + 2) * (height + 2)];
    boardy(board,width, height);
    
    // intialises minemine,  use 'x' to hold unchecked coordinates
    for(int i = 0; i < height; i++ )
    {
        for(int j = 0; j < width; j++ )
        {
            mine[width * i + j] = 'x'; 
        }
    }
    //state(mine, width, height, board);
  
    //populate with bombs 
    for(int i = 0; i < 10; i++)
    {
        if( scanf(" %c%d%d",&com, &x, &y) != 3)
        {
            //! TEST PRINT
            //printf("scanf fail during bomb phase\n");
            printf("error\n");
            exit(0); 
        }
        //check valid dimensions
        else if( dcheck(width, height, com, mine, x, y, flagcount) != 0)
        {
            //! TEST PRINT
            //printf("dcheck fail during bomb phase\n");
            printf("error\n");
            exit(0); 
        }
        // check valid command
        else if( com != 'b')
        {
            //! TEST PRINT
            //printf("command for bomb not found in bomb adding\n");
            printf("error\n");
            exit(0);
        }
        else
        {
            mine[width * y + x] = 'b';
            printf("%c %d %d\n",com, x, y);
        }
        
    }
    // prints current board 
    state(mine, width, height, board);
    
    //uncover or flag turns given n amount of turns where n = width * height 
    for(int i = 0; i  < width * height; i++)
    {
        if( scanf(" %c%d%d",&com, &x, &y) != 3)
        {
            //! TEST PRINT
            //printf("scanf during placement\n");
            printf("error\n");
            exit(0); 
        }
        
        int check = dcheck(width, height, com, mine, x, y,flagcount);

        if( check != 0)
        {
            // returns 4 if site has a bomb and is flagged 
            if( check == 4 )
            {
            mine[width * y + x] = 'k';
            flagcount++;
            printf("%c %d %d\n",com, x, y);
            }
            else if(check == 3)
            {
            printf("lost\n");
            exit(0); 
            }
            else
            {
            //! TEST PRINT
            //printf("dcheck fail during bomb phase\n");
            printf("error\n");
            exit(0); 
            }
        }
        //check valid dimensions
       
         else if( com != 'u' && com !='f')
        {
            //! TEST PRINT
            //printf("not valid placement command\n");
            printf("error\n");
            exit(0);
        }
        else if ( com == 'u')
        {
            mine[width * y + x] = bombs(mine, width, height, x, y);
            printf("%c %d %d\n",com, x, y);
        }
        else
        {
            mine[width * y + x] = 'f';
            printf("%c %d %d\n",com, x, y);
        }
        state(mine, width, height, board);
    }
    
    //If all steps are made correctly then game is won 
    printf("won\n");
    exit(0);
    
    
    return 0;
   
}
// this function initialises the board to match dimensions
void boardy(char* board, int width, int height)
{
    int bh = height + 2;
    int bw = width + 2;
    for (int i = 0; i < bh; i++)
    {
        for(int j = 0; j < bw; j++)
        {
            if((i == 0 && j == 0) || (i == bh - 1 && j == 0) ||( i == 0 && j == bw - 1) || (i == bh - 1 && j == bw - 1))
            {
                board[bw * i + j] = '+';
            }
            else if((i == 0 || i == bh - 1) && j > 0 && j < bw - 1)
            {
                board[bw *i + j] = '-';
            }
            else if((j == 0 || j == bw - 1) && i > 0 && i < bh - 1)
            {
                board[bw * i + j] = '|';
            }
            else 
            {
                board[bw * i + j] = '*';
            }
        }
    }
    return; 
}


// This fuction counts the adjacent bombs of an unveiled cell 
int bombs(char* mine, int width, int height, int x , int y)
{
    //ASCII value for 0
    int count = 48;
    //corner case: if width = 1 matrix
    if(x == 0 && width == 1)
    {
        if(y == 0)
        {
            if(mine[width * (y + 1) + x] == 'b' || mine[width * (y + 1) + x] == 'k')
            {
                count++;
            }
            return count;
        }
        else if (y > 0 && y < height - 1)
        {
            if(mine[width * (y + 1) + x] == 'b' || mine[width * (y + 1) + x] == 'k')
            {
                count++;
            }
            if(mine[width * (y - 1) + x] == 'b' || mine[width * (y - 1) + x] == 'k')
            {
                count++;
            }
            return count;
        }
        else  if(y == height - 1 )
        {
            if(mine[width * (y - 1) + x] == 'b' || mine[width * (y - 1) + x] == 'k')
            {
                count++;
            }
            return count;
        }
    }
    
    //corner case: if height = 1 matrix
    if(y == 0 && height == 1)
    {
        if(x == 0)
        {
            if(mine[width * y + (x + 1)] == 'b' || mine[width * y + (x + 1)] == 'k')
            {
                count++;
            }
            return count;
        }
        else if (x > 0 && x < width - 1)
        {
            if(mine[width * y + (x + 1)] == 'b' || mine[width * y + (x + 1)] == 'k')
            {
                count++;
            }
            if(mine[width * y + (x - 1)] == 'b' || mine[width * y + (x - 1)] == 'k')
            {
                count++;
            }
            return count;
        }
        else  if(x == width - 1 )
        {
            if(mine[width * y + (x - 1)] == 'b' || mine[width * y + (x - 1)] == 'k')
            {
                count++;
            }
            return count;
        }
    }
    // width && height > 1
    if(width > 1 && height > 1)
    {
        // top left corner 
        if(x == 0 && y == 0)
        {
            if(mine[width * y + (x + 1)] == 'b' || mine[width * y + (x + 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y + 1) + x] == 'b' || mine[width * (y + 1) + x] == 'k')
            {
                count++;
            }
            if(mine[width * (y + 1) + (x + 1)] == 'b' || mine[width * (y + 1) + (x + 1)] == 'k')
            {
                count++;
            }
            return count; 
        }
        // top right corner
        if(x == width - 1 && y == 0)
        {
            if(mine[width * y + (x-1)] == 'b' || mine[width * y + (x - 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y + 1) + x] == 'b' || mine[width * (y + 1) + x] == 'k')
            {
                count++;
            }
            if(mine[width * (y + 1) + (x - 1)] == 'b' || mine[width * (y + 1) + (x - 1)] == 'k')
            {
                count++;
            }
            return count; 
        }
        // bottom left corner
        if(x == 0 && y == height - 1)
        {
            if(mine[width * y + (x + 1)] == 'b' || mine[width * y + (x + 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y - 1) + x] == 'b' || mine[width * (y - 1) + x] == 'k')
            {
                count++;
            }
            if(mine[width * (y - 1) + (x + 1)] == 'b' ||mine[width * (y - 1) + (x + 1)] == 'k')
            {
                count++;
            }
            return count; 
        }
        // bottom right corner 
        if(x == width - 1 && y == height - 1)
        {
            if(mine[width * y + (x - 1)] == 'b' || mine[width * y + (x - 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y - 1) + x] == 'b' || mine[width * (y - 1) + x] == 'k')
            {
                count++;
            }
            if(mine[width * (y - 1) + (x - 1)] == 'b' || mine[width * (y - 1) + (x - 1)] == 'k')
            {
                count++;
            }
            return count; 
        }
        // top side non-corner 
        if( y == 0 && x > 0 && x < width - 1)
        {
            if(mine[width * y + (x - 1)] == 'b' || mine[width * y + (x - 1)] == 'k')
            {
                count++;
            }
            if(mine[width * y + (x + 1)] == 'b' || mine[width * y + (x + 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y + 1) + x] == 'b' || mine[width * (y + 1) + x] == 'k')
            {
                count++;
            }
            if(mine[width * (y + 1) + (x + 1)] == 'b' || mine[width * (y + 1) + (x + 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y + 1) + (x - 1)] == 'b' || mine[width * (y + 1) + (x - 1)] == 'k')
            {
                count++;
            }
            return count;
        }
        // left side non-corner
        if( x == 0 && y > 0 && y < height - 1)
        {
            if(mine[width * (y - 1) + x] == 'b' || mine[width * (y - 1) + x] == 'k')
            {
                count++;
            }
            if(mine[width * (y + 1) + x] == 'b' || mine[width * (y + 1) + x] == 'k')
            {
                count++;
            }
            if(mine[width * y + (x + 1)] == 'b' || mine[width * y + (x + 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y + 1) + (x + 1)] == 'b' || mine[width * (y + 1) + (x + 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y - 1) + (x + 1)] == 'b' || mine[width * (y - 1) + (x + 1)] == 'k')
            {
                count++;
            }
            return count;
        }
        // bottom side non-corner 
        if( y == height - 1 && x > 0 && x < width - 1)
        {
            if(mine[width * y + (x - 1)] == 'b' || mine[width * y + (x - 1)] == 'k')
            {
                count++;
            }
            if(mine[width * y + (x + 1)] == 'b' || mine[width * y + (x + 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y - 1) + x] == 'b' || mine[width * (y - 1) + x] == 'k')
            {
                count++;
            }
            if(mine[width * (y - 1) + (x + 1)] == 'b' || mine[width * (y - 1) + (x + 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y - 1) + (x - 1)] == 'b' || mine[width * (y - 1) + (x - 1)] == 'k')
            {
                count++;
            }
            return count;
        }
        // right side non-corner
        if( x == width - 1 && y > 0 && y < height - 1)
        {
            if(mine[width * (y - 1) + x] == 'b' || mine[width * (y - 1) + x] == 'k')
            {
                count++;
            }
            if(mine[width * (y + 1) + x] == 'b' || mine[width * (y + 1) + x] == 'k')
            {
                count++;
            }
            if(mine[width * y + (x - 1)] == 'b' || mine[width * y + (x - 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y + 1) + (x - 1)] == 'b' || mine[width * (y + 1) + (x - 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y - 1) + (x - 1)] == 'b' || mine[width * (y - 1) + (x - 1)] == 'k')
            {
                count++;
            }
            return count;
        }
        // non-edge coordinate
        if( (x < width - 1 && x > 0) && (y > 0 && y < height - 1))
        {
            if(mine[width * (y - 1) + (x - 1)] == 'b' || mine[width * (y - 1) + (x - 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y - 1) + x] == 'b' || mine[width * (y - 1) + x] == 'k')
            {
                count++;
            }
            if(mine[width * (y - 1) + (x + 1)] == 'b' || mine[width * (y - 1) + (x + 1)] == 'k')
            {
                count++;
            }
            if(mine[width * y + (x - 1)] == 'b' || mine[width * y + (x - 1)] == 'k')
            {
                count++;
            }
            if(mine[width * y + (x + 1)] == 'b' || mine[width * y + (x + 1)] == 'k')
            {
                count++;
            }
             if(mine[width * (y + 1) + (x - 1)] == 'b' || mine[width *(y + 1) + (x - 1)] == 'k')
            {
                count++;
            }
            if(mine[width * (y + 1) + x] == 'b' || mine[width *(y + 1) + x] == 'k')
            {
                count++;
            }
            if(mine[width * (y + 1) + (x + 1)] == 'b' || mine[width * (y + 1) + (x + 1)] == 'k')
            {
                count++;
            }
            return count;
        }
    }
    return count;
}

//this functions checks to see if valid spot or found bomb
int dcheck(int width, int height, char com, char* mine, int x, int y, int flagcount)
{
    //check x coordinate
    if( x >= 0 && x < width)
    {
        // check y coordinate
        if( y >= 0 && y < height)
        {
            // check valid command 
            if(com == 'b' || com == 'u' || com == 'f')
            {
                // check for uncheck coordinate
                if(mine[width * y + x] == 'x')
                {
                    // flagged bomb 
                    if ( com == 'f')
                    {
                        if(flagcount < 10)
                        {
                            //! TEST PRINT
                            //printf("bomb flagged\n");
                            return 4;
                        }
                        else
                        {
                            return 1;
                        }
                    }

                    //! TEST PRINT
                    //printf("dcheck success found x\n");
                    return 0;
                }
                // checks for bomb in location
                else if(mine[width * y + x] == 'b')
                {
                    // flagged bomb 
                    if ( com == 'f')
                    {
                        if(flagcount < 10)
                        {
                            //! TEST PRINT
                            //printf("bomb flagged\n");
                            return 4;
                        }
                        else
                        {
                            return 1;
                        }
                    }
                    // lost game 
                    else
                    {
                    //! TEST PRINT
                    //printf("dcheck found b returned 1\n");
                    return 3;
                    }
                }
                // square has already been utilised 
                else if(mine[width * y + x] == 'f' || mine[width * y + x] =='u')
                {
                    //! TEST PRINT
                    //printf("coordinate not empty, value:%c\n",mine[width * y + x]);
                    return 1;
                }
                else
                {
                    //! TEST PRINT
                    //printf("unknown break within command check \n");
                    return 5;
                }
            }
            else
            {
                //! TEST PRINT
                //printf("command check failure\n");
                return 5;
            }
        }
    }
   
    //! TEST PRINT
    //printf("x y coordinate failure x: %d, y: %d\n", x, y);
    return 1;
}


// this function returns an upated state of the current board 
void state(char* mine, int width, int height, char* board)
{
    int bw = width + 2;
    int bh = height + 2;
    
    for (int i = 0; i < height; i++ )
    {
        for (int j = 0; j < width; j++)
        {
            if(mine[width * i + j] == 'f' || mine[width * i + j] == 'k')
            {
                board[bw * (i+1) + (j+1)] = 'f';
            }
            else if(mine[width * i + j] == 'b' || mine[width * i + j] == 'x')
            {
               board[bw * (i+1) + (j+1)] = '*';
            }
            else
            {
                board[bw * (i+1) + (j+1)] = mine[width * i + j];
            }
        }
    }
    
    for(int i = 0; i < bh; i ++)
    {
        for(int j = 0; j < bw; j++)
        {
            char c = board[bw * i + j];
            putchar(c);
        }
        printf("\n");
    }
    
}
