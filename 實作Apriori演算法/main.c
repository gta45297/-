#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_HASH_LENGTH 2000
#define MAX_QUEUE_LENGTH 1000000  //for taskQueue
#define MAX_INPUT_LENGTH 70
#define MAX_OUTPUT_LENGTH 70
#define MAX_SEEK_LENGTH 10000000
#define MAX_WRITE_LENGTH 100
#define MAX_CONVERT_TO_NUM_LENGTH 100

#define SUP 4

typedef struct HashList
{
    int dead;
    int deadPool[ MAX_HASH_LENGTH ];
    int num;
    struct HashList *nextHashList[ MAX_HASH_LENGTH ];
    int outputBufferLastIndex;
    int outputBuffer[ MAX_OUTPUT_LENGTH ];
    int hashLevel;
    int hashSeekSet;
} HashList_SIZE, *HashList_TYPE;

void initNewHashList( HashList_TYPE inputHash )
{
    memset( inputHash, 0, sizeof( HashList_SIZE ) );
}

void delDataLabel( FILE *file, int *buffer )
{
    fread( buffer, 8, 1, file);
}

int readDataSize( FILE *file )
{
    int dataSize;
    fread( &dataSize, 4, 1, file);
    return dataSize;
}

void readDataList( FILE *file, int *buffer, int wantSize )
{
    fread( buffer, wantSize * sizeof( int ), 1, file );
}

void detectInputBufferOverflow( int inputSize )
{
    if( inputSize > MAX_INPUT_LENGTH - 1 )
    {
        printf( "ERROR, Input Buffer is not big enough, want:\t%d\n", inputSize );
        system( "pause" );
        for(;;);
    }
}

void detectHashListOverflow( int inputData )
{
    if( inputData > MAX_HASH_LENGTH - 1 )
    {
        printf( "ERROR, HashList is not big enough, want:\t%d\n", inputData );
        system( "pause" );
        for(;;);
    }
}

typedef struct BaseTaskQueue
{
    HashList_TYPE gotoHashList;
} *taskQueue_TYPE, taskQueue_SIZE, *seekNumQueue_TYPE, seekNumQueue_SIZE;

void initSeekNumQueue( seekNumQueue_TYPE inputAddress )
{
    memset( inputAddress, 0, sizeof( inputAddress ) * MAX_SEEK_LENGTH );
}

void detectTaskQueueOverflow( int inputIndex )
{
    if( inputIndex > MAX_QUEUE_LENGTH - 1 )
    {
        printf( "ERROR, taskQueue is not big enough, want:\t%d\n", inputIndex );
        system( "pause" );
        for(;;);
    }
}

void detectSeekNumQueueOverflow( int inputIndex )
{
    if( inputIndex > MAX_SEEK_LENGTH - 1 )
    {
        printf( "ERROR, seekQueue is not big enough, want:\t%d\n", inputIndex );
        system( "pause" );
        for(;;);
    }
}

void detectOutputBufferOverflow( int inputIndex )
{
    if( inputIndex > MAX_OUTPUT_LENGTH - 1 )
    {
        printf( "ERROR, output buffer is not big enough, want:\t%d\n", inputIndex );
        system( "pause" );
        for(;;);
    }
}

void detectWriteBufferOverflow( int inputIndex )
{
    if( inputIndex > MAX_WRITE_LENGTH - 1 )
    {
        printf( "ERROR, write buffer is not big enough, want:\t%d\n", inputIndex );
        system( "pause" );
        for(;;);
    }
}

void detectConvertBufferOverflow( int inputIndex )
{
    if( inputIndex > MAX_CONVERT_TO_NUM_LENGTH - 1 )
    {
        printf( "ERROR, convert buffer is not big enough, want:\t%d\n", inputIndex );
        system( "pause" );
        for(;;);
    }
}

int mergeChar( char* outputBuffer, int outputBufferIndex, char *charSlice )
{
    int i = 0;
    for( ; charSlice[ i ] != '*' ; i++ )
    {
        outputBuffer[ outputBufferIndex + i ] = charSlice[ i ];
    }
    return outputBufferIndex + i;
}

int main( void )
{
    //for No.3 ans
    unsigned long start = clock();
    //for No.1 ans
    FILE *file1 = fopen( "output.txt", "w" );
    //for No.2 ans
    unsigned int noTwoAns = 0;

    //load file
    FILE *file = fopen( "T10I10N0.1KD1K.data", "rb" );
    //init RootHash
    HashList_TYPE HashRoot = malloc( sizeof( HashList_SIZE ) );
    initNewHashList( HashRoot );
    //init seekNumQueue
    seekNumQueue_TYPE seekNumQueue = malloc( sizeof( seekNumQueue_SIZE ) * MAX_SEEK_LENGTH );
    initSeekNumQueue( seekNumQueue );
    // init inputBuffer
    int inputDataBuffer[ MAX_INPUT_LENGTH ] = { 0 };
    //init queue
    taskQueue_TYPE taskQueue = malloc( sizeof( taskQueue_SIZE ) * MAX_QUEUE_LENGTH );

    int targetLevel = 0;

    for(;;)
    {
        //status
        printf( "L%d......\n", targetLevel );

        rewind( file );
        int seekNumQueueIndex = 0;
        initSeekNumQueue( seekNumQueue );
        // load title
        while( fread( inputDataBuffer, 8, 1, file ) != 0 )
        {
            int dataSize = readDataSize( file );
            detectInputBufferOverflow( dataSize );

            int taskQueueIndex = 0;
            int lastTaskIndex = 0;

            readDataList( file, inputDataBuffer, dataSize );

            for( int i = 0; i < dataSize; i++ )
            {
                int targetItem = inputDataBuffer[ i ];
                detectHashListOverflow( targetItem );

                int iForTaskQueue = 0;

                for( ; iForTaskQueue < taskQueueIndex; iForTaskQueue++ )
                {
                    HashList_TYPE sourceHash = taskQueue[ iForTaskQueue ].gotoHashList;
                    //check dead
                    if( sourceHash->deadPool[ targetItem ] == 0 )
                    {
                        //create hashlist if not created
                        if( sourceHash->nextHashList[ targetItem ] == 0 )
                        {
                            //下層Level未創立
                            HashList_TYPE newHash = malloc( sizeof( HashList_SIZE ) );
                            initNewHashList( newHash );
                            //連結
                            sourceHash->nextHashList[ targetItem ] = newHash;
                            //set Level
                            newHash->hashLevel = sourceHash->hashLevel + 1;
                            // move Output Buffer & add new output buffer
                            memcpy( newHash->outputBuffer, sourceHash->outputBuffer, sourceHash->outputBufferLastIndex * sizeof( int ) );
                            newHash->outputBufferLastIndex = sourceHash->outputBufferLastIndex;
                            newHash->outputBuffer[ newHash->outputBufferLastIndex ] = targetItem;
                            //detect overflow
                            detectOutputBufferOverflow( newHash->outputBufferLastIndex + 1 );
                            newHash->outputBufferLastIndex = newHash->outputBufferLastIndex + 1;
                        }else
                        {
                            //nextHash is alive but need die
                            if( sourceHash->nextHashList[ targetItem ]->dead == 1 )
                            {
                                sourceHash->deadPool[ targetItem ] = 1;
                                free( sourceHash->nextHashList[ targetItem ] );
                                sourceHash->nextHashList[ targetItem ] = 0;
                                //goto end
                                continue;
                            }
                        }
                         //process
                        HashList_TYPE destinationHash = sourceHash->nextHashList[ targetItem ];

                        if( destinationHash->hashLevel < targetLevel )
                        {
                            detectTaskQueueOverflow( lastTaskIndex + 1 );

                            taskQueue[ lastTaskIndex ].gotoHashList = destinationHash;
                            lastTaskIndex++;
                        }else if( destinationHash->hashLevel == targetLevel )
                        {
                            if( destinationHash->hashSeekSet == 0 )
                            {
                                destinationHash->hashSeekSet = 1;
                                //insert seekQueue
                                seekNumQueue[ seekNumQueueIndex ].gotoHashList = destinationHash;

                                detectSeekNumQueueOverflow( seekNumQueueIndex + 1 );
                                seekNumQueueIndex++;
                            }
                            destinationHash->num++;
                        }
                    }
                }
                if( HashRoot->deadPool[ targetItem ] == 0 )
                {
                    //create if not found
                    if( HashRoot->nextHashList[ targetItem ] == 0 )
                    {
                        //下層Level未創立
                        HashList_TYPE newHash = malloc( sizeof( HashList_SIZE ) );
                        initNewHashList( newHash );
                        //連結
                        HashRoot->nextHashList[ targetItem ] = newHash;
                        //set Level
                        newHash->hashLevel = 0;
                        // move Output Buffer & add new output buffer
                        newHash->outputBuffer[ 0 ] = targetItem;
                        newHash->outputBufferLastIndex = 1;
                    }else
                    {
                        if( HashRoot->nextHashList[ targetItem ]->dead == 1 )
                        {
                            HashRoot->deadPool[ targetItem ] = 1;
                            free( HashRoot->nextHashList[ targetItem ] );
                            HashRoot->nextHashList[ targetItem ] = 0;
                            //goto end
                            continue;
                        }
                    }
                    //count
                    HashList_TYPE destinationHash = HashRoot->nextHashList[ targetItem ];

                    if( destinationHash->hashLevel < targetLevel )
                    {
                        detectTaskQueueOverflow( lastTaskIndex + 1 );

                        taskQueue[ lastTaskIndex ].gotoHashList = destinationHash;
                        lastTaskIndex++;
                    }else if( destinationHash->hashLevel == targetLevel )
                    {
                        if( destinationHash->hashSeekSet == 0 )
                        {
                            destinationHash->hashSeekSet = 1;
                            //detect overflow
                            detectSeekNumQueueOverflow( seekNumQueueIndex + 1 );
                            //insert seekQueue
                            seekNumQueue[ seekNumQueueIndex ].gotoHashList = destinationHash;
                            seekNumQueueIndex++;
                        }
                        destinationHash->num++;
                    }
                }
                taskQueueIndex = lastTaskIndex;
            }
        }
        //count num & set dead
        int processEnd = 1;
        for( int i = 0; i < seekNumQueueIndex; i++ )
        {
            if( seekNumQueue[ i ].gotoHashList->num < SUP )
            {
                seekNumQueue[ i ].gotoHashList->dead = 1;
            }else
            {
                processEnd = 0;
                //num>1 print
                int firstOutput = 1;
                HashList_TYPE outputTarget = seekNumQueue[ i ].gotoHashList;

                char outputToFileArray[ MAX_WRITE_LENGTH ];
                int writeToFileIndex = 0;
                char numToCharBuffer[ MAX_CONVERT_TO_NUM_LENGTH ];

                for( int j = 0; ; j++ )
                {
                    //direct output on the screen
                    /*
                    printf( "%4d", outputTarget->outputBuffer[ j ] );
                    firstOutput = 0;
                    //patch for format
                    if( firstOutput == 0 )
                    {
                        if( ( j + 1 ) < ( outputTarget->outputBufferLastIndex ) )
                        {
                            printf( "," );
                        }else
                        {
                            printf( ":%d" ,outputTarget->num );
                            break;
                        }
                    }*/
                    // write buffer to file
                    snprintf( numToCharBuffer, sizeof( numToCharBuffer ), "%d*", outputTarget->outputBuffer[ j ] );
                    writeToFileIndex = mergeChar( outputToFileArray, writeToFileIndex, numToCharBuffer );
                    //printf( "%4d", outputTarget->outputBuffer[ j ] );
                    firstOutput = 0;
                    //patch for format
                    if( firstOutput == 0 )
                    {
                        if( ( j + 1 ) < ( outputTarget->outputBufferLastIndex ) )
                        {
                            outputToFileArray[ writeToFileIndex ] = ',';
                            writeToFileIndex++;
                            //printf( "," );
                        }else
                        {
                            outputToFileArray[ writeToFileIndex ] = ':';
                            writeToFileIndex++;
                            snprintf( numToCharBuffer, sizeof( numToCharBuffer ), "%d*", outputTarget->num );
                            writeToFileIndex = mergeChar( outputToFileArray, writeToFileIndex, numToCharBuffer );
                            //printf( ":%d" ,outputTarget->num );
                            break;
                        }
                    }
                }
                outputToFileArray[ writeToFileIndex ] = '\n';
                writeToFileIndex++;
                detectWriteBufferOverflow( writeToFileIndex );
                //start write
                fwrite( (char*) outputToFileArray, writeToFileIndex, 1, file1 );

                //for No.2 ans
                noTwoAns++;
            }
        }
        if( processEnd == 1 )
        {
            break;
        }
        targetLevel++;
    }
    fclose( file );
    fclose( file1 );
    //for No.2 ans
    printf( "2. Total num =%u\n", noTwoAns );
    //for No.3 ans
    unsigned long end = clock();
    printf( "3. Total time=%lu mseconds\n", end-start );
    system("pause");
    return 0;
}
