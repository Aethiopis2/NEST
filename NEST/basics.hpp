/**
 * @brief Standard libs and utility functions
 *
 * @author Rediet Worku
 * @date 26th of October 2021, Sunday
 */
#pragma once


//=====================================================================|
#include <string>
#include <vector>
#include <cstdlib>
#include <cctype>
#include <map>



//=====================================================================|
#define iZero(d, s)  memset(d, 0, s)
#define RGBA(r, g, b, a) (((a % 255) << 24) | ( (r % 255) << 16) | \
	((g % 255) << 8) | (b % 255))



//=====================================================================|
// short formats for standandard types
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;



//=====================================================================|
#define WINDOW_WIDTH	1024
#define WINDOW_HEIGHT	768
#define WINDOW_X		SDL_WINDOWPOS_CENTERED
#define WINDOW_Y		SDL_WINDOWPOS_CENTERED


// game dimensions (game is drawn in this region in windowed mode)
#define GAME_WIDTH		640
#define GAME_HEIGHT		480