/**
 * @brief IV is Internal View or Disassembly view with memory dumps and
 *	CPU states and register values; allows to investigate a real code at
 *	runtime.
 *
 * @author Rediet Worku
 * @date 29th of October 2021, Wednesday
 */
#pragma once


//=====================================================================|
#include "texture-manager.hpp"
#include "nes.hpp"




//=====================================================================|
/**
 * helps with the disassembly view
 */
struct Disasm_Mnemonic
{
	std::string mnemonic;		// the mnemonic surface
};


class IV
{
public:

	IV(NES* pnes);

	void Init(SDL_Renderer* pr);
	void Draw_CPU();
	void Draw_RAM();
	void Draw_Disasm();

	void Set_Start_Address(const u16 addr) { start_addr = addr; }
	u16 Get_Start_Address() const { return start_addr; }

private:

	SDL_Renderer* prend;		// sdl renderer object
	NES* pnes;					// pointer to nes object
	std::map<u16, Disasm_Mnemonic> disasm_addr;		// full disassembly text

	int hex_texture_id;			// id for hex numerics texture
	int status_texture_id;		// id for 6502 status flags texture
	int flags_texture_id;		// id for prekooked symbols texture
	int registers_texture_id;	// starting id for 6503 register textures
	int ram_texture_id;			// ram surface
	int dollar$_texture_id;		// id for $ sign
	int pound_texture_id;		// id for # sign
	int colon_texture_id;		// id for colon
	int comma_texture_id;		// id for comma
	int x_texture_id;			// id for x label
	int y_texture_id;			// id for y label
	int a_texture_id;			// id for a label
	int bracket_texture_id;		// id fro braket texture
	int squareb_texture_id;		// id for square bracket texure
	int disasm_texture_id;		// id for disassembly texture/surface
	TextureInfo glyph_info;		// used to acquire texture glyph dimensions (monospace)

	// controls
	u16 start_addr = 0x0000;	// starting address for ram
	bool first_draw = true;		// indicates if this is a first time drawing


	// utils
	void Create_Numeric_Textures();
	void Create_Cpu_Flags_Texture();
	void Create_Cpu_Registers_Textures();
	void Create_Ram_Texture();
	void Create_Symbol_Textures();
	void Create_Disasm_Texture();
	

	void Draw_CPU_Flags();
	void Draw_Registers();
	void Draw_Disasm_Line(u16 addr, int x, int y);

	inline void Draw_Hex8(const u8 num, int& x, int& y, const u8 color_index = 0);
	inline void Draw_Hex16(const u16 num, int& x, int& y, const u8 color_index = 0);
	void Build_Disasm(const u16 start = 0x0, const u16 end = 0xFFFF);
};