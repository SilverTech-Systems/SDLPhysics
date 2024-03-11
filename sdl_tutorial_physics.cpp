#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

//Screen dimension constants
enum side_state{NO_COLLISION, SIDE_UP, SIDE_DOWN, SIDE_LEFT, SIDE_RIGHT, OVERLAP, REJECTION};
enum fonts{SMALL, MEDIUM, LARGE};

int opposite_side(int side)
{
    if(side == SIDE_UP)
        return SIDE_DOWN;
    if(side == SIDE_DOWN)
        return SIDE_UP;
    if(side == SIDE_LEFT)
        return SIDE_RIGHT;
    if(side == SIDE_RIGHT)
        return SIDE_LEFT;
}

const int SCREEN_WIDTH = 1366;
const int SCREEN_HEIGHT = 768;

bool KEY_UP = false;
bool KEY_DOWN = false;
bool KEY_LEFT = false;
bool KEY_RIGHT = false;


using namespace std;

//Texture wrapper class
class LTexture
{
	public:
		//Initializes variables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		#ifdef _SDL_TTF_H
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor, int typeFont );
		#endif

		//Creates blank texture
		//bool createBlank( int width, int height, SDL_TextureAccess = SDL_TEXTUREACCESS_STREAMING );

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation
		void setAlpha( Uint8 alpha );

		//Renders texture at given point
		void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );

		//Set self as render target
		void setAsRenderTarget();

		//Gets image dimensions
		int getWidth();
		int getHeight();

		//Pixel manipulators
		bool lockTexture();
		bool unlockTexture();
		void* getPixels();
		void copyPixels( void* pixels );
		int getPitch();
		Uint32 getPixel32( unsigned int x, unsigned int y );

	private:
		//The actual hardware texture
		SDL_Texture* mTexture;
		void* mPixels;
		int mPitch;

		//Image dimensions
		int mWidth;
		int mHeight;
};

class CPlayer
{
public:
    int KEY_UP = SDLK_UP;
    int KEY_DOWN = SDLK_DOWN;
    int KEY_LEFT = SDLK_LEFT;
    int KEY_RIGHT = SDLK_RIGHT;
    bool UP = false;
    bool LEFT = false;
    bool DOWN = false;
    bool RIGHT = false;
    int score_counter=0;
    char score_counter_str[10];
    char playerwon[10];

private:

};

const int MAX_NUMBER_OF_PLAYERS = 4;

CPlayer Players[MAX_NUMBER_OF_PLAYERS];
int number_of_players = 2;

//The application time based timer
class LTimer
{
    public:
		//Initializes variables
		LTimer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		Uint32 getTicks();

		//Checks the status of the timer
		bool isStarted();
		bool isPaused();

    private:
		//The clock time when the timer started
		Uint32 mStartTicks;

		//The ticks stored when the timer was paused
		Uint32 mPausedTicks;

		//The timer status
		bool mPaused;
		bool mStarted;
};


//The SpriteObject that will move around on the screen
class SpriteObject
{
    public:
		//The dimensions of the SpriteObject
		int PlayerNumber=0;
		int SpriteObject_WIDTH = 20;
		int SpriteObject_HEIGHT = 20;
		int mPosX_last;
        int mPosY_last;
        float mVelX_last;
        float mVelY_last;
		int SpriteObject_offsetX = 81;
		int SpriteObject_offsetY = 21;

        bool GROUND_FRICTION = false;
        const float GROUND_FRICTION_CONST = 0.97;
        bool GRAVITY = true;
        bool AIR_FRICTION =true;
        const float AIR_FRICTION_CONST = 0.997;

        const float ACTION_REACTION = 0.7;

        const float GRAVITY_CONST = 9.81;
        const float GRAVITY_ZOOM = 0.5;
		//Maximum axis velocity of the SpriteObject
		const int SpriteObject_VEL = 10;
		float MAX_VEL_X = SpriteObject_VEL * 80;
		float MAX_VEL_Y = SpriteObject_VEL * 80;

		float velX_vol;
		float velY_vol;
		int channel;
		const float VOL_CONST = 0.109;

		int collision_type = OVERLAP;

		//Initializes the variables
		SpriteObject();

		//Takes key presses and adjusts the SpriteObject's velocity


		//Moves the SpriteObject
		void move( float timeStep , bool KEY_UP, bool KEY_DOWN, bool KEY_LEFT, bool KEY_RIGHT, int collision_side, float ImpulseX, float ImpulseY );

		//Shows the SpriteObject on the screen
		void render(LTexture& texture);

		float mPosX, mPosY;
		float mVelX, mVelY;

    private:

		LTimer air_friction_timer;
		LTimer gravity_timer;
		LTimer ground_friction_timer;
		LTimer movement_timer;

};

int CheckCollision(SpriteObject SpriteObject1, SpriteObject SpriteObject2, float &ImpulseX, float &ImpulseY)
{
    static int in_collision = 0;
    static int in_collision_x = 0;
    static int in_collision_y = 0;
    ImpulseX=0;
    ImpulseY=0;



    int top1 = SpriteObject1.mPosY;
    int bottom1 = SpriteObject1.mPosY + SpriteObject1.SpriteObject_HEIGHT  -  SpriteObject1.SpriteObject_offsetY;
    int left1 = SpriteObject1.mPosX;
    int right1 = SpriteObject1.mPosX + SpriteObject1.SpriteObject_WIDTH - SpriteObject1.SpriteObject_offsetX;


    int top2 = SpriteObject2.mPosY ;
    int bottom2 = SpriteObject2.mPosY + SpriteObject2.SpriteObject_HEIGHT -  SpriteObject2.SpriteObject_offsetY;
    int left2 = SpriteObject2.mPosX ;
    int right2 = SpriteObject2.mPosX + SpriteObject2.SpriteObject_WIDTH - SpriteObject2.SpriteObject_offsetX;


    if(top1 > bottom2)
    {
        in_collision = SIDE_UP;
        in_collision_y = SIDE_UP;
        return NO_COLLISION;
    }
    if(top2 > bottom1)
    {
        in_collision = SIDE_DOWN;
        in_collision_y = SIDE_DOWN;
        return NO_COLLISION;
    }
    if(right2 < left1)
    {
        in_collision = SIDE_LEFT;
        in_collision_x = SIDE_LEFT;
        return NO_COLLISION;
    }
    if(right1 < left2)
    {
        in_collision = SIDE_RIGHT;
        in_collision_x = SIDE_RIGHT;
        return NO_COLLISION;
    }

    if (SpriteObject1.collision_type == OVERLAP || SpriteObject2.collision_type == OVERLAP)
    {
        return OVERLAP;
    }

    ImpulseX = SpriteObject2.mVelX_last;
    ImpulseY = SpriteObject2.mVelY_last;
    return in_collision;
}

//Starts up SDL and creates window
bool init();
void handleEvent( SDL_Event& e );
//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

Mix_Music *gBackgroundMusic = NULL;
Mix_Chunk *gHigh = NULL;
Mix_Chunk *gThree = NULL;
Mix_Chunk *gTwo = NULL;
Mix_Chunk *gOne = NULL;

//Scene textures
LTexture gSpriteObjectTexture;
LTexture gSpriteObject2Texture;
LTexture gSpriteObject3Texture;
LTexture gBackgroundTexture;
LTexture gWinnerBackgroundTexture;
//Globally used font
TTF_Font *gFontSmall = NULL;
TTF_Font *gFontMedium = NULL;
TTF_Font *gFontLarge = NULL;


//Rendered texture
LTexture gTextTemp;
LTexture gTextWinnerTexture;
LTexture gPlayerWon;
LTexture gTextTexture1;
LTexture gTextTexture2;
LTexture gTextTexture3;
LTexture gTextTexture4;
LTexture gTextTexture_score_value[4];
LTexture gStartText;



LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
	mPixels = NULL;
	mPitch = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile( std::string path )
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Convert surface to display format
		 SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );
		SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_RGBA8888, NULL );
		if( formattedSurface == NULL )
		{
			printf( "Unable to convert loaded surface to display format! %s\n", SDL_GetError() );
		}
		else
		{
			//Create blank streamable texture
			newTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h );
			if( newTexture == NULL )
			{
				printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
			}
			else
			{
				//Enable blending on texture
				SDL_SetTextureBlendMode( newTexture, SDL_BLENDMODE_BLEND );

				//Lock texture for manipulation
				SDL_LockTexture( newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch );

				//Copy loaded/formatted surface pixels
				memcpy( mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );

				//Get image dimensions
				mWidth = formattedSurface->w;
				mHeight = formattedSurface->h;

				//Get pixel data in editable format
				Uint32* pixels = (Uint32*)mPixels;
				int pixelCount = ( mPitch / 4 ) * mHeight;

				//Map colors
				Uint32 colorKey = SDL_MapRGB( formattedSurface->format, 0, 0xFF, 0xFF );
				Uint32 transparent = SDL_MapRGBA( formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00 );

				//Color key pixels
				for( int i = 0; i < pixelCount; ++i )
				{
					if( pixels[ i ] == colorKey )
					{
						pixels[ i ] = transparent;
					}
				}

				//Unlock texture to update
				SDL_UnlockTexture( newTexture );
				mPixels = NULL;
			}

			//Get rid of old formatted surface
			SDL_FreeSurface( formattedSurface );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}


bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor, int typeFont )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface;
	if(typeFont==SMALL)
    {
        textSurface = TTF_RenderText_Solid( gFontSmall, textureText.c_str(), textColor );
    }
    else if(typeFont== MEDIUM)
    {
        textSurface = TTF_RenderText_Solid( gFontMedium, textureText.c_str(), textColor );
    }
    else if(typeFont==LARGE)
    {
        textSurface = TTF_RenderText_Solid( gFontLarge, textureText.c_str(), textColor );
    }

	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}


	//Return success
	return mTexture != NULL;
}

/*bool LTexture::createBlank( int width, int height, SDL_TextureAccess access )
{
	//Create uninitialized texture
	mTexture = SDL_CreateTexture( gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height );
	if( mTexture == NULL )
	{
		printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
	}
	else
	{
		mWidth = width;
		mHeight = height;
	}

	return mTexture != NULL;
}*/

void LTexture::free()
{
	//Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
		mPixels = NULL;
		mPitch = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	//Modulate texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	//Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha )
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };
	 SDL_RenderCopy( gRenderer, mTexture, NULL, &renderQuad );

	//Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

void LTexture::setAsRenderTarget()
{
	//Make self render target
	SDL_SetRenderTarget( gRenderer, mTexture );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

bool LTexture::lockTexture()
{
	bool success = true;

	//Texture is already locked
	if( mPixels != NULL )
	{
		printf( "Texture is already locked!\n" );
		success = false;
	}
	//Lock texture
	else
	{
		if( SDL_LockTexture( mTexture, NULL, &mPixels, &mPitch ) != 0 )
		{
			printf( "Unable to lock texture! %s\n", SDL_GetError() );
			success = false;
		}
	}

	return success;
}

bool LTexture::unlockTexture()
{
	bool success = true;

	//Texture is not locked
	if( mPixels == NULL )
	{
		printf( "Texture is not locked!\n" );
		success = false;
	}
	//Unlock texture
	else
	{
		SDL_UnlockTexture( mTexture );
		mPixels = NULL;
		mPitch = 0;
	}

	return success;
}

void* LTexture::getPixels()
{
	return mPixels;
}

void LTexture::copyPixels( void* pixels )
{
	//Texture is locked
	if( mPixels != NULL )
	{
		//Copy to locked pixels
		memcpy( mPixels, pixels, mPitch * mHeight );
	}
}

int LTexture::getPitch()
{
	return mPitch;
}

Uint32 LTexture::getPixel32( unsigned int x, unsigned int y )
{
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32*)mPixels;

    //Get the pixel requested
    return pixels[ ( y * ( mPitch / 4 ) ) + x ];
}


LTimer::LTimer()
{
    //Initialize the variables
    mStartTicks = 0;
    mPausedTicks = 0;

    mPaused = false;
    mStarted = false;
}

void LTimer::start()
{
    //Start the timer
    mStarted = true;

    //Unpause the timer
    mPaused = false;

    //Get the current clock time
    mStartTicks = SDL_GetTicks();
	mPausedTicks = 0;
}

void LTimer::stop()
{
    //Stop the timer
    mStarted = false;

    //Unpause the timer
    mPaused = false;

	//Clear tick variables
	mStartTicks = 0;
	mPausedTicks = 0;
}

void LTimer::pause()
{
    //If the timer is running and isn't already paused
    if( mStarted && !mPaused )
    {
        //Pause the timer
        mPaused = true;

        //Calculate the paused ticks
        mPausedTicks = SDL_GetTicks() - mStartTicks;
		mStartTicks = 0;
    }
}

void LTimer::unpause()
{
    //If the timer is running and paused
    if( mStarted && mPaused )
    {
        //Unpause the timer
        mPaused = false;

        //Reset the starting ticks
        mStartTicks = SDL_GetTicks() - mPausedTicks;

        //Reset the paused ticks
        mPausedTicks = 0;
    }
}

Uint32 LTimer::getTicks()
{
	//The actual timer time
	Uint32 time = 0;

    //If the timer is running
    if( mStarted )
    {
        //If the timer is paused
        if( mPaused )
        {
            //Return the number of ticks when the timer was paused
            time = mPausedTicks;
        }
        else
        {
            //Return the current time minus the start time
            time = SDL_GetTicks() - mStartTicks;
        }
    }

    return time;
}

bool LTimer::isStarted()
{
	//Timer is running and paused or unpaused
    return mStarted;
}

bool LTimer::isPaused()
{
	//Timer is running and paused
    return mPaused && mStarted;
}


SpriteObject::SpriteObject()
{
    //Initialize the position
    mPosX = 200;
    mPosY = 0;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
    air_friction_timer.start();
    gravity_timer.start();
    ground_friction_timer.start();
    movement_timer.start();
}



void handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        for(int i = 0; i < number_of_players; i++)
        {

            if( e.key.keysym.sym == Players[i].KEY_UP )
            {
                Players[i].UP = true;
            }
            if( e.key.keysym.sym == Players[i].KEY_DOWN )
            {
                Players[i].DOWN = true;
            }
            if( e.key.keysym.sym == Players[i].KEY_LEFT )
            {
                Players[i].LEFT = true;
            }
            if( e.key.keysym.sym == Players[i].KEY_RIGHT )
            {
                Players[i].RIGHT = true;
            }
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        for(int i = 0; i < number_of_players;i++)
        {
            if( e.key.keysym.sym == Players[i].KEY_UP )
            {
                Players[i].UP = false;
            }
            if( e.key.keysym.sym == Players[i].KEY_DOWN )
            {
                Players[i].DOWN = false;
            }
            if( e.key.keysym.sym == Players[i].KEY_LEFT )
            {
                Players[i].LEFT = false;
            }
            if( e.key.keysym.sym == Players[i].KEY_RIGHT )
            {
                Players[i].RIGHT = false;
            }
        }
    }
}

void SpriteObject::move( float timeStep, bool KEY_UP, bool KEY_DOWN, bool KEY_LEFT, bool KEY_RIGHT, int collision_side, float ImpulseX, float ImpulseY )
{
    int velocity = SpriteObject_VEL;
    mVelX += ImpulseX * ACTION_REACTION;
    mVelY += ImpulseY * ACTION_REACTION;
    if( KEY_UP == true && KEY_RIGHT == true )
    {
        velocity /= sqrt(2);
    }
    if( KEY_UP == true && KEY_LEFT == true )
    {
        velocity /= sqrt(2);
    }
    if( KEY_DOWN == true && KEY_RIGHT == true )
    {
        velocity /= sqrt(2);
    }
    if( KEY_DOWN == true && KEY_LEFT == true )
    {
        velocity /= sqrt(2);
    }
    if( KEY_UP == true )
    {
        mVelY -= velocity;
    }
    if( KEY_DOWN == true )
    {
        mVelY += velocity;
    }
    if( KEY_LEFT == true )
    {
        mVelX -= velocity;
    }
    if( KEY_RIGHT == true )
    {
        mVelX += velocity;
    }
    //If the SpriteObject went too far to the left or right
	if( mPosX < 0 )
	{
	    mPosX = 0;
		mVelX = -mVelX * ACTION_REACTION;
		velX_vol = mVelX * VOL_CONST;
		Mix_Volume(channel, velX_vol);
		Mix_PlayChannel( channel, gHigh, 0 );
	}
	else if( mPosX > SCREEN_WIDTH - SpriteObject_WIDTH+SpriteObject_offsetX )
	{
	    mPosX = SCREEN_WIDTH - SpriteObject_WIDTH+SpriteObject_offsetX;
		mVelX = -mVelX * ACTION_REACTION;
		velX_vol = mVelX * VOL_CONST;
		Mix_Volume(channel, velX_vol);
		Mix_PlayChannel( channel, gHigh, 0 );
	}


    //If the SpriteObject went too far up or down
	if( mPosY < 0 )
	{
	    mPosY = 0;
		mVelY = -mVelY * ACTION_REACTION;
        velY_vol = mVelY * VOL_CONST;
		Mix_Volume(channel, velY_vol);
		Mix_PlayChannel( channel, gHigh, 0 );
	}
	else if( mPosY > SCREEN_HEIGHT - SpriteObject_HEIGHT+SpriteObject_offsetY )
	{
        mPosY = SCREEN_HEIGHT - SpriteObject_HEIGHT+SpriteObject_offsetY;
		mVelY = -mVelY * ACTION_REACTION;
        velY_vol = mVelY * VOL_CONST;
        Mix_Volume(channel, velY_vol);
		Mix_PlayChannel( channel, gHigh, 0 );
	}


	if (AIR_FRICTION == true)
    {
        if (air_friction_timer.getTicks() >= 1)
        {
            mVelX *= AIR_FRICTION_CONST;
            mVelY *= AIR_FRICTION_CONST;
            air_friction_timer.start();
        }
    }
    if (GRAVITY == true)
    {
        if (gravity_timer.getTicks() >= 1)
        {
            mVelY += GRAVITY_CONST * GRAVITY_ZOOM;
            gravity_timer.start();
        }
    }
    if ( mPosY == SCREEN_HEIGHT - SpriteObject_HEIGHT && GROUND_FRICTION == true )
    {
        if ( ground_friction_timer.getTicks() >= 1 )
        {
            mVelX *= GROUND_FRICTION_CONST;
            ground_friction_timer.start();
        }
    }

    if( collision_side == SIDE_LEFT )
	{
	    mPosX=mPosX_last;
		mVelX = -mVelX * ACTION_REACTION;
		velX_vol = mVelX * VOL_CONST;
		Mix_Volume(channel, velX_vol);
		Mix_PlayChannel( channel, gHigh, 0 );
	}
	if( collision_side == SIDE_RIGHT )
	{
	    mPosX=mPosX_last;
		mVelX = -mVelX * ACTION_REACTION;
		velX_vol = mVelX * VOL_CONST;
		Mix_Volume(channel, velX_vol);
		Mix_PlayChannel( channel, gHigh, 0 );
	}
	if( collision_side == SIDE_UP )
	{
	    mPosY=mPosY_last;
		mVelY = -mVelY * ACTION_REACTION;
        velY_vol = mVelY * VOL_CONST;
		Mix_Volume(channel, velY_vol);
		Mix_PlayChannel( channel, gHigh, 0 );
	}
	if( collision_side == SIDE_DOWN )
	{
	    mPosY=mPosY_last;
		mVelY = -mVelY * ACTION_REACTION;
        velY_vol = mVelY * VOL_CONST;
        Mix_Volume(channel, velY_vol);
		Mix_PlayChannel( channel, gHigh, 0 );
	}

        if(mVelX>MAX_VEL_X)
            mVelX=MAX_VEL_X;
        if(mVelY>MAX_VEL_Y)
            mVelY=MAX_VEL_Y;
        if(mVelX<-MAX_VEL_X)
            mVelX=-MAX_VEL_X;
        if(mVelY<-MAX_VEL_Y)
            mVelY=-MAX_VEL_Y;


        if( collision_side == NO_COLLISION)
        {
                mPosX_last=mPosX;
                mPosY_last=mPosY;
                mVelX_last=mVelX;
                mVelY_last=mVelY;
        }

         //Move the SpriteObject left or right
        mPosX += mVelX * timeStep;
        //Move the SpriteObject up or down
        mPosY += mVelY * timeStep;

}

void SpriteObject::render(LTexture& texture)
{
    //Show the SpriteObject
	texture.render( (int)mPosX, (int)mPosY );
}
bool init()
{

	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}
				 //Initialize SDL_ttf
				if( TTF_Init() == -1 )
				{
					printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
			}
		}
	}
	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load SpriteObject texture
	if( !gSpriteObjectTexture.loadFromFile( "final2ship.png" ) )
	{
		printf( "Failed to load SpriteObject texture!\n" );
		success = false;
	}
	gHigh = Mix_LoadWAV( "high.wav" );
	gBackgroundMusic = Mix_LoadMUS("Tremor.wav");
	if( !gSpriteObject2Texture.loadFromFile( "final2ship.png" ) )
	{
		printf( "Failed to load SpriteObject texture!\n" );
		success = false;
	}
	if( !gSpriteObject3Texture.loadFromFile( "final2ship.png" ) )
	{
		printf( "Failed to load SpriteObject texture!\n" );
		success = false;
	}
	if( gHigh == NULL )
	{
		printf( "Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	gThree = Mix_LoadWAV( "Drei.wav" );
	if( gThree == NULL )
	{
		printf( "Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	gTwo = Mix_LoadWAV( "Zwei.wav" );
	if( gTwo == NULL )
	{
		printf( "Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	gOne = Mix_LoadWAV( "Eins.wav" );
	if( gOne == NULL )
	{
		printf( "Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	if( gBackgroundMusic == NULL )
	{
		printf( "Failed to load high sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	if( !gBackgroundTexture.loadFromFile( "space.jpg" ) )
    {
        printf( "Failed to load background texture image!\n" );
        success = false;
    }
    if( !gWinnerBackgroundTexture.loadFromFile( "winner.jpg" ) )
    {
        printf( "Failed to load background texture image!\n" );
        success = false;
    }
    //Open the font
	gFontSmall = TTF_OpenFont( "UQ_0.ttf", 30);
	gFontMedium = TTF_OpenFont( "UQ_0.ttf", 90);
	gFontLarge = TTF_OpenFont( "UQ_0.ttf", 150);
	if( gFontSmall == NULL || gFontMedium == NULL || gFontLarge == NULL )
	{
		printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
		success = false;
	}
	else
	{
		//Render text
		SDL_Color textColor = { 0, 0, 120 };

	}

	return success;
}

void close()
{
	//Free loaded images
	gSpriteObjectTexture.free();
	gSpriteObject2Texture.free();
	gBackgroundTexture.free();
	gWinnerBackgroundTexture.free();

    Mix_FreeChunk( gHigh );
    Mix_FreeChunk( gThree );
    Mix_FreeChunk( gTwo );
    Mix_FreeChunk( gOne );
    Mix_FreeMusic( gBackgroundMusic );
    gHigh = NULL;
    gThree = NULL;
    gTwo = NULL;
    gOne = NULL;
    gBackgroundMusic = NULL;
	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;
	//Free loaded images
	gPlayerWon.free();
	gTextWinnerTexture.free();
	gTextTexture1.free();
	gTextTexture2.free();
	gTextTexture3.free();
	gTextTexture4.free();


	gTextTexture_score_value[0].free();
	gTextTexture_score_value[1].free();
	gTextTexture_score_value[2].free();
	gTextTexture_score_value[3].free();

	//Free global font
	TTF_CloseFont( gFontSmall );
	TTF_CloseFont( gFontMedium );
	TTF_CloseFont( gFontLarge );
	gFontSmall = NULL;
	gFontMedium = NULL;
	gFontLarge = NULL;

	//Quit SDL subsystems
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

}

int main( int argc, char* args[] )
{

    int j=0;
    int i=0;
    bool flagmove=false;
    bool flagstartgame=false;
    bool flagendgame=false;
    bool flag_soundone=true;
    bool flag_soundtwo=true;
    bool flag_soundthree=true;

	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{

			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;
			SDL_Event e_keydown;

			//Setting Players Data

            Players[0].KEY_UP = SDLK_UP;
            Players[0].KEY_DOWN = SDLK_DOWN;
            Players[0].KEY_RIGHT = SDLK_RIGHT;
            Players[0].KEY_LEFT = SDLK_LEFT;
            Players[1].KEY_UP = SDLK_w;
            Players[1].KEY_DOWN = SDLK_s;
            Players[1].KEY_RIGHT = SDLK_d;
            Players[1].KEY_LEFT = SDLK_a;
            Players[2].KEY_UP = SDLK_i;
            Players[2].KEY_DOWN = SDLK_k;
            Players[2].KEY_RIGHT = SDLK_l;
            Players[2].KEY_LEFT = SDLK_j;
            Players[3].KEY_UP = SDLK_KP_8;
            Players[3].KEY_DOWN = SDLK_KP_5;
            Players[3].KEY_RIGHT = SDLK_KP_6;
            Players[3].KEY_LEFT = SDLK_KP_4;

            //Setting Collision Data
            int collision = 0;
            float impulseX = 0;
            float impulseY = 0;


			//The SpriteObject that will be moving around on the screen

			SpriteObject ship[MAX_NUMBER_OF_PLAYERS];

            ship[0].mPosX = 0;
			ship[0].mPosY = 0;
			ship[1].mPosX = SCREEN_WIDTH -81;
			ship[1].mPosY = 0;
			ship[2].mPosX=0;
            ship[2].mPosY=SCREEN_HEIGHT-21;
			ship[3].mPosX = SCREEN_WIDTH-81;
            ship[3].mPosY = SCREEN_HEIGHT-21;

            for(int i=0;i<number_of_players;i++)
            {
                ship[i].SpriteObject_offsetX=81;
                ship[i].SpriteObject_offsetY=21;
            }

            for(int i=0;i<number_of_players;i++)
            {
                ship[i].SpriteObject_HEIGHT = 75;
                ship[i].SpriteObject_WIDTH = 150;
            }

			ship[0].channel = 0;
			ship[1].channel = 1;
            ship[2].channel = 2;
            ship[3].channel = 3;

            ship[0].collision_type = REJECTION;
            ship[1].collision_type = REJECTION;
            ship[2].collision_type = REJECTION;
            ship[3].collision_type = REJECTION;

			//Keeps track of time between steps
			LTimer stepTimer;
			LTimer start_timer1;
			LTimer start_timer2;
			LTimer start_timer3;
			LTimer start_timer4;


            SDL_Color textColor = {255, 255, 255};

            //gTextTexture.loadFromRenderedText( score, textColor );

            gTextTexture1.loadFromRenderedText( "Score 1:", textColor, SMALL );
            gTextTexture2.loadFromRenderedText( "Score 2:", textColor, SMALL );
            gTextTexture3.loadFromRenderedText( "Score 3:", textColor, SMALL );
            gTextTexture4.loadFromRenderedText( "Score 4:", textColor, SMALL );



			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
				    e_keydown=e;
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
                    handleEvent( e );
				}

				//Calculate time step
				float timeStep;// = stepTimer.getTicks() / 1000.f/ number_of_players;
                float time_const = 1/1000.f * (1/float(number_of_players));
                float start_time;



                for(i=0;i<number_of_players;i++)
                {
                    for(j=0;j<number_of_players;j++)
                    {
                        collision = NO_COLLISION;
                        impulseX = 0;
                        impulseY = 0;
                        timeStep = stepTimer.getTicks() * time_const;
                        if(i!=j)
                        {
                                collision = CheckCollision( ship[i], ship[j], impulseX, impulseY );
                                if(collision != NO_COLLISION && collision != OVERLAP)
                                {
                                    if(flagmove==true)
                                    {
                                        ship[i].move( timeStep, false, false, false, false, collision, -impulseX, -impulseY);
                                    }
                                    Players[i].score_counter+=abs(ship[i].mVelX_last/2) + abs(ship[i].mVelY_last/2);
                                    sprintf(Players[i].score_counter_str, "%d", Players[i].score_counter);
                                    gTextTexture_score_value[i].loadFromRenderedText(Players[i].score_counter_str, textColor, SMALL);
                                    timeStep = stepTimer.getTicks() * time_const;
                                    if(flagmove==true)
                                    {
                                        ship[j].move( timeStep, false, false, false, false, opposite_side(collision), -ship[i].mVelX_last, -ship[i].mVelY_last);
                                    }
                                    Players[j].score_counter+=abs(ship[j].mVelX_last/2)+ abs(ship[j].mVelY_last/2);
                                    sprintf(Players[j].score_counter_str, "%d", Players[j].score_counter);
                                    gTextTexture_score_value[j].loadFromRenderedText(Players[j].score_counter_str, textColor, SMALL);
                                    if(Players[i].score_counter>=1000 || Players[j].score_counter>=1000)
                                    {
                                        if(Players[i].score_counter>Players[j].score_counter)
                                        {
                                            sprintf(Players[i].playerwon, "%d", i+1);
                                            gPlayerWon.loadFromRenderedText(Players[i].playerwon, textColor, LARGE);
                                        }
                                        else
                                        {
                                            sprintf(Players[j].playerwon, "%d", j+1);
                                            gPlayerWon.loadFromRenderedText(Players[j].playerwon, textColor, LARGE);
                                        }
                                        flagmove=false;
                                        flagendgame=true;
                                    }

                                }
                                else
                                {
                                    if(flagmove==true)
                                    {
                                         ship[i].move( timeStep, Players[i].UP, Players[i].DOWN, Players[i].LEFT, Players[i].RIGHT, NO_COLLISION, 0, 0);
                                    }
                                }
                        }
                        if(number_of_players == 1)
                        {
                            if(flagmove==true)
                            {
                                ship[i].move( timeStep, Players[0].UP, Players[0].DOWN, Players[0].LEFT, Players[0].RIGHT, NO_COLLISION, 0, 0);
                            }

                        }
                    }
                }
                stepTimer.start();
				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );
                gBackgroundTexture.render( 0, 0 );
				//Render SpriteObject
				for(int i=0;i<number_of_players;i++)
                {
                    ship[i].render(gSpriteObjectTexture);
                }


                if(flagstartgame==false)
                {
                    gStartText.loadFromRenderedText("Press any key to start", textColor, MEDIUM);

                    gStartText.render(SCREEN_WIDTH-1050, 300);
                    if(e_keydown.type == SDL_KEYDOWN)
                    {

                        flagstartgame=true;
                    }
                }
                if(flagstartgame==true)
                {
                        if (start_timer1.isStarted() == false)
                        {
                             start_timer1.start();
                        }
                        if(start_timer1.getTicks()>=2000 && start_timer1.getTicks() < 4000)
                        {
                            gStartText.loadFromRenderedText("3", textColor, LARGE);
                            gStartText.render(SCREEN_WIDTH-700, 300);
                            if (flag_soundthree == true)
                            {
                                Mix_Volume(4, 120);
                                Mix_PlayChannel( 4, gThree, 0 );
                                flag_soundthree = false;
                            }
                        }
                        if(start_timer1.getTicks()>=4000 && start_timer1.getTicks() < 7000)
                        {
                            gStartText.loadFromRenderedText("2", textColor, LARGE);
                            gStartText.render(SCREEN_WIDTH-700, 300);
                            if (flag_soundtwo == true)
                            {
                                Mix_Volume(4, 120);
                                Mix_PlayChannel( 4, gTwo, 0 );
                                flag_soundtwo = false;
                            }
                        }
                        if(start_timer1.getTicks()>=7000 && start_timer1.getTicks() < 8000)
                        {
                            gStartText.loadFromRenderedText("1", textColor, LARGE);
                            gStartText.render(SCREEN_WIDTH-700, 300);
                            if (flag_soundone == true)
                            {
                                Mix_Volume(4, 120);
                                Mix_PlayChannel( 4, gOne, 0 );
                                flag_soundone = false;
                            }
                        }
                        if(start_timer1.getTicks()>=8000)
                        {
                            if(Mix_PlayingMusic()==false)
                            {
                                Mix_PlayMusic(gBackgroundMusic,-1);
                            }
                        }

                        if(start_timer1.getTicks()>=8000)
                        {
                            flagmove=true;
                        }
                }

                if(flagendgame==true)
                {
                    flagmove=false;
                    gTextWinnerTexture.loadFromRenderedText("WINNER:", textColor, LARGE);
                    gTextTemp.loadFromRenderedText("Play again: y/ n", textColor, MEDIUM);
                    gWinnerBackgroundTexture.render(0,0);
                    gTextWinnerTexture.render(SCREEN_WIDTH-600, 300);
                    gTextTemp.render(SCREEN_WIDTH-600, 400);
                    gPlayerWon.render(SCREEN_WIDTH-180, 300);
                    if( e.key.keysym.sym == SDLK_y )
                    {
                        flagmove = true;
                        flagendgame = false;
                        Players[0].score_counter=0;
                        Players[1].score_counter=0;
                        Players[2].score_counter=0;
                        Players[3].score_counter=0;
                        Players[0].score_counter_str[0]=0;
                        Players[1].score_counter_str[0]=0;
                        Players[2].score_counter_str[0]=0;
                        Players[3].score_counter_str[0]=0;
                        gTextTexture_score_value[0].loadFromRenderedText(Players[0].score_counter_str, textColor, LARGE);
                        gTextTexture_score_value[1].loadFromRenderedText(Players[1].score_counter_str, textColor, LARGE);
                        gTextTexture_score_value[2].loadFromRenderedText(Players[2].score_counter_str, textColor, LARGE);
                        gTextTexture_score_value[3].loadFromRenderedText(Players[3].score_counter_str, textColor, LARGE);
                    }
                    if(e.key.keysym.sym == SDLK_n)
                    {
                        quit=true;
                    }

                }


                gTextTexture1.render( 5,5 );
                gTextTexture2.render( SCREEN_WIDTH-150 , 5);
                gTextTexture3.render( 5,SCREEN_HEIGHT-30);
                gTextTexture4.render( SCREEN_WIDTH -150 , SCREEN_HEIGHT - 30 );
                gTextTexture_score_value[0].render( 95, 5 );
                gTextTexture_score_value[1].render( SCREEN_WIDTH-55 , 5);
                gTextTexture_score_value[2].render( 95 , SCREEN_HEIGHT - 30 );
                gTextTexture_score_value[3].render( SCREEN_WIDTH -55 , SCREEN_HEIGHT - 30 );



				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}
	//Free resources and close SDL
	close();
	return 0;
}
