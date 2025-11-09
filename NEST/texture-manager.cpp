/**
 * @brief implementation for NEST's TextureManager class
 *
 * @author Rediet Worku
 * @date 28th of October 2021, Tuesday
 */
#pragma once


//=====================================================================|
#include "texture-manager.hpp"
#include "NEST.hpp"




//=====================================================================|
/**
 * @brief loads the true type font and itnitalzes the pointer required
 *	by sdl font rendereres. The emulator depends on monospaced fonts
 *	to make its calculations a lot easier.
 *
 * @param file_path the path for font; good idea to make it relative
 * @param font_size the size for the font
 *
 * @return true if successful alas false
 */
bool TextureManager::Load_Font(const std::string& file_path, 
	const int font_size)
{
	if (TTF_Init())
		return false;

	pfont = TTF_OpenFont(file_path.c_str(), font_size);
	if (!pfont)
		return false;

	return true;
} // end Load_Font

//=====================================================================|
/**
 * @brief Create's a texture of a specified dimensions and flag value
 *
 * @param width the length for texture
 * @param hieght of the texture
 * @param prend the renderer
 * @param access flag defining the surface type
 *
 * @return int id of the texture which is an index into its vector
 */
int TextureManager::Create_Texture(const int width, const int height,
	SDL_Renderer* prend, const int access)
{
	int id = textures.size();

	SDL_Texture* ptext = SDL_CreateTexture(prend, 
		SDL_PIXELFORMAT_ABGR8888,
		access, 
		width, height);

	if (!ptext)
		return -1;	// no texture

	textures.push_back({ ptext, width, height });
	return id;
} // end Create_Texture

//=====================================================================|
/**
 * @brief Create's a texture containing a pre-kooked text along side
 *	its dimensions to make life easy
 *
 * @param text the text create a texture for
 * @param color the color of text
 * @param prend the renderer
 * @param access flag defining the surface type
 *
 * @return int id of the texture which is an index into its vector 
 */
int TextureManager::Create_Text_Texture(const std::string& text, 
	const SDL_Color color, SDL_Renderer* prend, const int access)
{
	SDL_Surface* psurf = TTF_RenderText_Blended(pfont, text.c_str(), color);
	if (!psurf)
		return -1;

	// Create a render-target texture
	int id = Create_Texture(psurf->w, psurf->h, prend, access);
	SDL_Texture* ptext = SDL_CreateTextureFromSurface(prend, psurf);
	if (!ptext)
		return -1;

	SDL_SetRenderTarget(prend, textures[id].ptexture);
	SDL_RenderCopy(prend, ptext, nullptr, nullptr);
	SDL_SetRenderTarget(prend, nullptr);

	SDL_FreeSurface(psurf);
	SDL_DestroyTexture(ptext);
	return id;
} // end Create_Text_Texture

//=====================================================================|
/**
 * @brief creates a texture tailored to insturction mneuomincs, so that
 *	iv can have a better time drawing disassembly. 
 *
 * @param mneuomic string to draw as texture
 * @param color of string
 * @param prend renderer pointer
 */
void TextureManager::Create_Mneumonic_Texture(const std::string& mneumonic,
	SDL_Color color, SDL_Renderer* prend)
{
	// create a normal text texture as before
	int id = Create_Text_Texture(mneumonic, color, prend);

	// add texture to map
	mneumonics[mneumonic] = id;
} // end Create_Mneumonic_Texture

//=====================================================================|
/**
 * @brief locks a lockable texture in sdl, i.e. SDL_TEXTUREACCESS_STREAMING
 *	and makes the internal pointer buffer and pitch so that it can be
 *	used to plot pixels or so.
 *	My object model persumes single locable texture per object and 
 *	32-bits per pixel.
 * 
 * @param id the id for lockable texture resource
 *
 * @return true if successful
 */
bool TextureManager::Lock(const int id)
{
	if (id < 0 || id >= textures.size())
		return false;

	SDL_Texture* ptext = textures[id].ptexture;
	if (SDL_LockTexture(ptext, nullptr, (void**)&buffer, &pitch))
	{
		SDL_Log("Failed to lock texture id: %d", id);
		return false;
	} // end if

	// now buffer and pitch are ready for plotting
	pitch >>= 2;
	return true;
} // end Lock

//=====================================================================|
/**
 * @brief unlocks a locked texture and reset's the internal pointer and
 *	pitch to null.
 *
 * @param id for the resource texture
 *
 * @return true on success
 */
bool TextureManager::Unlock(const int id)
{
	if (id < 0 || id >= textures.size())
		return false;

	SDL_UnlockTexture(textures[id].ptexture);

	buffer = nullptr;	// doubletap
	pitch = 0;
	return true;
} // end Unlock

//=====================================================================|
/**
 * @brief plots a single pixel/point at x,y with a given color. The method
 *	persumes the attributes buffer and pitch have all been initalized and
 *	surface is already locked. 
 * 
 * @param x coordinate on texture
 * @param y coordinate on texture
 * @param c the color of pixel/point
 */
void TextureManager::Plot_Pixel(const int x, const int y, const u32 c)
{
	buffer[x + y * pitch] = c;
} // end Plot_Pixel

//=====================================================================|
/**
 * @brief blast's whatever is contained within the source texture to 
 *	the destination, whereever the context is set, given by the params;
 *	thus can be used to scale on the destination surface.
 *
 * @param id the id for the texture resource
 * @param prend pointer to the rendering context
 * @param x coordinate on destination texture
 * @param y coordinate on destination texture
 * @param width length on the destination texture
 * @param height on the destination texture
 */
void TextureManager::Draw(const int id, SDL_Renderer* prend, const int x, 
	const int y, const int width, const int height)
{
	if (id < 0 || id >= textures.size())
		return;

	// discard invalidity
	if ((width < 0 && height >= 0) || (width >= 0 && height < 0))
		return;

	TextureInfo ti = textures[id];
	SDL_Rect dest;
	if (width == -1 && height == -1)
	{
		dest = { x, y, textures[id].w, textures[id].h };
	} // end if no width set
	else
	{
		dest = { x, y, width, height };
	} // end else

	SDL_RenderCopy(prend, ti.ptexture, nullptr, &dest);
} // end Draw

//=====================================================================|
/**
 * @brief Scrolls the texture down by the height specified begining with
 *	the starting points x, y; it basically overdraws itself with overlapping
 *	rectangular regions
 *
 * @param x the horizontal point
 * @param y the vertical point
 * @param height the height of line
 * @param src_texture_id id for the source texture
 * @param pdest pointer to the destination surface
 * @param prend pointer to the renderer
 */
void TextureManager::Scroll_Texture_Down(const int x, const int y,
	const int height, const int src_texture_id, 
	SDL_Renderer* prend)
{
	SDL_Rect src{ x, y, textures[src_texture_id].w, 
		textures[src_texture_id].h - y};
	SDL_Rect dst{ x, height, textures[src_texture_id].w, 
		textures[src_texture_id].h - y };

	SDL_RenderCopy(prend, textures[src_texture_id].ptexture, &src, &dst);
} // end Scroll_Texture_Down

//=====================================================================|
/**
 * @brief Scrolls the texture up, this is essientially opposite of 
 *	Scroll_Texture_Down method
 *
 * @param x the horizontal point
 * @param y the vertical point
 * @param height the height of line
 * @param src_texture_id id for the source texture
 * @param pdest pointer to the destination surface
 * @param prend pointer to the renderer
 */
void TextureManager::Scroll_Texture_Up(const int x, const int y,
	const int height, const int src_texture_id,
	SDL_Renderer* prend)
{
	SDL_Rect src{ x, y, textures[src_texture_id].w,
		textures[src_texture_id].h - y };
	SDL_Rect dst{ x, y << 1, textures[src_texture_id].w,
		textures[src_texture_id].h - y };

	SDL_RenderCopy(prend, textures[src_texture_id].ptexture, &src, &dst);
} // end Scroll_Texture_Up

//=====================================================================|
/**
 * @brief returns the next id for texture, which is really the next
 *	available space
 *
 * @return id
 */
int TextureManager::Get_Next_ID() const
{
	return textures.size();
} // end Get_Next_ID

//=====================================================================|
/**
 * @brief returns the textures width for a particular id
 *
 * @param id for texture to scan
 * 
 * @return texture width or dummy value
 */
int TextureManager::Get_Texture_Width(const int id) const
{
	if (id < 0 || id >= textures.size())
		return -1;

	return textures[id].w;
} // end Get_Texture_Width

//=====================================================================|
/**
 * @brief returns the textures height for a particular id
 *
 * @param id for texture to scan
 * 
 * @return texture height or invalid height value
 */
int TextureManager::Get_Texture_Height(const int id) const
{
	if (id < 0 || id >= textures.size())
		return -1;

	return textures[id].h;
} // end Get_Texture_Width

//=====================================================================|
/**
 * @brief return's the texuture id given its name from the map
 */
int TextureManager::Get_Texture_ID(const std::string& name) const
{
	return mneumonics.at(name);
} // end Get_Texture_ID

//=====================================================================|
/**
 * @brief return's the texture info located at id
 *
 * @param id for the texture resource
 */
TextureInfo TextureManager::Get_Texture_Info(const int id) const
{
	if (id < 0 || id >= textures.size())
		return {};

	return textures[id];
} // end Get_Texture

//=====================================================================|
/**
 * @bief returns the texture info given its name from the map
 *
 * @param name for texture resource
 */
TextureInfo TextureManager::Get_Texture_Info_By_Name(const 
	std::string& name) const
{
	return textures[mneumonics.at(name)];
} // end Get_Texture_Info_By_Name