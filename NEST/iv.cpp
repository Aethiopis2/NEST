/**
 * @brief IV implementation of iv class
 *
 * @author Rediet Worku
 * @date 29th of October 2021, Wednesday
 */
#pragma once

//=====================================================================|
#include <iostream>
#include <sstream>
#include <iomanip>
#include "iv.hpp"





//=====================================================================|
// colors
// flag colors
const SDL_Color off_color = { 150, 150, 150, 255 };
const SDL_Color on_color = { 219, 255, 31, 255 };

const SDL_Color label_color = { 0, 160, 160, 255 };
const SDL_Color text_color = { 222, 222, 222, 255 };
const SDL_Color register_color = { 0, 128, 255, 255 };
const SDL_Color mnemonic_color = { 255, 127, 29, 255 };
const SDL_Color addr_mode_color = { 255, 127, 229, 255 };
const SDL_Color highlight_color = { 255,255,0,100 };  // translucent yellow


// these constants help with labels for disassembler view, and 
//	organize our little texture templates
const std::string ADDR_IMP{ "; IMP" };
const std::string ADDR_IMM{ "; IMM" };
const std::string ADDR_ZP0{ "; ZP0" };
const std::string ADDR_ZPX{ "; ZPX" };
const std::string ADDR_ZPY{ "; ZPY" };
const std::string ADDR_IZX{ "; IZX" };
const std::string ADDR_IZY{ "; IZY" };
const std::string ADDR_ABS{ "; ABS" };
const std::string ADDR_ABX{ "; ABX" };
const std::string ADDR_ABY{ "; ABY" };
const std::string ADDR_IND{ "; IND" };
const std::string ADDR_REL{ "; REL" };

using TM = TextureManager;

// helpers
std::string hex8(uint8_t v) 
{
	std::ostringstream ss; 
	ss << std::uppercase << std::hex << std::setw(2) << std::setfill('0') 
		<< (int)v; 
	return ss.str();
}
std::string hex16(uint16_t v) 
{
	std::ostringstream ss; 
	ss << std::uppercase << std::hex << std::setw(4) << std::setfill('0') 
		<< v; 
	return ss.str();
}


//=====================================================================|
/**
 * @brief constructor
 * 
 * @param pc pointer to NES CPU the 6502
 */
IV::IV(NES* nes)
	: prend(nullptr), pnes(nes),
	hex_texture_id(0), status_texture_id(0), flags_texture_id(0),
	registers_texture_id(0),
	ram_texture_id(0), dollar$_texture_id(0), pound_texture_id(0),
	colon_texture_id(0), disasm_texture_id(0), comma_texture_id(0),
	x_texture_id(0), y_texture_id(0), a_texture_id(0), bracket_texture_id(0),
	squareb_texture_id(0)
{ }

//=====================================================================|
/**
 * @brief initalizes IV by first creating the required textures and
 *	drawing for first time.
 *
 * @param pr pointer to SDL Renderer object
 */
void IV::Init(SDL_Renderer* pr)
{
	prend = pr;

	// create the textures and all
	Create_Numeric_Textures();
	Create_Cpu_Flags_Texture();
	Create_Cpu_Registers_Textures();
	Create_Symbol_Textures();
	Build_Disasm();

	// get a glyph info, since monospaced, all texts are
	//	same size, coolio!
	glyph_info = TM::Instance()->Get_Texture_Info(hex_texture_id);

	Create_Ram_Texture();
	Create_Disasm_Texture();

	// draw some static stuff
	
	// draw dollar$ to cpu registers surface, for each A X Y SP PC
	SDL_SetRenderTarget(prend,
		TM::Instance()->Get_Texture_Info(registers_texture_id).ptexture);
	int x = glyph_info.w * 2; 
	int y = 0;

	for (int i = 0; i < 5; i++)
	{
		TM::Instance()->Draw(dollar$_texture_id, prend, x, y);
		if (i < 2)
			x += glyph_info.w * 7;
		else
			x += glyph_info.w * 8;
	} // end for

	SDL_SetRenderTarget(prend, nullptr);
} // end Init

//=====================================================================|
/**
 * @brief draws the current internal state of 6502 CPU
 */
void IV::Draw_CPU()
{
	Draw_CPU_Flags();
	Draw_Registers();
} // end Draw_CPU

//=====================================================================|
/**
 * @brief draw's the content of the ram 256 bytes at once in a a painfuly
 *	slow O(n2) means. Not smart enough, just redraw's everything - yikes!
 */
void IV::Draw_RAM()
{
	const static int ram_start_y = 490;
	const static int ram_start_x = 10;
	static u16 old_addr = start_addr;

	// set rendering context to ram texture
	SDL_SetRenderTarget(prend, TM::Instance()->Get_Texture_Info(ram_texture_id).ptexture);
	if (first_draw || old_addr != start_addr)
	{
		int x = 0, y = 0;
		if (old_addr + 16 == start_addr)
		{
			// scroll down
			TM::Instance()->Scroll_Texture_Down(x, glyph_info.h * 2,
				glyph_info.h, ram_texture_id, prend);

			// draw the last line
			y = glyph_info.h * 16;
			u16 start = start_addr + 240;

			// the address label
			Draw_Hex16(start, x, y, 1);
			TM::Instance()->Draw(colon_texture_id, prend, x, y);
			
			x += (glyph_info.w << 1);
			for (int i = 0; i < 16; i++)
			{
				Draw_Hex8(pnes->ram[start + i], x, y);
				x += glyph_info.w;
			} // end for draw line
		} // end if scrolling down
		else if (start_addr == old_addr - 16)
		{
			// scroll up
			TM::Instance()->Scroll_Texture_Up(x, glyph_info.h, glyph_info.h,
				ram_texture_id, prend);

			// draw the first line
			y = glyph_info.h;

			// the address label
			Draw_Hex16(start_addr, x, y, 1);
			TM::Instance()->Draw(colon_texture_id, prend, x, y);

			x += (glyph_info.w << 1);
			for (int i = 0; i < 16; i++)
			{
				Draw_Hex8(pnes->ram[start_addr + i], x, y);
				x += glyph_info.w;
			} // end for draw line
		} // end else if scrolling up
		else
		{
			// draw everything regardless
			int w = TM::Instance()->Get_Texture_Info(hex_texture_id).w;
			int h = TM::Instance()->Get_Texture_Info(hex_texture_id).h;

			y += h;
			for (u16 row = 0; row < 16; row++)
			{
				u16 addr = start_addr + (row << 4);

				Draw_Hex16(addr, x, y, 1);
				TM::Instance()->Draw(colon_texture_id, prend, x, y);
				x += TM::Instance()->Get_Texture_Info(colon_texture_id).w + (w);

				// hit columns
				for (u16 col = 0; col < 16; col++)
				{
					Draw_Hex8(pnes->Read(col + addr), x, y);
					x += w;
				} // end for

				x = 0;	 // reset
				y += h;
			} // end for row
		} // end else full draw

		old_addr = start_addr;
	} // end if needs to draw

	// draw written address only if it falls within range
	while (!pnes->addr_written.empty())
	{
		u16 a = pnes->addr_written.back();
		if (a >= start_addr && a < start_addr + 256)
		{
			// compute the position based on address
			int diff = a - start_addr;
			int y = ((diff / 16) + 1) * glyph_info.h;

			int x = (diff % 16) * (glyph_info.w * 3) + (6 * glyph_info.w);
			Draw_Hex8(pnes->Read(a), x, y);
		} // end if drawing em

		pnes->addr_written.pop_back();
	} // end while

	// finally  draw to main default texture
	SDL_SetRenderTarget(prend, nullptr);
	TM::Instance()->Draw(ram_texture_id, prend, ram_start_x, ram_start_y);
} // end Draw_RAM

//=====================================================================|
/**
 * @brief Draw's the disassembly view +/-10 lines above and below the
 *	pc by default. It optimizes drawing by only drawing regions that 
 *	changed, i.e. by scrolling up/down whenever possibles.
 */ 
void IV::Draw_Disasm()
{
	const static int dis_start_x = GAME_WIDTH + 10;
	const static int dis_start_y = (glyph_info.h + 15) * 2;
	const static int TOP_LINES = 10;	// 10 above, 10 below, PC middle
	const static int BOT_LINES = 10;

	static TextureInfo dasm_texture = TM::Instance()->Get_Texture_Info(
		disasm_texture_id);
	static u16 old_pc = pnes->cpu.pc;

	if (first_draw || old_pc != pnes->cpu.pc)
	{
		SDL_SetRenderTarget(prend, dasm_texture.ptexture);

		int x = 0;
		int y = 0;

		auto start = disasm_addr.find(pnes->cpu.pc);
		if (start != disasm_addr.end())
		{
			// test if we simply scroll or redraw the entire deal
			if (old_pc + 1 <= pnes->cpu.pc && old_pc + 3 >= pnes->cpu.pc)
			{
				TM::Instance()->Scroll_Texture_Down(0, glyph_info.h, glyph_info.h,
					disasm_texture_id, prend);

				// draw the last line
				int count = 0;
				for (; count < BOT_LINES; count++)
				{
					if (start == disasm_addr.end())
						start = disasm_addr.begin();
					y += glyph_info.h;

					++start;
				} // end for

				Draw_Disasm_Line(start->first, x, y);
			} // end if scroll down
			else
			{
				// do the entire thing
				// move backwards, 10 steps, wrap around if must
				int count = 0;
				while (count++ <= 10)
				{
					if (start == disasm_addr.begin())
						start = disasm_addr.end();

					--start;
				} // end while

				// now hit the drawing
				for (count = 0; count < TOP_LINES + BOT_LINES + 1; count++)
				{
					if (start == disasm_addr.end())
						start = disasm_addr.begin();

					Draw_Disasm_Line(start->first, x, y);
					y += glyph_info.h;

					++start;
				} // end for
			} // end else redraw all
		} // if found the address

		SDL_SetRenderTarget(prend, nullptr);
	} // end if need to draw

	// draw it
	TM::Instance()->Draw(disasm_texture_id, prend, dis_start_x, dis_start_y);

	// highlight the pc
	SDL_Rect hl = { dis_start_x, dis_start_y + glyph_info.h * 10, 
		dasm_texture.w, glyph_info.h };
	SDL_SetRenderDrawBlendMode(prend, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(prend, highlight_color.r, highlight_color.g, 
		highlight_color.b, highlight_color.a);
	SDL_RenderFillRect(prend, &hl);
	SDL_SetRenderDrawBlendMode(prend, SDL_BLENDMODE_NONE);

	// restore black
	SDL_SetRenderDrawColor(prend, 0, 0, 0, 255);
	first_draw = false;
} // end Draw_Disasm

//=====================================================================|
/**
 * @brief create's surfaces that we'll use to render numerics as hex
 *	values. 32 of them, the first 16 in white color and the next 16 in
 *	dull color. They are arranged such that hex_texture_id + number 
 *	yeilds the correct id to the numeric texture, and offseting by 16
 *	gives the dull colored variant.
 */
void IV::Create_Numeric_Textures()
{
	const static std::string hex_digits[]{ "0", "1", "2", "3", "4", "5", 
		"6", "7", "8", "9", "A", "B", "C", "D", "E", "F" };

	hex_texture_id = TextureManager::Instance()->Get_Next_ID();
	int count = 0;
	SDL_Color color[] = { text_color, label_color, off_color };
	
	while (count < 3)
	{
		for (auto& hex : hex_digits)
		{
			TextureManager::Instance()->Create_Text_Texture(
				hex, color[count], prend);
		} // end for

		++count;
	} // end while
} // end Create_Numeric_Textures

//=====================================================================|
/**
 * @brief Create's textures that will render as flag or status register
 *	values of 6502 as on (bright green) or off (dull gray) to indicate
 *	CPU state. The texture's are created in an alternating format, 1 for
 *	on color and 1 for off color begining with the first status, Negative
 *	flag.
 */
void IV::Create_Cpu_Flags_Texture()
{
	//const static std::string flag_lables[]{ "N", "V", "U", "B", "D",
	//	"I", "Z", "C" };
	const static std::string flags{ "N  V  U  B  D  I  Z  C" };

	status_texture_id = TM::Instance()->Get_Next_ID();
	TM::Instance()->Create_Text_Texture(flags, off_color, prend);

	// now hit the template
	flags_texture_id = TM::Instance()->Get_Next_ID();
	for (int i = 0; i < flags.length(); i+=3)
	{
		std::string flag{ flags[i] };
		TM::Instance()->
			Create_Text_Texture(flag, off_color, prend);

		TM::Instance()->
			Create_Text_Texture(flag, on_color, prend);
	} // end for
} // end Make_Cpu_Flags_Texture

//=====================================================================|
/**
 * @brief in a similar spirt to Create_Cpu_Flag_Textures, this function
 *	create's textures for the 6502 cpu registers for fast drawing.
 */
void IV::Create_Cpu_Registers_Textures()
{
	const static std::string registers{ "A:     X:     Y:     SP:     PC:     " };
	registers_texture_id = TM::Instance()->Create_Text_Texture(registers, 
		register_color, prend);
} // end Make_Cpu_Flags_Texture

//=====================================================================|
/**
 * @brief Create's the heading texture for the RAM dump view, since 
 *	heading never changes we can store it once.
 */
void IV::Create_Ram_Texture()
{
	const int ram_width = glyph_info.w * 53;
	const int ram_height = glyph_info.h * 17;

	// create a texture
	ram_texture_id = TM::Instance()->Create_Texture(ram_width, ram_height,
		prend, SDL_TEXTUREACCESS_TARGET);

	// draw some static stuff to it, the header
	SDL_SetRenderTarget(prend, TM::Instance()->Get_Texture_Info(ram_texture_id).ptexture);
	int x = glyph_info.w * 6;
	int y = 0;
	
	for (u16 i = 0; i < 16; i++)
	{
		Draw_Hex8(i, x, y, 1);
		x += glyph_info.w;
	} // end for

	// set default renderer
	SDL_SetRenderTarget(prend, nullptr);
} // end Create_Ram_Texture

//=====================================================================|
/**
 * @brief makes textures used for rendering special symbols such as
 *	dollar, pound, colons.
 */
void IV::Create_Symbol_Textures()
{
	dollar$_texture_id = TM::Instance()->Create_Text_Texture(
		"$", text_color, prend);
	pound_texture_id = TM::Instance()->Create_Text_Texture(
		"#", text_color, prend);
	colon_texture_id = TM::Instance()->Create_Text_Texture(
		":", label_color, prend);
	comma_texture_id = TM::Instance()->Create_Text_Texture(
		",", text_color, prend);

	// do the braket pair
	bracket_texture_id = TM::Instance()->Create_Text_Texture(
		"(", text_color, prend);
	TM::Instance()->Create_Text_Texture(
		")", text_color, prend);	// +1

	// and square version
	squareb_texture_id = TM::Instance()->Create_Text_Texture(
		"[", text_color, prend);
	TM::Instance()->Create_Text_Texture(
		"]", text_color, prend);	// +1

	a_texture_id = TM::Instance()->Create_Text_Texture("A", text_color, prend);
	x_texture_id = TM::Instance()->Create_Text_Texture("X", text_color, prend);
	y_texture_id = TM::Instance()->Create_Text_Texture("Y", text_color, prend);
} // end Create_Glpyh_Table_Textures

//=====================================================================|
/**
 * @breif create's a texture of size mxn to blit texts at will from the
 *	disassembled code and texture of texts for mneumonics
 */
void IV::Create_Disasm_Texture()
{
	const static int dasm_width = glyph_info.w * 36;
	const static int dasm_height = glyph_info.h * 21;	// 21-lines

	disasm_texture_id = TM::Instance()->Create_Texture(dasm_width, dasm_height,
		prend, SDL_TEXTUREACCESS_TARGET);

	// now create all textures for mneuonics
	std::map<std::string, int> seen;	// tracks dups
	for (auto& m : disasm_addr)
	{
		if (seen[m.second.mnemonic]++ < 1)
		{
			TM::Instance()->Create_Mneumonic_Texture(m.second.mnemonic,
				mnemonic_color, prend);
		}
	} // end for 
	seen.clear();

	// create textures for the 12 addressing modes
	const std::vector<std::string> addr_modes{ ADDR_IMP, ADDR_IMM, ADDR_ZP0, 
		ADDR_ZPX, ADDR_ZPY, ADDR_IZX, ADDR_IZY, ADDR_ABS, ADDR_ABX, 
		ADDR_ABY, ADDR_IND, ADDR_REL };
	for (auto& s : addr_modes)
	{
		TM::Instance()->Create_Mneumonic_Texture(s,
			addr_mode_color, prend);
	} // end for
} // end Create_Disasm_Texture

//=====================================================================|
/**
 * @brief draw's the cpu status flags by choosing the correct texture,
 *	i.e. text + color for each state. Each texture is stored in alternating
 *	on/off manner begining with negative, N flag
 */
void IV::Draw_CPU_Flags()
{
	const static int flag_x = GAME_WIDTH + 147;		// x starting point
	const static int flag_y = 10;					// y starting point
	const static int flag_x_gap = 15;				// gap between flags

	const static u8 status = pnes->cpu.status;		// keep a copy of the old status states
	const static TextureInfo status_texture = TM::Instance()->Get_Texture_Info(status_texture_id);

	// helper to make loops easy
	const static u8 registers[]{ N, V, U, B, D, I, Z, C };
	SDL_SetRenderTarget(prend, status_texture.ptexture);

	// check if we need to update anything
	if (status != pnes->cpu.status)
	{
		int x = 0;
		int y = 0;

		// alright redraw only the difference
		u8 s = pnes->cpu.status;	// shorten
		for (int i = 0; i < 8; i++)
		{
			u8 fv = GET_FLAG(s, registers[i]);
			if (fv != GET_FLAG(status, registers[i]))
			{
				if (fv)
					TM::Instance()->Draw(flags_texture_id + (i << 1) + 1, prend, x, y);
				else
					TM::Instance()->Draw(flags_texture_id + (i << 1), prend, x, y);
			} // end if

			x += glyph_info.w * 3;
		} // end for
	} // end if difference

	SDL_SetRenderTarget(prend, nullptr);
	TM::Instance()->Draw(status_texture_id, prend, flag_x, flag_y);
} // end draw_flags

//=====================================================================|
/**
 * @brief draw's each register in a specific order along with values 
 *	begining witht the accumulator, A register. Once drawn, it no longer
 *	redraw's the same static images over again to avoid redundancy
 */
void IV::Draw_Registers()
{
	const static int register_x = GAME_WIDTH + 10;
	const static int register_y = glyph_info.h + 15;

	// make local copy, and draw only on change
	static u8 shadows[]{ pnes->cpu.a, pnes->cpu.x, pnes->cpu.y, pnes->cpu.sp };
	static u16 shadow_pc = pnes->cpu.pc;

	// flatten registers into array for easy access
	u8 registers[]{ pnes->cpu.a, pnes->cpu.x, pnes->cpu.y, pnes->cpu.sp };

	SDL_SetRenderTarget(prend, TM::Instance()->Get_Texture_Info(registers_texture_id).ptexture);
	int x = glyph_info.w * 3;
	int y = 0;

	for (int i = 0; i < 4; i++)
	{
		if (first_draw || registers[i] != shadows[i])
		{
			if (i == 3)
				x += glyph_info.w;

			Draw_Hex8(registers[i], x, y);
			shadows[i] = registers[i];
		} // end if first time or on change
		else
		{
			x += (glyph_info.w << 1);
		} // end else just skipping

		x += glyph_info.w * 5;
	} // end for bundle test

	// finally draw pc
	x += glyph_info.w;	// offset by one
	if (first_draw || pnes->cpu.pc != shadow_pc)
	{
		Draw_Hex16(pnes->cpu.pc, x, y);
		shadow_pc = pnes->cpu.pc;
	} // end if pc

	SDL_SetRenderTarget(prend, nullptr);
	TM::Instance()->Draw(registers_texture_id, prend, register_x, register_y);
} // end Draw_Registers

//=====================================================================|
/**
 * @brief draw's a line of disassembly using the template texture for
 *	fast bliting.
 *
 * @param addr the 16 bit address
 * @param x the x-pos
 * @param y the y-pos
 */
void IV::Draw_Disasm_Line(u16 addr, int x, int y)
{
	u8 opcode = pnes->cpu.Read(addr++);

	// draw the address label
	Draw_Hex16(addr++, x, y, 1);
	TM::Instance()->Draw(colon_texture_id, prend, x, y);

	// draw the op-codes
	x = glyph_info.w * 6;
	int count = 0;
	while (count++ < pnes->cpu.lookup[opcode].bytes)
	{
		Draw_Hex8(pnes->Read(addr), x, y, 2);
		x += glyph_info.w;	// space
	} // end while

	// now draw the menonic
	x = glyph_info.w * 15;
	int id = TM::Instance()->Get_Texture_ID(pnes->cpu.lookup[opcode].name);
	TM::Instance()->Draw(id, prend, x, y);

	// draw the operands
	x = glyph_info.w * 21;
	if (pnes->cpu.lookup[opcode].Addrmode == &CPU6502::IMP)
	{
		// just draw the addressing mode
		x = glyph_info.w * 31;
		id = TM::Instance()->Get_Texture_ID(ADDR_IMP);
		TM::Instance()->Draw(id, prend, x, y);
	} // end if implied
	else if (pnes->cpu.lookup[opcode].Addrmode == &CPU6502::IMM)
	{
		u8 value = pnes->cpu.Read(addr++);

		TM::Instance()->Draw(pound_texture_id, prend, x, y);
		x += glyph_info.w;

		TM::Instance()->Draw(dollar$_texture_id, prend, x, y);
		x += glyph_info.w;

		Draw_Hex8(value, x, y);

		// draw the addressing mode
		x = glyph_info.w * 31;
		id = TM::Instance()->Get_Texture_ID(ADDR_IMM);
		TM::Instance()->Draw(id, prend, x, y);
	} // end else immediate
	else if (pnes->cpu.lookup[opcode].Addrmode == &CPU6502::ZP0)
	{
		u8 lo = pnes->Read(addr++);

		TM::Instance()->Draw(dollar$_texture_id, prend, x, y);
		x += glyph_info.w;

		Draw_Hex8(lo, x, y);

		// draw the addressing mode
		x = glyph_info.w * 31;
		id = TM::Instance()->Get_Texture_ID(ADDR_ZP0);
		TM::Instance()->Draw(id, prend, x, y);
	} // end else zero page 0
	else if (pnes->cpu.lookup[opcode].Addrmode == &CPU6502::ZPX)
	{
		u8 lo = pnes->Read(addr++);

		TM::Instance()->Draw(dollar$_texture_id, prend, x, y);
		x += glyph_info.w;

		Draw_Hex8(lo, x, y);

		TM::Instance()->Draw(comma_texture_id, prend, x, y);
		x += glyph_info.w * 2;

		TM::Instance()->Draw(x_texture_id, prend, x, y);

		// draw the addressing mode
		x = glyph_info.w * 31;
		id = TM::Instance()->Get_Texture_ID(ADDR_ZPX);
		TM::Instance()->Draw(id, prend, x, y);
	} // end else zero page x
	else if (pnes->cpu.lookup[opcode].Addrmode == &CPU6502::ZPY)
	{
		u8 lo = pnes->Read(addr++);

		TM::Instance()->Draw(dollar$_texture_id, prend, x, y);
		x += glyph_info.w;

		Draw_Hex8(lo, x, y);

		TM::Instance()->Draw(comma_texture_id, prend, x, y);
		x += glyph_info.w * 2;

		TM::Instance()->Draw(y_texture_id, prend, x, y);

		// draw the addressing mode
		x = glyph_info.w * 31;
		id = TM::Instance()->Get_Texture_ID(ADDR_ZPY);
		TM::Instance()->Draw(id, prend, x, y);
	} // end else zero page y
	else if (pnes->cpu.lookup[opcode].Addrmode == &CPU6502::IZX)
	{
		u8 lo = pnes->Read(addr++);

		TM::Instance()->Draw(bracket_texture_id, prend, x, y);
		x += glyph_info.w;

		TM::Instance()->Draw(dollar$_texture_id, prend, x, y);
		x += glyph_info.w;

		Draw_Hex8(lo, x, y);

		TM::Instance()->Draw(comma_texture_id, prend, x, y);
		x += glyph_info.w * 2;

		TM::Instance()->Draw(x_texture_id, prend, x, y);
		x += glyph_info.w;

		TM::Instance()->Draw(bracket_texture_id + 1, prend, x, y);

		// draw the addressing mode
		x = glyph_info.w * 31;
		id = TM::Instance()->Get_Texture_ID(ADDR_IZX);
		TM::Instance()->Draw(id, prend, x, y);
	} // end indirect x addressing
	else if (pnes->cpu.lookup[opcode].Addrmode == &CPU6502::IZY)
	{
		u8 lo = pnes->Read(addr++);

		TM::Instance()->Draw(bracket_texture_id, prend, x, y);
		x += glyph_info.w;

		TM::Instance()->Draw(dollar$_texture_id, prend, x, y);
		x += glyph_info.w;

		Draw_Hex8(lo, x, y);

		TM::Instance()->Draw(comma_texture_id, prend, x, y);
		x += glyph_info.w * 2;

		TM::Instance()->Draw(y_texture_id, prend, x, y);
		x += glyph_info.w;

		TM::Instance()->Draw(bracket_texture_id + 1, prend, x, y);

		// draw the addressing mode
		x = glyph_info.w * 31;
		id = TM::Instance()->Get_Texture_ID(ADDR_IZY);
		TM::Instance()->Draw(id, prend, x, y);
	} // end else indirect y addressing
	else if (pnes->cpu.lookup[opcode].Addrmode == &CPU6502::ABS)
	{
		u8 lo = pnes->Read(addr++);
		u8 hi = pnes->Read(addr++);

		TM::Instance()->Draw(dollar$_texture_id, prend, x, y);
		x += glyph_info.w;

		Draw_Hex8(hi, x, y);
		Draw_Hex8(lo, x, y);

		// draw the addressing mode
		x = glyph_info.w * 31;
		id = TM::Instance()->Get_Texture_ID(ADDR_ABS);
		TM::Instance()->Draw(id, prend, x, y);
	} // end else absolute addressing
	else if (pnes->cpu.lookup[opcode].Addrmode == &CPU6502::ABX)
	{
		u8 lo = pnes->Read(addr++);
		u8 hi = pnes->Read(addr++);

		TM::Instance()->Draw(dollar$_texture_id, prend, x, y);
		x += glyph_info.w;

		Draw_Hex8(hi, x, y);
		Draw_Hex8(lo, x, y);

		TM::Instance()->Draw(comma_texture_id, prend, x, y);
		x += glyph_info.w * 2;

		TM::Instance()->Draw(x_texture_id, prend, x, y);

		// draw the addressing mode
		x = glyph_info.w * 31;
		id = TM::Instance()->Get_Texture_ID(ADDR_ABX);
		TM::Instance()->Draw(id, prend, x, y);
	} // end else absoulte x indexing
	else if (pnes->cpu.lookup[opcode].Addrmode == &CPU6502::ABY)
	{
		u8 lo = pnes->Read(addr++);
		u8 hi = pnes->Read(addr++);

		TM::Instance()->Draw(dollar$_texture_id, prend, x, y);
		x += glyph_info.w;

		Draw_Hex8(hi, x, y);
		Draw_Hex8(lo, x, y);

		TM::Instance()->Draw(comma_texture_id, prend, x, y);
		x += glyph_info.w * 2;

		TM::Instance()->Draw(y_texture_id, prend, x, y);

		// draw the addressing mode
		x = glyph_info.w * 31;
		id = TM::Instance()->Get_Texture_ID(ADDR_ABY);
		TM::Instance()->Draw(id, prend, x, y);
	} // end else absolute y
	else if (pnes->cpu.lookup[opcode].Addrmode == &CPU6502::IND)
	{
		u8 lo = pnes->Read(addr++); 
		u8 hi = pnes->Read(addr++); 

		TM::Instance()->Draw(bracket_texture_id, prend, x, y);
		x += glyph_info.w;

		TM::Instance()->Draw(dollar$_texture_id, prend, x, y);
		x += glyph_info.w;

		Draw_Hex8(hi, x, y);
		Draw_Hex8(lo, x, y);

		TM::Instance()->Draw(bracket_texture_id + 1, prend, x, y);
		
		// draw the addressing mode
		x = glyph_info.w * 31;
		id = TM::Instance()->Get_Texture_ID(ADDR_IND);
		TM::Instance()->Draw(id, prend, x, y);
	} // end else indirect
	else
	{
		// presume REL
		u8 value = pnes->Read(addr++);

		TM::Instance()->Draw(squareb_texture_id, prend, x, y);
		x += glyph_info.w;

		TM::Instance()->Draw(dollar$_texture_id, prend, x, y);
		x += glyph_info.w;

		Draw_Hex16(addr + value, x, y);

		TM::Instance()->Draw(squareb_texture_id + 1, prend, x, y);
		
		// draw the addressing mode
		x = glyph_info.w * 31;
		id = TM::Instance()->Get_Texture_ID(ADDR_REL);
		TM::Instance()->Draw(id, prend, x, y);
	} // end at last relative
} // end Draw_Disasm_Line

//=====================================================================|
/**
 * @brief draws a hex texture onto the main surface or rendering surface
 *	provided by SDL. It basically splits a u8 value into 2 and draws 
 *	each nibble as hex twice.
 *
 * @param num the 8-bit number to draw, texture indexed using its value
 * @param x the x-coordinate for drawing
 * @param y the y-coordinate 
 * @param color_index used to index the correct colored hex texture (0 - 2)
 */
inline void IV::Draw_Hex8(const u8 num, int& x, int& y, const u8 color_index)
{
	// since all hex textures are monospaced fonts; i.e. consola
	static const int num_text_width = TM::Instance()->
		Get_Texture_Info(hex_texture_id).w;	

	// split num into 2 nibbles
	u8 hi = (num & 0xf0) >> 4;
	u8 lo = num & 0x0f;

	int hi_texture_id = ((0x03 & color_index) << 4) + hex_texture_id + hi;
	int lo_texture_id = ((0x03 & color_index) << 4) + hex_texture_id + lo;

	TM::Instance()->Draw(hi_texture_id, prend, x, y); x += num_text_width;
	TM::Instance()->Draw(lo_texture_id, prend, x, y); x += num_text_width;
} // end Draw_Hex8

//=====================================================================|
/**
 * @brief the 16-bit version of Draw_Hex8, basically calls it twice
 *
 * @param num the 16-bit number to draw, texture indexed using its value
 * @param x the x-coordinate for drawing
 * @param y the y-coordinate
 * @param color_index used to index the correct colored hex texture (0 - 2)
 */
inline void IV::Draw_Hex16(const u16 num, int& x, int& y, const u8 color_index)
{
	// split to bytes
	u8 hi = (num & 0xff00) >> 8;
	u8 lo = (num & 0x00ff);

	Draw_Hex8(hi, x, y, color_index);
	Draw_Hex8(lo, x, y, color_index);
} // end Draw_Hex16

//=====================================================================|
/**
 * @brief builds a list of disassembly addresses for quick referencing
 *	or walking across back and forth.
 *
 * @param start the starting address defaults to 0
 * @param end the ending address defaults to 65536
 */
void IV::Build_Disasm(const u16 start, const u16 end)
{
	u8 value = 0, lo = 0, hi = 0;		// temps
	u16 line_addr = 0;

	u32 addr = start;
	while (addr <= (u32)end)
	{
		line_addr = addr;
		Disasm_Mnemonic dm;

		uint8_t opcode = pnes->Read(addr);

		dm.mnemonic += pnes->cpu.lookup[opcode].name;
		addr += pnes->cpu.lookup[opcode].bytes;

		// Add the formed string to a std::map, using the instruction's
		// address as the key. 
		disasm_addr[line_addr] = dm;
	} // end while
} // end Full_Disasm