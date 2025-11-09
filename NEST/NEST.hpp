/**
 * @brief A generic NEST engine class that wrapps around SDL functions that power
 *	NEST. In a sense this is the NEST itself.
 *
 * @author Rediet Worku
 * @date 26th of October 2021, Sunday
 */
#pragma once


//=====================================================================|
#include "iv.hpp"




//=====================================================================|
class NEST
{
public:

	NEST();
	~NEST();

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

	// NEST states
	bool isrunning;		// indicates if main loop is running or should exit
	bool isfull_screen;	// fullscreen vs windowed mode
	bool isPaused;		// stops running NEST code when true

	IV iv;				// internal view - snapshot of NES internal dump
	int screen_id = 0;

	// window props
	int x, y, width, height;


	// misc
	std::string error_string;

	// util
	void Handle_Keys(SDL_Event& event);
};