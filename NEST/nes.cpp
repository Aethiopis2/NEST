/**
 * @brief The implementation NES emulator, the NEST.
 *
 * @author Rediet Worku
 * @date 26th of October 2021, Sunday
 */

//=====================================================================|
#include "nes.hpp"



//=====================================================================|
/**
 * @brief
 */
NES::NES()
{
	iZero(ram, RAM_SIZE);
	cpu.Connect_NES(this);
} // end NES


//=====================================================================|
/**
 * @brief
 */
NES::~NES() { }


//=====================================================================|
/**
 * @brief writes an 8-bit data to the address specified. This function
 *	in a sense acts like a bus that connects all devices that are connected
 *	to NES.
 * 
 * @param address the 16-bit address
 * @param data the 8-bit value to write
 */
void NES::Write(const u16 address, const u8 data)
{
	if (address >= 0 && address <= 0xFFFF)
	{
		ram[address] = data;
		addr_written.push_back(address);
	} // end if addr written to
} // end Write


//=====================================================================|
/**
 * brief reads the 16-bit address provided it's valid and returns the 
 *	8-bit value stored at that address. Togther the Read/Write functions
 *	form the heart of NES bus.
 *
 * @param address the 16-bit address to read data from
 */
u8 NES::Read(const u16 address) const
{
	if (address >= 0x0000 && address <= 0xFFFF)
		return ram[address];

	return 0;
} // end Read