/**
 * @brief Definition for main NEST engine class or the heart of NEST
 *
 * @author Rediet Worku
 * @date 26th of October 2021, Sunday
 */


//=====================================================================|
#include "NEST.hpp"
#include "nes.hpp"




//=====================================================================|
/**
 * @brief
 */
NEST::NEST()
	: pWnd(nullptr), pRenderer(nullptr), 
	iv(NES::Instance())
{
	x = y = width = height = 0;

	// NEST states
	isrunning = true;
	isfull_screen = false;
	isPaused = false;
	error_string = "";
} // end Constructor


//=====================================================================|
/**
 * @breif
 */
NEST::~NEST()
{
	Cleanup();
} // end Destroyer


//=====================================================================|
/**
 * @breif initalizes the NEST engine with the following parameters
 *
 * @param title a window title name
 * @param x starting x position
 * @param y starting y position
 * @param width the width of the window defaults to 1024
 * @param height the height of window defaults to 728
 * @param full_screen boolean that determines NEST should be fullscreen or not 
 *
 * @return a true for successful initalization a false otherwise with
 *			error_string with details of error 
 */
bool NEST::Init(const std::string title, const bool full_screen, 
	const int x, const int y, const int width, const int height)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		error_string = "SDL_Initalization failed in NEST::Init";
		return false;
	} // end if Init failed

	// now let's make us a window
	if (!(pWnd = SDL_CreateWindow(title.c_str(), x, y, width, height, 
		full_screen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_SHOWN)))
	{
		error_string = "SDL_CreateWindow failed in NEST::Init";
		return false;
	} // end if no window

	// while at it toss in renderer too
	if (!(pRenderer = SDL_CreateRenderer(pWnd, -1, 0)))
	{
		error_string = "SDL_CreateRenderer failed in NEST::Init";
		return false;
	} // end pRender
	
	// load font
	TextureManager::Instance()->Load_Font("C:\\Windows\\Fonts\\Consola.ttf");

	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->isfull_screen = full_screen;

	// create a dummy surface
	iv.Init(pRenderer);
	screen_id = TextureManager::Instance()->Create_Texture(256, 240, pRenderer);
	return true;
} // end Init


//=====================================================================|
/**
 * @brief Cleans the resources and kills the main window
 */
void NEST::Cleanup()
{
	SDL_DestroyRenderer(pRenderer);
	SDL_DestroyWindow(pWnd);
	pWnd = nullptr;
	pRenderer = nullptr;		// double tap

	SDL_Quit();
} // end void


//=====================================================================|
/**
 * @brief alters the is running state of the engine externally 
 *
 * @param v a new boolean value to set
 */
void NEST::Set_IsRunning(const bool v)
{
	isrunning = v;
} // end set_isrunning


//=====================================================================|
/**
 * @brief returns the current state of isrunning state member
 */
bool NEST::Is_Running() const
{
	return isrunning;
} // end Is_Running


//=====================================================================|
/**
 * @brief Renders the texturs on the screen at warap speeds for emulator
 */
void NEST::Render()
{
	SDL_RenderClear(pRenderer);
	
	//
	TextureManager::Instance()->Lock(screen_id);

	for (int i = 0; i < 1000; i++)
	{
		int x = rand() % 256;
		int y = rand() % 240;
		u32 color = RGBA(rand(), rand(), rand(), rand());
		TextureManager::Instance()->Plot_Pixel(x, y, color);
	}

	TextureManager::Instance()->Unlock(screen_id);

	SDL_Rect src, dest;
	src.x = dest.x = 0;
	src.y = dest.y = 0;
	src.w = 256;
	dest.w = 640;
	src.h = 240;
	dest.h = 480;
	SDL_RenderCopyEx(pRenderer, 
		TextureManager::Instance()->Get_Texture_Info(screen_id).ptexture, 
		&src, &dest, 0, 0, SDL_FLIP_NONE);

	iv.Draw_CPU();
	iv.Draw_RAM();
	iv.Draw_Disasm();

	SDL_RenderPresent(pRenderer);
} // Render


//=====================================================================|
/**
 * @brief updates the state of emulator
 */
void NEST::Update()
{
	NES::Instance()->cpu.Clock();
} // end Update


//=====================================================================|
/**
 * @brief handles keyboard entries and processes them
 */
void NEST::Handle_Events()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			isrunning = false;
			break;
		
		case SDL_KEYDOWN:
			Handle_Keys(event);
			break;

		default:
			break;
		} // end switch
	} // end if poll event
} // end Handle_Events


//=====================================================================|
/**
 * @brief toggles pause on/off
 */
void NEST::Pause()
{
	isPaused = !isPaused;
} // end Pause


//=====================================================================|
/**
 * @brief determines if NEST has been paused or not
 */
bool NEST::Is_Paused() const
{
	return isPaused;
} // end isPaused;


//=====================================================================|
/**
 * @brief returns a true if app is full screen
 */
bool NEST::Is_Full_Screen() const
{
	return isfull_screen;
} // end Is_Full_Screen


//=====================================================================|
/**
 * @brief Gets a description of last error occured
 */
std::string NEST::Get_Error_Message() const 
{
	return error_string;
} // end Update


//=====================================================================|
/**
 * @brief handles key events
 */
void NEST::Handle_Keys(SDL_Event& event)
{
	switch (event.key.keysym.sym)
	{
	case SDLK_ESCAPE:
		isrunning = false;
		break;

	case SDLK_PAGEUP: 
		iv.Set_Start_Address(iv.Get_Start_Address() - 256);
		break;

	case SDLK_PAGEDOWN: 
		iv.Set_Start_Address(iv.Get_Start_Address() + 256);
		break;

	case SDLK_UP:
		iv.Set_Start_Address(iv.Get_Start_Address() - 16);
		break;

	case SDLK_DOWN:
		iv.Set_Start_Address(iv.Get_Start_Address() + 16);
		break;
	} // end swtich
} // end Handle_Keys