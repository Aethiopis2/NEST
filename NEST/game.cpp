/**
 * @brief Definition for main game engine class or the heart of NEST
 *
 * @author Rediet Worku
 * @date 26th of October 2021, Sunday
 */


 //=====================================================================|
#include "game.hpp"



//=====================================================================|
/**
 * @brief
 */
Game::Game()
	: pWnd(nullptr), pRenderer(nullptr)
{
	x = y = width = height = 0;

	// game states
	isrunning = true;
	isfull_screen = false;
	isPaused = false;
	error_string = "";
} // end Constructor


//=====================================================================|
/**
 * @breif
 */
Game::~Game()
{
	Cleanup();
} // end Destroyer


//=====================================================================|
/**
 * @breif initalizes the game engine with the following parameters
 *
 * @param title a window title name
 * @param x starting x position
 * @param y starting y position
 * @param width the width of the window defaults to 1024
 * @param height the height of window defaults to 728
 * @param full_screen boolean that determines game should be fullscreen or not 
 *
 * @return a true for successful initalization a false otherwise with
 *			error_string with details of error 
 */
bool Game::Init(const std::string title, const bool full_screen, 
	const int x, const int y, const int width, const int height)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		error_string = "SDL_Initalization failed in Game::Init";
		return false;
	} // end if Init failed

	// now let's make us a window
	if (!(pWnd = SDL_CreateWindow(title.c_str(), x, y, width, height, 
		full_screen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_SHOWN)))
	{
		error_string = "SDL_CreateWindow failed in Game::Init";
		return false;
	} // end if no window

	// while at it toss in renderer too
	if (!(pRenderer = SDL_CreateRenderer(pWnd, -1, 0)))
	{
		error_string = "SDL_CreateRenderer failed in Game::Init";
		return false;
	} // end pRender
	
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->isfull_screen = full_screen;

	return true;
} // end Init


//=====================================================================|
/**
 * @brief Cleans the resources and kills the main window
 */
void Game::Cleanup()
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
void Game::Set_IsRunning(const bool v)
{
	isrunning = v;
} // end set_isrunning


//=====================================================================|
/**
 * @brief returns the current state of isrunning state member
 */
bool Game::Is_Running() const
{
	return isrunning;
} // end Is_Running


//=====================================================================|
/**
 * @brief Renders the texturs on the screen at warap speeds for emulator
 */
void Game::Render()
{
	SDL_RenderClear(pRenderer);
	
	//

	SDL_RenderPresent(pRenderer);
} // Render


//=====================================================================|
/**
 * @brief updates the state of emulator
 */
void Game::Update()
{

} // end Update


//=====================================================================|
/**
 * @brief handles keyboard entries and processes them
 */
void Game::Handle_Events()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			isrunning = false;
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
void Game::Pause()
{
	isPaused = !isPaused;
} // end Pause


//=====================================================================|
/**
 * @brief determines if game has been paused or not
 */
bool Game::Is_Paused() const
{
	return isPaused;
} // end isPaused;


//=====================================================================|
/**
 * @brief returns a true if app is full screen
 */
bool Game::Is_Full_Screen() const
{
	return isfull_screen;
} // end Is_Full_Screen


//=====================================================================|
/**
 * @brief Gets a description of last error occured
 */
std::string Game::Get_Error_Message() const 
{
	return error_string;
} // end Update