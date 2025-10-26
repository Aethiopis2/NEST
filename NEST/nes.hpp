/**
 * @brief A definiton for that old classic retro game console Nintendo Home 
 *	Entertainment System, NES. I personally never owned it but I did own a cheap-shot
 *	copy version of it (or that played similar games) called Terminator II (how fitting).
 *	NES conists of the following devices which we emulate:
 *		1. An 8-bit CPU with 16-bit address range, 6502 with Decimal mode disallowed
 *		2. A 2KB physical RAM that is mirrored every 8KB
 *
 * @author Rediet Worku
 * @date 26th of October 2021, Sunday
 */
#pragma once


//=====================================================================|
#include "basics.hpp"
#include "cpu6502.h"



//=====================================================================|
constexpr u16 RAM_SIZE = 65'536;		// the size of NES RAM



//=====================================================================|
class NES
{
public:

	NES();
	~NES();

	void Write(const u16 address, const u8 data);
	u8 Read(const u16 address) const;

private:

	// connected devices
	CPU6502 cpu;		
	u8 ram[RAM_SIZE];
};