/**
 * @brief A generic game engine class that wrapps around SDL functions that power
 *	NEST. In a sense this is the NEST itself.
 *
 * @author Rediet Worku
 * @date 26th of October 2021, Sunday
 */
#pragma once


//=====================================================================|
#include "basics.hpp"
#include <SDL.h>



//=====================================================================|
#define WINDOW_WIDTH	1024
#define WINDOW_HEIGHT	768
#define WINDOW_X		SDL_WINDOWPOS_CENTERED
#define WINDOW_Y		SDL_WINDOWPOS_CENTERED



//=====================================================================|
class Game
{
public:

	Game();
	~Game();

	bool Init(std::string title, const bool full_screen = false,
		const int x = WINDOW_X, const int y = WINDOW_Y,
		const int width = WINDOW_WIDTH, const int height = WINDOW_HEIGHT);
	void Cleanup();

	void Set_IsRunning(const bool v);
	bool Is_Running() const;

	void Render();
	void Update();
	void Handle_Events();

	void Pause();
	bool Is_Paused() const;
	bool Is_Full_Screen() const;
	std::string Get_Error_Message() const;

private:

	SDL_Window* pWnd;
	SDL_Renderer* pRenderer;

	// game states
	bool isrunning;		// indicates if main loop is running or should exit
	bool isfull_screen;	// fullscreen vs windowed mode
	bool isPaused;		// stops running game code when true


	// window props
	int x, y, width, height;


	// misc
	std::string error_string;
};