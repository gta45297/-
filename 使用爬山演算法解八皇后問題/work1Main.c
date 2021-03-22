#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
int colLength;

#define min(i,j) (i<j)?i:j
#define max(i,j) (i>j)?i:j

void countActionProcess( int *list, int action )
{
    switch( action )
    {
    case 0://north west
        list[1] = list[1] - 1;
        list[0] = list[0] + 1;
        break;
    case 1://north
        list[0] = list[0] + 1;
        break;
    case 2://north east
        list[1] = list[1] + 1;
        list[0] = list[0] + 1;
        break;
    case 3://west
        list[1] = list[1] - 1;
        break;
    case 4://east
        list[1] = list[1] + 1;
        break;
    case 5://south west
        list[1] = list[1] - 1;
        list[0] = list[0] - 1;
        break;
    case 6://south
        list[0] = list[0] - 1;
        break;
    case 7://south east
        list[1] = list[1] + 1;
        list[0] = list[0] - 1;
    }
}

int checkCanMove( int *list )
{
    if( ( list[ 1 ] >= 0 ) && ( list[ 1 ] < colLength ) && ( list[ 0 ] >= 0 ) && ( list[ 0 ] < colLength ))
    {
        return 1;
    }
    return 0;
}

int dfsForCountQueen( int (*board)[colLength], int *list, int action )
{
    countActionProcess( list, action );
    if( checkCanMove( list ) == 1 )
    {
        int thisStageCount = board[ list[ 0 ] ][ list[ 1 ] ] == 1;
        //can dfs
        return dfsForCountQueen( board, list, action ) + thisStageCount;//list used call by address, so it cannot just put == in this place.
    }
    return 0;
}

int collsionCount( int (*board)[colLength] )
{
    int sum = 0;
    for( int i = 0; i < colLength; i++ )
    {
        for( int j = 0; j < colLength; j++ )
        {
            if( board[ i ][ j ] == 1 )//
            {
                int list[ 2 ] = { i, j };
                int tempStockList[ 2 ] = { i, j };

                for( int k = 0; k < 8; k++)
                {
                    sum += dfsForCountQueen( board, list, k );
                    list[ 0 ] = tempStockList[ 0 ];
                    list[ 1 ] = tempStockList[ 1 ];
                }
            }
        }
    }
 //   printf("\nTotalsum=%d\n", sum/2);
    return sum/2;
}

int moveQueen( int (*board)[colLength], int (*queenList)[2], int action )
{
    int chooseQueen = action/8;
    int queenAction = action%8;
    int disableBoardX = queenList[chooseQueen][1];
    int disableBoardY = queenList[chooseQueen][0];
    int setBoardX;
    int setBoardY;
    switch( queenAction )
    {
    case 0://north west
        setBoardX = disableBoardX - 1;
        setBoardY = disableBoardY + 1;
        break;
    case 1://north
        setBoardY = disableBoardY + 1;
        break;
    case 2://north east
        setBoardX = disableBoardX + 1;
        setBoardY = disableBoardY + 1;
        break;
    case 3://west
        setBoardX = disableBoardX - 1;
        break;
    case 4://east
        setBoardX = disableBoardX + 1;
        break;
    case 5://south west
        setBoardX = disableBoardX - 1;
        setBoardY = disableBoardY - 1;
        break;
    case 6://south
        setBoardY = disableBoardY - 1;
        break;
    case 7://south east
        setBoardX = disableBoardX + 1;
        setBoardY = disableBoardY - 1;
    }

    if( !( ( setBoardX >= 0 ) && ( setBoardX < colLength ) && ( setBoardY >= 0 ) && ( setBoardY < colLength ) && ( board[ setBoardY ][ setBoardX ] != 1 ) ) )
    {
        return 0;
    }
    //disable old Queen
    board[disableBoardY][disableBoardX] = 0;
    //put NEW Queen
    board[setBoardY][setBoardX] = 1;
    queenList[chooseQueen][1] =setBoardX;
    queenList[chooseQueen][0] =setBoardY;

    return 1;
}

int restoreMove( int inputAction )
{
    int choose = inputAction/8;
    int action = inputAction%8;
    action = 7-action;
    return choose*8+action;
}

int main( void )
{
    srand(time(NULL));
    // n
    printf( "What is n?\n" );
    scanf( "%d", &colLength );

    do
    {
        // queen's location
        int board[ colLength ][ colLength ];
        int queenList[ colLength ][ 2 ];
        memset( board, 0, colLength*colLength*sizeof(int) );
        // book for action
        int booked[ colLength*8 ];
        memset( booked, 0, colLength*8*sizeof(int) );
        //replace
        int minCollision = 999999999;
        // put queens
        for( int i = 0; i < colLength; i++ )
        {
            int tempForCol;
            int tempForRow;
            do
            {
                tempForCol = rand()%colLength;
                tempForRow = rand()%colLength;
            }while( board[ tempForCol ][ tempForRow ] == 1 );
            board[ tempForCol ][ tempForRow ] = 1;
            queenList[ i ][ 0 ] = tempForCol;
            queenList[ i ][ 1 ] = tempForRow;
        }

        minCollision = collsionCount( board );

        if( minCollision == 0 )
        {
            goto PRINT;
        }

        do
        {
            int action;
            do
            {
                action = rand()%(colLength*8);
                //did all action done?
                int tryActionCount = 0;
                for(int i = 0; i < colLength*8; i++ )
                {
                    tryActionCount += booked[ i ];
                }
                if( tryActionCount == colLength*8 )
                {
 //                   printf( "Cannot find the better answer\n" );
                    goto REPLACE;
                }
                //already in book?
            }while( booked[ action ] == 1 );
            //action write into book
            booked[ action ] = 1;
            //move queen
            if( moveQueen( board, queenList, action ) == 1 )
            {
                //count NEW collision
                int newBoardCollision = collsionCount( board );
                if( newBoardCollision < minCollision )
                {
                    // found better board
                    minCollision = newBoardCollision;
                    //flush book
                    memset( booked, 0, colLength*8*4 );
//                printf("Let's go to next board\n");
                }else
                if( newBoardCollision >= minCollision )
                {
 //               printf("Restore MOVE\n");
                    //restore move
                    moveQueen( board, queenList, restoreMove( action ) );
                }

                if( newBoardCollision == 0 )
                {
                PRINT:
//                    printf( " Queen is set up.\n" );
                    for(int i = 0; i < colLength; i++)
                    {
                        for( int j = 0; j <colLength; j++)
                        {
                            printf("%d", board[colLength-1-i][j] );
                            if( j == colLength-1 )
                            {
                                printf("\n");
                            }
                        }
                     }
                    goto END;
                }
            }
        }while(1);
       REPLACE:
       printf("");
    }while(1);
    END:
    system( "pause" );
    return 0;
}

