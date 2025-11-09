/**
 * @brief 
 *
 * @author Rediet Worku
 * @date 28th of October 2021, Tuesday
 */
#pragma once


//=====================================================================|
#include "basics.hpp"
#include <SDL.h>
#include <SDL_ttf.h>





//=====================================================================|
/**
 * @brief this baby helps hold texture info so that each texture drawn
 *	has its texture and knows about itself.
 */
struct TextureInfo
{
	SDL_Texture* ptexture = nullptr;

	int w = 0;
	int h = 0;

	TextureInfo() = default;
	TextureInfo(SDL_Texture* ptext, int width, int height)
		: ptexture(ptext), w(width), h(height) { }
};


//=====================================================================|
class TextureManager
{
public:

	// singlton; we don't want making instances of this baby in our
	//	implementations
	static TextureManager* Instance()
	{
		static TextureManager* ptext = nullptr;
		if (!ptext)
			ptext = new TextureManager();

		return ptext;
	} // end Instance

	
	bool Load_Font(const std::string& file_path, const int font_size = 16);
	int Create_Texture(const int width, const int height, SDL_Renderer* prend,
		const int access = SDL_TEXTUREACCESS_STREAMING);
	int Create_Text_Texture(const std::string& text, const SDL_Color color, 
		SDL_Renderer* prend, const int access = SDL_TEXTUREACCESS_TARGET);
	void Create_Mneumonic_Texture(const std::string& mneumonic, const SDL_Color color,
		SDL_Renderer* prend);

	bool Lock(const int id);
	bool Unlock(const int id);
	void Plot_Pixel(const int x, const int y, const u32 c);
	void Draw(const int id, SDL_Renderer* prend, const int x, const int y,
		const int width = -1, const int height = -1);

	void Scroll_Texture_Down(const int x, const int y, const int height,
		const int src_texture_id, SDL_Renderer* prend);
	void Scroll_Texture_Up(const int x, const int y, const int height,
		const int src_texture_id, SDL_Renderer* prend);

	int Get_Next_ID() const;
	int Get_Texture_Width(const int id) const;
	int Get_Texture_Height(const int id) const;
	int Get_Texture_ID(const std::string& name) const;
	TextureInfo Get_Texture_Info(const int id) const;
	TextureInfo Get_Texture_Info_By_Name(const std::string& name) const;

private:

	TextureManager() : pfont(nullptr), pitch(0), buffer(nullptr) {}

	std::vector<TextureInfo> textures;		// holds all sdl textures here
	std::map<std::string, int> mneumonics;	// mneumonics mapper for emulator iv

	TTF_Font* pfont;						// loaded font

	int pitch;		// horizontal strides in pixels (deals with 32-bit color mode only)
	u32* buffer;	// used on locked surfaces, pointer to that surface/texture
};