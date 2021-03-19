//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define true 1
#define false 0
//Screen dimension constants
const int SCREEN_WIDTH = 371;  //753  371
const int SCREEN_HEIGHT = 268; //544  268

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//get image format
SDL_PixelFormat *fmt;
//
SDL_Surface *surface;

Uint32 temp, pixel;

SDL_Color *color;

SDL_Renderer* renderer;

typedef struct pixelCore
{
    int group;
    struct pixelCore *nextPixelGroup;
    struct pixelCore *nextPixelGroupRear;
    struct pixelCore *head;
    struct pixelCore *next;
    struct pixelCore *rear;
    int x;
    int y;
    int dead;
}pixelNode_Size, *pixelNode_Type;

typedef struct BaseArray
{
    pixelNode_Type gotoNode;
}relatedArray_Size, *relatedArray_Type;

unsigned int groupArrayIndex = 0;

double distanceProcess( pixelNode_Type inputPixelNode1, pixelNode_Type inputPixelNode2 )
{
    int powAns = ( inputPixelNode1->x - inputPixelNode2->x ) * ( inputPixelNode1->x - inputPixelNode2->x ) + ( inputPixelNode1->y - inputPixelNode2->y ) * ( inputPixelNode1->y - inputPixelNode2->y );
    return sqrt( powAns );
}

int init()
{   //Initialization flag
    int success = true;
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {   //Create window
        gWindow = SDL_CreateWindow( "DBSCAN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {   //Get window surface
            renderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_SOFTWARE );
        }
    }
    return success;
}

int loadMedia()
{   //Loading success flag
    int success = true;
    //Load splash image
    surface = SDL_LoadBMP( "input.bmp" );
    if( surface == NULL )
    {
        printf( "Unable to load image %s! SDL Error: %s\n", "blue.bmp", SDL_GetError() );
        success = false;
    }
    return success;
}

void close()
{
    //Destroy renderer
    SDL_DestroyRenderer( renderer );
    renderer = NULL;
    //Destroy surface
    SDL_FreeSurface( surface );
    renderer = NULL;
    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    //Quit SDL subsystems
    SDL_Quit();
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{   /*
        此為上網抓的程式碼
        BMP分為多種格式8BIT 26BIT 32BIT，其中這個作業是26BIT，在format->BytesPerPixel中為3(位元組)
        意外的是在網路上找到的程式碼都無法正常定位array的位移，也許因為在26BIT中不需要alpha的緣故

        在這個解法中，可以發現有檢測ENDIAN的程式碼，似乎在不同的系統上，其變化要自己處理。
        其他的不用原因是因為剛好對齊位元組，如同Assembly中的PTR一樣，會按照格式輸出！！！特別！！！

        pixels為pixel array的起始位址
        BytesPerPixel為每pixel佔的位元組數量
        pitch??  評估是整個像素點會佔的數量
    */
    int bpp = surface->format->BytesPerPixel;
     /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;
    switch (bpp)
    {
        case 1:
            return *p;
            break;
        case 2:
            return *(Uint16 *)p;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
            break;
        case 4:
            return *(Uint32 *)p;
            break;
        default:
            return 0; /* shouldn't happen, but avoids warnings */
    }
}

int main( int argc, char* args[] )
{
    int inputDistance;
    int inputNoise;
    printf( "Distance:>" );
    scanf( "%d", &inputDistance );

    inputDistance *= 100000;

    printf( "Noise:>" );
    scanf( "%d", &inputNoise );

    srand( time( NULL ) );
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {   //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            //Event handler
            SDL_Event e;

            int blackNodeCount = 0;

            int mainNodeArrayIndex = 0;

            for( int i = 0; i<surface->h; i++ )
            {
                for( int j = 0; j<surface->w; j++ )
                {
                    SDL_Color rgb;
                    Uint32 data = getpixel(surface, j, i);
                    SDL_GetRGB(data, surface->format, &rgb.r, &rgb.g, &rgb.b);

                    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
                    SDL_RenderDrawPoint( renderer, j, i );

                    if( rgb.r <= 100 )
                    {
                        blackNodeCount++;
                    }
                }
            }

            printf( "%d\n", blackNodeCount );

            pixelNode_Type mainNodeArray = malloc( sizeof( pixelNode_Size ) * blackNodeCount );
            memset( mainNodeArray, 0, sizeof( pixelNode_Size ) * blackNodeCount );
            /*           all unLink node is link to 0        !!!!!!!!!!!!!!!!!!!!!!!!!!                  */
            for( int i = 0; i<surface->h; i++ )
            {
                for( int j = 0; j<surface->w; j++ )
                {
                    SDL_Color rgb;
                    Uint32 data = getpixel(surface, j, i);
                    SDL_GetRGB(data, surface->format, &rgb.r, &rgb.g, &rgb.b);

                    if( rgb.r <= 100 )
                    {
                        mainNodeArray[ mainNodeArrayIndex ].x = j;
                        mainNodeArray[ mainNodeArrayIndex ].y = i;
                        mainNodeArrayIndex++;
                    }
                }
            }
            SDL_RenderPresent(renderer);

            relatedArray_Type relatedNodeArray = malloc( sizeof( relatedArray_Size ) * blackNodeCount );

            pixelNode_Type mainRoot = malloc( sizeof( pixelNode_Size ) );
            memset( mainRoot, 0, sizeof( pixelNode_Size ) );
            // start algo
            for( int i = 0; i < blackNodeCount; i++ )
            {
                unsigned int relatedNodeArrayIndex = 0;

                pixelNode_Type targetNode = &mainNodeArray[ i ];
                // calculate distance and count
                for( int j = 0; j < blackNodeCount; j++ )
                {
                    //reduce
                    //reduce Y
                    if( targetNode->y - mainNodeArray[ j ].y < 0 )
                    {   //in the down space
                        if( targetNode->y + inputDistance < mainNodeArray[j].y )
                        {
                        	break;
                        }
                    }else
                    {   //top space
                        int targetY = targetNode->y - inputDistance;
                        for( ; mainNodeArray[ j ].y < targetY ; j++ )
                        {
                            if( j + 1 == blackNodeCount )
                            {
                                break;
                            }
                        }
                    }
                    //reduce X
                    if( targetNode->x - mainNodeArray[ j ].x < 0 )
                    {   //right
                        int targetX = targetNode->x + inputDistance;
                        for( ; mainNodeArray[ j ].x > targetX; j++ )
                        {
                            if( j + 1 == blackNodeCount )
                            {
                                break;
                            }
                        }
                    }
                    if( mainNodeArray[ j ].x - targetNode->x < 0 )
                    {   //left
                        int targetX = targetNode->x - inputDistance;
                        for( ; mainNodeArray[ j ].x < targetX; j++ )
                        {
                            if( j + 1 == blackNodeCount )
                            {
                                break;
                            }
                        }
                    }

                    if( distanceProcess( targetNode, &mainNodeArray[ j ] ) * 100000 <= inputDistance )
                    {   //found relatedNode
                        relatedNodeArray[ relatedNodeArrayIndex ].gotoNode = &mainNodeArray[ j ];
                        relatedNodeArrayIndex++;
                    }

                }
                //noise
                if( relatedNodeArrayIndex < inputNoise )
                {
                    continue;
                }
                //get all relatedNode's Root
                for( int j = 0; j < relatedNodeArrayIndex; j++ )
                {
                    pixelNode_Type targetRelatedNode = relatedNodeArray[ j ].gotoNode;
                    for( ; targetRelatedNode->head != 0; targetRelatedNode = targetRelatedNode->head );
                    relatedNodeArray[ j ].gotoNode = targetRelatedNode;
                }


                unsigned int *randomBooked = malloc( sizeof( int ) * 0x1000000 );


                int printR = rand() % 256;
                int printG = rand() % 256;
                int printB = rand() % 256;

                SDL_SetRenderDrawColor( renderer, printR, printG, printB, 255 );

                for( int j = 0 ; j < relatedNodeArrayIndex; j++ )
                {
                    if( relatedNodeArray[ j ].gotoNode->dead == 1 )
                    { printf("Wrong\n");
                        continue;
                    }



                    pixelNode_Type debugPRINT = relatedNodeArray[ j ].gotoNode;
                    for( ; debugPRINT != 0; debugPRINT = debugPRINT->next )
                    {
                        SDL_RenderDrawPoint( renderer, debugPRINT->x, debugPRINT->y );
                    }
                     //Handle events on queue
                    SDL_PollEvent( &e );
                    SDL_RenderPresent(renderer);
                }
                free( randomBooked );







                //get group address if exist & disable exist group
                pixelNode_Type alreadyHeadNode = 0;
                for( int j = 0; j < relatedNodeArrayIndex; j++ )
                {
                    pixelNode_Type targetComparsion = relatedNodeArray[ j ].gotoNode;
                    for( int k = j + 1; k < relatedNodeArrayIndex; k++ )
                    {
                        if( targetComparsion == 0 )
                        {
                            break;
                        }

                        if( targetComparsion == relatedNodeArray[ k ].gotoNode  )
                        {
                            relatedNodeArray[ k ].gotoNode = 0;
                        }

                        if( alreadyHeadNode == 0 )
                        {
                            if( targetComparsion->dead == 0 )
                            {
                                if( targetComparsion->group == 1 )
                                {
                                    alreadyHeadNode = targetComparsion;
                                }
                            }else
                            {
                                printf("found Linkeed ERROR!!\n" ); system("pause");
                            }
                        }
                    }
                }
                //insert relatedNode to mainRoot
                if( alreadyHeadNode == 0 )
                {
                    if( mainRoot->nextPixelGroup == 0 )
                    {
                        mainRoot->nextPixelGroup = relatedNodeArray[ 0 ].gotoNode;
                        mainRoot->nextPixelGroupRear = relatedNodeArray[ 0 ].gotoNode;
                    }else
                    {
                        mainRoot->nextPixelGroupRear->nextPixelGroup = relatedNodeArray[ 0 ].gotoNode;
                        mainRoot->nextPixelGroupRear = relatedNodeArray[ 0 ].gotoNode;

                        if( relatedNodeArray[ 0 ].gotoNode->rear != 0 )
                        {
                            printf("found Linked ERROR 1 !!\n"); system("pause");
                        }
                    }

                    mainRoot->nextPixelGroupRear->rear = relatedNodeArray[ 0 ].gotoNode;

                    relatedNodeArray[ 0 ].gotoNode->group = 1;

                    pixelNode_Type insertPixelHandle = mainRoot->nextPixelGroupRear;
                    for( int j = 1; j < relatedNodeArrayIndex; j++ )
                    {
                        if( relatedNodeArray[ j ].gotoNode != 0 )
                        {
                            insertPixelHandle->next = relatedNodeArray[ j ].gotoNode;
                            insertPixelHandle = insertPixelHandle->next;

                            mainRoot->nextPixelGroupRear->rear = relatedNodeArray[ j ].gotoNode;

                            relatedNodeArray[ j ].gotoNode->head = mainRoot->nextPixelGroupRear;
                            relatedNodeArray[ j ].gotoNode->group = 1;
                            relatedNodeArray[ j ].gotoNode->dead = 1;
                        }
                    }
                }else
                {
                    pixelNode_Type insertPixelHandle = alreadyHeadNode;
                    for( int j = 0; j < relatedNodeArrayIndex; j++ )
                    {
                        if( relatedNodeArray[ j ].gotoNode != 0 )
                        {
                            if( relatedNodeArray[ j ].gotoNode != alreadyHeadNode )
                            {
                                if( insertPixelHandle->rear == 0 )
                                {
                                    insertPixelHandle->next = relatedNodeArray[ j ].gotoNode;

                                    if( insertPixelHandle->next->rear == 0 )
                                    {
                                        insertPixelHandle = insertPixelHandle->next;
                                    }else
                                    {
                                        insertPixelHandle = insertPixelHandle->next->rear;
                                    }
                                }else
                                {
                                    insertPixelHandle->rear->next = relatedNodeArray[ j ].gotoNode;

                                    if( insertPixelHandle->rear->next->rear == 0 )
                                    {
                                        insertPixelHandle = insertPixelHandle->rear->next;
                                    }else
                                    {
                                        insertPixelHandle = insertPixelHandle->rear->next->rear;
                                    }
                                }


                                if( relatedNodeArray[ j ].gotoNode->rear == 0 )
                                {
                                    alreadyHeadNode->rear = relatedNodeArray[ j ].gotoNode;
                                }else
                                {
                                    alreadyHeadNode->rear = relatedNodeArray[ j ].gotoNode->rear;
                                }

                                relatedNodeArray[ j ].gotoNode->head = alreadyHeadNode;
                                relatedNodeArray[ j ].gotoNode->group = 1;
                                relatedNodeArray[ j ].gotoNode->dead = 1;
                            }
                        }
                    }
                }
            }

            //output
            unsigned int *randomBooked = malloc( sizeof( int ) * 0x1000000 );
            memset( randomBooked, 0, sizeof( int ) * 0x1000000 );

            if( randomBooked == 0 )
            {
                printf("ERROR\n");system("pause");
            }


            pixelNode_Type printHandle = mainRoot->nextPixelGroup;
            for( ; printHandle != 0; printHandle = printHandle->nextPixelGroup )
            {
                if( printHandle->dead == 1 )
                {
                    continue;
                }

                int printR = rand() % 256;
                int printG = rand() % 256;
                int printB = rand() % 256;
                for(;;)
                {

                    int randomAns = ( printR << 16 ) | ( printG << 8 ) | ( printB );
        			if( randomBooked[ randomAns ] == 0 )
                    {
                        randomBooked[ randomAns ] = 1;
                        break;
                    }
                }

                SDL_SetRenderDrawColor( renderer, printR, printG, printB, 255 );
                pixelNode_Type printInsideHandle = printHandle;
                for( ; printInsideHandle != 0; printInsideHandle = printInsideHandle->next )
                {
                    SDL_RenderDrawPoint( renderer, printInsideHandle->x, printInsideHandle->y );
                }

                //Handle events on queue
                SDL_PollEvent( &e );

                SDL_RenderPresent( renderer );
            }

            //printf( "print complete." );

            /* save to BMP */
            SDL_Surface *testHandle = SDL_GetWindowSurface( gWindow );
            SDL_LockSurface( testHandle );

            printf( "\n%d\n", inputDistance );

            char outputName[ 30 ] = "output_D";
            char outputNameKtemp[ 10 ];
            itoa( inputDistance / 100000, outputNameKtemp, 10 );
            int i = 8;
            for( int j = 0; j < 9; i++, j++ ){
                outputName[ i ] = outputNameKtemp[ j ];

                if( outputNameKtemp[ j ] == '\0' ){
                    break;
                }
            }

            itoa( inputNoise, outputNameKtemp, 10 );
            outputName[ i ] = '_';
            i++;
            outputName[ i ] = 'N';
            i++;

            for( int j = 0 ; j < 9; i++, j++ ){
                outputName[ i ] = outputNameKtemp[ j ];

                if( outputNameKtemp[ j ] == '\0' ){
                    break;
                }
            }

            outputName[ i ] = '.';
            outputName[ i + 1 ] = 'b';
            outputName[ i + 2 ] = 'm';
            outputName[ i + 3 ] = 'p';
            outputName[ i + 4 ] = '\0';

            printf( "%s\n", outputName );

            SDL_SaveBMP( testHandle, outputName );
            SDL_UnlockSurface(  testHandle );
            printf( "Saved." );

            int quit = 0;
            while( !quit )
            {
                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if( e.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                }
            }
           //Update the surface
            SDL_UpdateWindowSurface( gWindow );
        }
    }
    //Free resources and close SDL
    close();
    return 0;
}

