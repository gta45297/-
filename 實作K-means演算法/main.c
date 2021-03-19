//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define true 1
#define false 0
//Screen dimension constants
const int SCREEN_WIDTH = 333;
const int SCREEN_HEIGHT = 500;

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
    struct pixelCore *nextPixelNode;
    int x;
    int y;
    int r;
    int g;
    int b;
}pixelNode_Size, *pixelNode_Type;

void initPixelNode( pixelNode_Type inputPixelNode )
{
    inputPixelNode->nextPixelNode = NULL;
    inputPixelNode->x = 0;
    inputPixelNode->y = 0;
    inputPixelNode->r = 0;
    inputPixelNode->g = 0;
    inputPixelNode->b = 0;
}

typedef struct BaseQueue
{
    int centerR;
    int centerG;
    int centerB;
    pixelNode_Type next;
    pixelNode_Type rear;
}groupQueue_Size, *groupQueue_Type;

void initGroupQueue( groupQueue_Type inputQueue, int inputLength )
{
    for( int i = 0; i< inputLength; i++ )
    {
        inputQueue[ i ].next = NULL;
        inputQueue[ i ].rear = NULL;
        inputQueue[ i ].centerR = 0;
        inputQueue[ i ].centerG = 0;
        inputQueue[ i ].centerB = 0;
    }
}

double distanceProcess( groupQueue_Type inputMainGroup, int mainGroupIndex, pixelNode_Type inputNode2 )
{
    int powAns = ( inputMainGroup[ mainGroupIndex ].centerR - inputNode2->r ) * ( inputMainGroup[ mainGroupIndex ].centerR - inputNode2->r ) + ( inputMainGroup[ mainGroupIndex ].centerG - inputNode2->g ) * ( inputMainGroup[ mainGroupIndex ].centerG - inputNode2->g ) + ( inputMainGroup[ mainGroupIndex ].centerB - inputNode2->b ) * ( inputMainGroup[ mainGroupIndex ].centerB - inputNode2->b );
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
        gWindow = SDL_CreateWindow( "K-mean", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
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
        �����W���쪺�{���X
        BMP�����h�خ榡8BIT 26BIT 32BIT�A�䤤�o�ӧ@�~�O26BIT�A�bformat->BytesPerPixel����3(�줸��)
        �N�~���O�b�����W��쪺�{���X���L�k���`�w��array���첾�A�]�\�]���b26BIT�����ݭnalpha���t�G

        �b�o�ӸѪk���A�i�H�o�{���˴�ENDIAN���{���X�A���G�b���P���t�ΤW�A���ܤƭn�ۤv�B�z�C
        ��L�����έ�]�O�]����n����줸�աA�p�PAssembly����PTR�@�ˡA�|���Ӯ榡��X�I�I�I�S�O�I�I�I

        pixels��pixel array���_�l��}
        BytesPerPixel���Cpixel�����줸�ռƶq
        pitch??  �����O��ӦV���I�|�����ƶq
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
    int inputK;
    printf( "K:>" );
    scanf( "%d", &inputK );

    groupQueue_Type mainGroup = malloc( sizeof( groupQueue_Size ) * inputK );
    initGroupQueue( mainGroup, inputK );

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

            pixelNode_Type pixelGroup = NULL;
            pixelNode_Type pixelGroupRear = NULL;

            for(int i = 0; i<surface->h; i++ )
            {
                for( int j = 0; j<surface->w; j++ )
                {
                    SDL_Color rgb;
                    Uint32 data = getpixel(surface, j, i);
                    SDL_GetRGB(data, surface->format, &rgb.r, &rgb.g, &rgb.b);

                    pixelNode_Type newPixelNode = malloc( sizeof( pixelNode_Size ) );
                    initPixelNode( newPixelNode );
                    if( pixelGroup == NULL )
                    {
                        pixelGroup = newPixelNode;
                    }else
                    {
                        pixelGroupRear->nextPixelNode = newPixelNode;
                    }
                    pixelGroupRear = newPixelNode;
                    newPixelNode->r = rgb.r;
                    newPixelNode->g = rgb.g;
                    newPixelNode->b = rgb.b;
                    newPixelNode->x = j;
                    newPixelNode->y = i;
                }
            }
            // algo start
            // random boss
            int maxPixelCount = surface->h * surface->w;
            int bookForRandom[ maxPixelCount ] ;
            memset( bookForRandom, 0, sizeof( bookForRandom ) );
            int random[ inputK ];
            for( int i = 0; i<inputK; i++ )
            {
                for(;;)
                {
                    unsigned int newRand = 0;
                    newRand = rand() * rand();

                    int wantPixel = newRand % maxPixelCount;
                    if( bookForRandom[ wantPixel ] == 0 )
                    {
                        bookForRandom[ wantPixel ] = 1;
                        random[ i ] = wantPixel;
                        maxPixelCount--;
                        break;
                    }
                }
            }
            //put choose random to mainGroup
            for( int i = 0; i < inputK; i++ )
            {
                pixelNode_Type pixelHandle = pixelGroup;
                pixelNode_Type nodeForDelete;
                for( int j = 0; j != random[ i ]; j++ )
                {
                    if( j != 0 )
                    {
                        nodeForDelete = pixelHandle;
                        pixelHandle = pixelHandle->nextPixelNode;
                    }
                }
                if( pixelHandle == pixelGroup )
                {
                    mainGroup[ i ].next = pixelGroup;
                    mainGroup[ i ].rear = pixelGroup;
                    pixelGroup = pixelGroup->nextPixelNode;
                    mainGroup[ i ].rear->nextPixelNode = NULL;
                }else
                {
                    nodeForDelete->nextPixelNode = pixelHandle->nextPixelNode;
                    mainGroup[ i ].next = pixelHandle;
                    mainGroup[ i ].rear = pixelHandle;
                    pixelHandle->nextPixelNode = NULL;
                }
                mainGroup[ i ].centerR = mainGroup[ i ].next->r;
                mainGroup[ i ].centerG = mainGroup[ i ].next->g;
                mainGroup[ i ].centerB = mainGroup[ i ].next->b;
            }
            mainGroup[ 0 ].rear->nextPixelNode = pixelGroup;
            mainGroup[ 0 ].rear = pixelGroupRear;

            unsigned loopTimes = 0;
            int continueRun = 1;
            while( continueRun )
            {
                continueRun = 0;

                for( int i = 0; i < inputK; i++ )
                {
                    pixelNode_Type targetGroup = mainGroup[ i ].next;
                    pixelNode_Type previousTarget = targetGroup;
                    int patch = 0;
                    for( ; targetGroup != NULL; )
                    {
                        int chooseK = -1;
                        int currentDistance = (int)( distanceProcess( mainGroup, i, targetGroup ) * 1000000 );

                        for( int j = 0; j < inputK; j++ )
                        {
                            //Handle events on queue
                            while( SDL_PollEvent( &e ) != 0 )
                            {
                                //User requests quit
                                if( e.type == SDL_QUIT )
                                {
                                    goto END;
                                }
                            }
                            //reduce
                            if( i == j )
                            {
                                continue;
                            }
                            int reduceDistance;
                            if( targetGroup->r - mainGroup[ j ].centerR < 0 )
                            {
                                reduceDistance = mainGroup[ j ].centerR - targetGroup->r;
                            }else
                            {
                                reduceDistance = targetGroup->r - mainGroup[ j ].centerR;
                            }
                            if( reduceDistance < currentDistance )
                            {
                                if( targetGroup->g - mainGroup[ j ].centerG < 0 )
                                {
                                    reduceDistance = mainGroup[ j ].centerG - targetGroup->g;
                                }else
                                {
                                    reduceDistance = targetGroup->g - mainGroup[ j ].centerG;
                                }
                                if( reduceDistance < currentDistance )
                                {
                                    if( targetGroup->b - mainGroup[ j ].centerB < 0 )
                                    {
                                        reduceDistance = mainGroup[ j ].centerB - targetGroup->b;
                                    }else
                                    {
                                        reduceDistance = targetGroup->b - mainGroup[ j ].centerB;
                                    }
                                    if( reduceDistance < currentDistance )
                                    {

                                    }else
                                    {printf("C");
                                        continue;
                                    }
                                }else
                                {printf("B");
                                    continue;
                                }
                            }else
                            {
                                continue;
                            }

                            int challengeDistance = (int)( distanceProcess( mainGroup, j, targetGroup ) * 1000000 );
                            if( challengeDistance < currentDistance )
                            {
                                currentDistance = challengeDistance;
                                chooseK = j;
                            }
                        }
                        //start move
                        if( chooseK != -1 )
                        {
                            pixelNode_Type moveTarget = targetGroup;
                            //take
                            if( moveTarget == mainGroup[ i ].next )
                            {   //fisrt pixel
                                if( moveTarget->nextPixelNode == NULL )
                                {
                                    //printf("ERROR, %d group will be empty, it isn't correct work.\n", i ); system("pause");
                                    goto EIGNORE;
                                    mainGroup[ i ].next = NULL;
                                    mainGroup[ i ].rear = NULL;
                                }else
                                {
                                    mainGroup[ i ].next = moveTarget->nextPixelNode;
                                    moveTarget->nextPixelNode = NULL;
                                    targetGroup = mainGroup[ i ].next;
                                    patch = 1;
                                }
                            }else
                            {
                                if( moveTarget->nextPixelNode == NULL )
                                {
                                    mainGroup[ i ].rear = previousTarget;
                                    previousTarget->nextPixelNode = NULL;
                                }else
                                {
                                    previousTarget->nextPixelNode = moveTarget->nextPixelNode;
                                    moveTarget->nextPixelNode = NULL;
                                    targetGroup = previousTarget;
                                }
                            }
                            //insert
                            if( mainGroup[ chooseK ].next == NULL )
                            {
                                printf("ERROR, %d group is empty!!!!!, it isn't correct work.\n", chooseK );system("pause");
                            }else
                            {
                                mainGroup[ chooseK ].rear->nextPixelNode = moveTarget;
                                mainGroup[ chooseK ].rear = moveTarget;
                            }
                        }
                        EIGNORE:
                        previousTarget = targetGroup;

                        if( patch == 0 )
                        {
                            targetGroup = targetGroup->nextPixelNode;
                        }else
                        {
                            patch = 0;
                        }
                    }
                }
                // step 2=> calculate avg
                for( int i = 0; i < inputK; i++ )
                {int y = 0;
                    unsigned int rValue = 0;
                    unsigned int gValue = 0;
                    unsigned int bValue = 0;
                    unsigned int count = 0;
                    pixelNode_Type pixelHandle = mainGroup[ i ].next;
                    for( ; pixelHandle != NULL; pixelHandle = pixelHandle->nextPixelNode )
                    {  y++;
                        rValue += pixelHandle->r;
                        gValue += pixelHandle->g;
                        bValue += pixelHandle->b;
                        count++;
                    }
                    if( mainGroup[ i ].centerR !=  rValue / count )
                    {
                        mainGroup[ i ].centerR = rValue / count;
                         continueRun = 1;
                    }
                    if( mainGroup[ i ].centerG != gValue / count )
                    {
                        mainGroup[ i ].centerG = gValue / count;
                         continueRun = 1;
                    }
                    if( mainGroup[ i ].centerB != bValue / count )
                    {
                        mainGroup[ i ].centerB = bValue / count;
                         continueRun = 1;
                    }
                }
                //print
                for( int i = 0; i < inputK; i++ )
                {
                    pixelNode_Type pixelHandle = mainGroup[ i ].next;
                    int printR = mainGroup[ i ].centerR;
                    int printG = mainGroup[ i ].centerG;
                    int printB = mainGroup[ i ].centerB;
                    for( ; pixelHandle != NULL; pixelHandle = pixelHandle->nextPixelNode )
                    {
                        SDL_SetRenderDrawColor( renderer, printR, printG, printB, 255 );
                        SDL_RenderDrawPoint( renderer, pixelHandle->x, pixelHandle->y );

                        //Handle events on queue
                        while( SDL_PollEvent( &e ) != 0 )
                        {
                            //User requests quit
                            if( e.type == SDL_QUIT )
                            {
                                goto END;
                            }
                        }
                    }
                    SDL_RenderPresent(renderer);
                }
                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if( e.type == SDL_QUIT )
                    {
                        goto END;
                    }
                }
                loopTimes++;
                printf( "Loop Time = %d\n", loopTimes );
            }
            //printf("done\n");

            /* save to BMP */
            SDL_Surface *testHandle = SDL_GetWindowSurface( gWindow );
            SDL_LockSurface( testHandle );

            char outputName[ 15 ] = "output";
            char outputNameKtemp[ 10 ];
            itoa( inputK, outputNameKtemp, 10 );
            int i = 0;
            for( ; i < 9; i++ ){
                outputName[ i + 6 ] = outputNameKtemp[ i ];

                if( outputNameKtemp[ i ] == '\0' ){
                    break;
                }

            }
            outputName[ i + 6 ] = '.';
            outputName[ i + 7 ] = 'b';
            outputName[ i + 8 ] = 'm';
            outputName[ i + 9 ] = 'p';

            printf( "%s", outputName );

            SDL_SaveBMP( testHandle, outputName );
            SDL_UnlockSurface(  testHandle );
            printf( "\nSaved." );

            int quit = false;

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
            //Wait two seconds
            SDL_Delay( 2000 );
        }
    }
END:
    //Free resources and close SDL
    close();
    return 0;
}
