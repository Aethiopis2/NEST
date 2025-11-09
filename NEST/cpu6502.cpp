/**
 * @brief The definition for 6502 NES CPU
 *
 * @author Rediet Worku
 * @date 26th of October 2021, Sunday
 */

//=====================================================================|
#include "cpu6502.hpp"
#include "nes.hpp"



//=====================================================================|
/**
 * constructor
 */
CPU6502::CPU6502()
	:nes{ nullptr },
	a{ 0 }, x{ 0 }, y{ 0 }, sp{ 0 }, pc{ 0 }, status{ 0 },
	addr_abs{ 0 }, addr_rel{ 0 }, cycles{ 0 }, fetched{ 0 }, opcode{ 0 }
{
	using a = CPU6502;
	lookup =
	{
		{ "BRK", &a::BRK, &a::IMP, 7, 1 },{ "ORA", &a::ORA, &a::IZX, 6, 2 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 8, 1 },{ "???", &a::NOP, &a::IMP, 3, 1 },{ "ORA", &a::ORA, &a::ZP0, 3, 2 },{ "ASL", &a::ASL, &a::ZP0, 5, 2 },{ "???", &a::UNK, &a::IMP, 5, 1 },{ "PHP", &a::PHP, &a::IMP, 3, 1 },{ "ORA", &a::ORA, &a::IMM, 2, 2 },{ "ASL", &a::ASL, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "???", &a::NOP, &a::IMP, 4, 1 },{ "ORA", &a::ORA, &a::ABS, 4, 3 },{ "ASL", &a::ASL, &a::ABS, 6, 3 },{ "???", &a::UNK, &a::IMP, 6, 1 },
		{ "BPL", &a::BPL, &a::REL, 2, 2 },{ "ORA", &a::ORA, &a::IZY, 5, 2 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 8, 1 },{ "???", &a::NOP, &a::IMP, 4, 1 },{ "ORA", &a::ORA, &a::ZPX, 4, 2 },{ "ASL", &a::ASL, &a::ZPX, 6, 2 },{ "???", &a::UNK, &a::IMP, 6, 1 },{ "CLC", &a::CLC, &a::IMP, 2, 1 },{ "ORA", &a::ORA, &a::ABY, 4, 3 },{ "???", &a::NOP, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 7, 1 },{ "???", &a::NOP, &a::IMP, 4, 1 },{ "ORA", &a::ORA, &a::ABX, 4, 3 },{ "ASL", &a::ASL, &a::ABX, 7, 3 },{ "???", &a::UNK, &a::IMP, 7, 1 },
		{ "JSR", &a::JSR, &a::ABS, 6, 3 },{ "AND", &a::AND, &a::IZX, 6, 2 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 8, 1 },{ "BIT", &a::BIT, &a::ZP0, 3, 2 },{ "AND", &a::AND, &a::ZP0, 3, 2 },{ "ROL", &a::ROL, &a::ZP0, 5, 2 },{ "???", &a::UNK, &a::IMP, 5, 1 },{ "PLP", &a::PLP, &a::IMP, 4, 1 },{ "AND", &a::AND, &a::IMM, 2, 2 },{ "ROL", &a::ROL, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "BIT", &a::BIT, &a::ABS, 4, 3 },{ "AND", &a::AND, &a::ABS, 4, 3 },{ "ROL", &a::ROL, &a::ABS, 6, 3 },{ "???", &a::UNK, &a::IMP, 6, 1 },
		{ "BMI", &a::BMI, &a::REL, 2, 2 },{ "AND", &a::AND, &a::IZY, 5, 2 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 8, 1 },{ "???", &a::NOP, &a::IMP, 4, 1 },{ "AND", &a::AND, &a::ZPX, 4, 2 },{ "ROL", &a::ROL, &a::ZPX, 6, 2 },{ "???", &a::UNK, &a::IMP, 6, 1 },{ "SEC", &a::SEC, &a::IMP, 2, 1 },{ "AND", &a::AND, &a::ABY, 4, 3 },{ "???", &a::NOP, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 7, 1 },{ "???", &a::NOP, &a::IMP, 4, 1 },{ "AND", &a::AND, &a::ABX, 4, 3 },{ "ROL", &a::ROL, &a::ABX, 7, 3 },{ "???", &a::UNK, &a::IMP, 7, 1 },
		{ "RTI", &a::RTI, &a::IMP, 6, 1 },{ "EOR", &a::EOR, &a::IZX, 6, 2 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 8, 1 },{ "???", &a::NOP, &a::IMP, 3, 1 },{ "EOR", &a::EOR, &a::ZP0, 3, 2 },{ "LSR", &a::LSR, &a::ZP0, 5, 2 },{ "???", &a::UNK, &a::IMP, 5, 1 },{ "PHA", &a::PHA, &a::IMP, 3, 1 },{ "EOR", &a::EOR, &a::IMM, 2, 2 },{ "LSR", &a::LSR, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "JMP", &a::JMP, &a::ABS, 3, 3 },{ "EOR", &a::EOR, &a::ABS, 4, 3 },{ "LSR", &a::LSR, &a::ABS, 6, 3 },{ "???", &a::UNK, &a::IMP, 6, 1 },
		{ "BVC", &a::BVC, &a::REL, 2, 2 },{ "EOR", &a::EOR, &a::IZY, 5, 2 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 8, 1 },{ "???", &a::NOP, &a::IMP, 4, 1 },{ "EOR", &a::EOR, &a::ZPX, 4, 2 },{ "LSR", &a::LSR, &a::ZPX, 6, 2 },{ "???", &a::UNK, &a::IMP, 6, 1 },{ "CLI", &a::CLI, &a::IMP, 2, 1 },{ "EOR", &a::EOR, &a::ABY, 4, 3 },{ "???", &a::NOP, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 7, 1 },{ "???", &a::NOP, &a::IMP, 4, 1 },{ "EOR", &a::EOR, &a::ABX, 4, 3 },{ "LSR", &a::LSR, &a::ABX, 7, 3 },{ "???", &a::UNK, &a::IMP, 7, 1 },
		{ "RTS", &a::RTS, &a::IMP, 6, 1 },{ "ADC", &a::ADC, &a::IZX, 6, 2 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 8, 1 },{ "???", &a::NOP, &a::IMP, 3, 1 },{ "ADC", &a::ADC, &a::ZP0, 3, 2 },{ "ROR", &a::ROR, &a::ZP0, 5, 2 },{ "???", &a::UNK, &a::IMP, 5, 1 },{ "PLA", &a::PLA, &a::IMP, 4, 1 },{ "ADC", &a::ADC, &a::IMM, 2, 2 },{ "ROR", &a::ROR, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "JMP", &a::JMP, &a::IND, 5, 3 },{ "ADC", &a::ADC, &a::ABS, 4, 3 },{ "ROR", &a::ROR, &a::ABS, 6, 3 },{ "???", &a::UNK, &a::IMP, 6, 1 },
		{ "BVS", &a::BVS, &a::REL, 2, 2 },{ "ADC", &a::ADC, &a::IZY, 5, 2 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 8, 1 },{ "???", &a::NOP, &a::IMP, 4, 1 },{ "ADC", &a::ADC, &a::ZPX, 4, 2 },{ "ROR", &a::ROR, &a::ZPX, 6, 2 },{ "???", &a::UNK, &a::IMP, 6, 1 },{ "SEI", &a::SEI, &a::IMP, 2, 1 },{ "ADC", &a::ADC, &a::ABY, 4, 3 },{ "???", &a::NOP, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 7, 1 },{ "???", &a::NOP, &a::IMP, 4, 1 },{ "ADC", &a::ADC, &a::ABX, 4, 3 },{ "ROR", &a::ROR, &a::ABX, 7, 3 },{ "???", &a::UNK, &a::IMP, 7, 1 },
		{ "???", &a::NOP, &a::IMP, 2, 1 },{ "STA", &a::STA, &a::IZX, 6, 2 },{ "???", &a::NOP, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 6, 1 },{ "STY", &a::STY, &a::ZP0, 3, 2 },{ "STA", &a::STA, &a::ZP0, 3, 2 },{ "STX", &a::STX, &a::ZP0, 3, 2 },{ "???", &a::UNK, &a::IMP, 3, 1 },{ "DEY", &a::DEY, &a::IMP, 2, 1 },{ "???", &a::NOP, &a::IMP, 2, 1 },{ "TXA", &a::TXA, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "STY", &a::STY, &a::ABS, 4, 3 },{ "STA", &a::STA, &a::ABS, 4, 3 },{ "STX", &a::STX, &a::ABS, 4, 3 },{ "???", &a::UNK, &a::IMP, 4, 1 },
		{ "BCC", &a::BCC, &a::REL, 2, 2 },{ "STA", &a::STA, &a::IZY, 6, 2 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 6, 1 },{ "STY", &a::STY, &a::ZPX, 4, 2 },{ "STA", &a::STA, &a::ZPX, 4, 2 },{ "STX", &a::STX, &a::ZPY, 4, 2 },{ "???", &a::UNK, &a::IMP, 4, 1 },{ "TYA", &a::TYA, &a::IMP, 2, 1 },{ "STA", &a::STA, &a::ABY, 5, 3 },{ "TXS", &a::TXS, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 5, 1 },{ "???", &a::NOP, &a::IMP, 5, 1 },{ "STA", &a::STA, &a::ABX, 5, 3 },{ "???", &a::UNK, &a::IMP, 5, 1 },{ "???", &a::UNK, &a::IMP, 5, 1 },
		{ "LDY", &a::LDY, &a::IMM, 2, 2 },{ "LDA", &a::LDA, &a::IZX, 6, 2 },{ "LDX", &a::LDX, &a::IMM, 2, 2 },{ "???", &a::UNK, &a::IMP, 6, 1 },{ "LDY", &a::LDY, &a::ZP0, 3, 2 },{ "LDA", &a::LDA, &a::ZP0, 3, 2 },{ "LDX", &a::LDX, &a::ZP0, 3, 2 },{ "???", &a::UNK, &a::IMP, 3, 1 },{ "TAY", &a::TAY, &a::IMP, 2, 1 },{ "LDA", &a::LDA, &a::IMM, 2, 2 },{ "TAX", &a::TAX, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "LDY", &a::LDY, &a::ABS, 4, 3 },{ "LDA", &a::LDA, &a::ABS, 4, 3 },{ "LDX", &a::LDX, &a::ABS, 4, 3 },{ "???", &a::UNK, &a::IMP, 4, 1 },
		{ "BCS", &a::BCS, &a::REL, 2, 2 },{ "LDA", &a::LDA, &a::IZY, 5, 2 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 5, 1 },{ "LDY", &a::LDY, &a::ZPX, 4, 2 },{ "LDA", &a::LDA, &a::ZPX, 4, 2 },{ "LDX", &a::LDX, &a::ZPY, 4, 2 },{ "???", &a::UNK, &a::IMP, 4, 1 },{ "CLV", &a::CLV, &a::IMP, 2, 1 },{ "LDA", &a::LDA, &a::ABY, 4, 3 },{ "TSX", &a::TSX, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 4, 1 },{ "LDY", &a::LDY, &a::ABX, 4, 3 },{ "LDA", &a::LDA, &a::ABX, 4, 3 },{ "LDX", &a::LDX, &a::ABY, 4, 3 },{ "???", &a::UNK, &a::IMP, 4, 1 },
		{ "CPY", &a::CPY, &a::IMM, 2, 2 },{ "CMP", &a::CMP, &a::IZX, 6, 2 },{ "???", &a::NOP, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 8, 1 },{ "CPY", &a::CPY, &a::ZP0, 3, 2 },{ "CMP", &a::CMP, &a::ZP0, 3, 2 },{ "DEC", &a::DEC, &a::ZP0, 5, 2 },{ "???", &a::UNK, &a::IMP, 5, 1 },{ "INY", &a::INY, &a::IMP, 2, 1 },{ "CMP", &a::CMP, &a::IMM, 2, 2 },{ "DEX", &a::DEX, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "CPY", &a::CPY, &a::ABS, 4, 3 },{ "CMP", &a::CMP, &a::ABS, 4, 3 },{ "DEC", &a::DEC, &a::ABS, 6, 3 },{ "???", &a::UNK, &a::IMP, 6, 1 },
		{ "BNE", &a::BNE, &a::REL, 2, 2 },{ "CMP", &a::CMP, &a::IZY, 5, 2 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 8, 1 },{ "???", &a::NOP, &a::IMP, 4, 1 },{ "CMP", &a::CMP, &a::ZPX, 4, 2 },{ "DEC", &a::DEC, &a::ZPX, 6, 2 },{ "???", &a::UNK, &a::IMP, 6, 1 },{ "CLD", &a::CLD, &a::IMP, 2, 1 },{ "CMP", &a::CMP, &a::ABY, 4, 3 },{ "NOP", &a::NOP, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 7, 1 },{ "???", &a::NOP, &a::IMP, 4, 1 },{ "CMP", &a::CMP, &a::ABX, 4, 3 },{ "DEC", &a::DEC, &a::ABX, 7, 3 },{ "???", &a::UNK, &a::IMP, 7, 1 },
		{ "CPX", &a::CPX, &a::IMM, 2, 2 },{ "SBC", &a::SBC, &a::IZX, 6, 2 },{ "???", &a::NOP, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 8, 1 },{ "CPX", &a::CPX, &a::ZP0, 3, 2 },{ "SBC", &a::SBC, &a::ZP0, 3, 2 },{ "INC", &a::INC, &a::ZP0, 5, 2 },{ "???", &a::UNK, &a::IMP, 5, 1 },{ "INX", &a::INX, &a::IMP, 2, 1 },{ "SBC", &a::SBC, &a::IMM, 2, 2 },{ "NOP", &a::NOP, &a::IMP, 2, 1 },{ "???", &a::SBC, &a::IMP, 2, 1 },{ "CPX", &a::CPX, &a::ABS, 4, 3 },{ "SBC", &a::SBC, &a::ABS, 4, 3 },{ "INC", &a::INC, &a::ABS, 6, 3 },{ "???", &a::UNK, &a::IMP, 6, 1 },
		{ "BEQ", &a::BEQ, &a::REL, 2, 2 },{ "SBC", &a::SBC, &a::IZY, 5, 2 },{ "???", &a::UNK, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 8, 1 },{ "???", &a::NOP, &a::IMP, 4, 1 },{ "SBC", &a::SBC, &a::ZPX, 4, 2 },{ "INC", &a::INC, &a::ZPX, 6, 2 },{ "???", &a::UNK, &a::IMP, 6, 1 },{ "SED", &a::SED, &a::IMP, 2, 1 },{ "SBC", &a::SBC, &a::ABY, 4, 3 },{ "NOP", &a::NOP, &a::IMP, 2, 1 },{ "???", &a::UNK, &a::IMP, 7, 1 },{ "???", &a::NOP, &a::IMP, 4, 1 },{ "SBC", &a::SBC, &a::ABX, 4, 3 },{ "INC", &a::INC, &a::ABX, 7, 3 },{ "???", &a::UNK, &a::IMP, 7, 1 },
	};
} // end constructor


//=====================================================================|
/**
 * Destructor
 */
CPU6502::~CPU6502()
{
}


//=====================================================================|
/**
 * @brief Connects the cpu to an instance of the bus.
 */
void CPU6502::Connect_NES(NES* n)
{
	nes = n;
} // end Connect_NESBus


//=====================================================================|
/**
 * @brief Writes the byte data at the 16-bit address provided
 */
void CPU6502::Write(u16 addr, u8 data)
{
	nes->Write(addr, data);
} // end Write


//=====================================================================|
/**
 * @brief Reads the data from the 16-bit address put out on the bus
 */
u8 CPU6502::Read(u16 addr)
{
	return nes->Read(addr);
} // end Read


//=====================================================================|
/**
 * @brief Runs the cpu clock at every frame of animation. Our little CPU 
 *	is not cycle accurate, it runs all the steps that normally takes 
 *	multiple cycle for an actual NES CPU, in one blow, we then simply 
 *	wait for the clock to expire till we read, decode, and excute the 
 *	next instruction.
 */
void CPU6502::Clock()
{
	if (!cycles)
	{
		opcode = Read(pc++);
		cycles = lookup[opcode].cycles;
		uint8_t add_cycle1 = (this->*lookup[opcode].Addrmode)();
		uint8_t add_cycle2 = (this->*lookup[opcode].Operate)();
		cycles += (add_cycle1 & add_cycle2);
	} // end if

	--cycles;
} // end Clock


//=====================================================================|
/**
 * @brief Reset's the CPU and start's it in the default state; 
 *	i.e. pc = 0xFFFC
 */
void CPU6502::Reset()
{
	a = x = y = 0;
	sp = 0xFD;
	status = 0x0 | U;
	pc = 0xFCFF;		// 0xFFFC

	addr_rel = addr_abs = fetched = 0;
	cycles = 8;		// take your time
} // end Reset


//=====================================================================|
/**
 * @brief Handles the software based interrupts; IRQs -- reads the byte 
 *	stored at address 0xFFFE
 */
void CPU6502::IRQ()
{
	if (!GET_FLAG(status, I))
	{
		Write(0x0100 + sp--, (pc >> 8) & 0x00FF);
		Write(0x0100 + sp--, (pc & 0x00FF));

		SET_FLAG(status, B, 0);
		SET_FLAG(status, U, 1);
		SET_FLAG(status, I, 1);
		Write(0x0100 + sp--, status);

		pc = (((u16)Read(0xFFFF) << 8) | ((u16)Read(0xFFFE)));
		cycles = 7;
	} // end if
} // end IRQ


//=====================================================================|
/**
 * @brief Handles the non maskable interrupt; same as IRQ except that 
 *	this cannot be masked and address is at 0xFFFA
 */
void CPU6502::NMI()
{
	Write(0x0100 + sp--, (pc >> 8) & 0x00FF);
	Write(0x0100 + sp--, (pc & 0x00FF));

	SET_FLAG(status, B, 0);
	SET_FLAG(status, U, 1);
	SET_FLAG(status, I, 1);
	Write(0x0100 + sp--, status);

	pc = (((u16)Read(0xFFFB) << 8) | ((u16)Read(0xFFFA)));
	cycles = 8;
} // end NMI



//=====================================================================|
// ADDRESSING MODES
//=====================================================================|
/**
 * @brief The impled addressing mode, requires no further data to excute 
 *	the instruction, i.e. the opcode itself imples a given instruction. 
 *	It could also mean that data is already being operated on the accumulator.
 */
u8 CPU6502::IMP()
{
	fetched = a;
	return 0;
} // end IMM


//=====================================================================|
/**
 * @brief The immidate mode addressing implies (aint that ironic) the 
 *	next byte in memory is the data being supplied as part of the instruction.
 */
u8 CPU6502::IMM()
{
	addr_abs = pc++;
	return 0;
} // end IMM


//=====================================================================|
/**
 * @brief Zero page addressing is a special addressing that puts the data 
 *	for the instruction somewhere in the first page of the RAM (the first 256 bytes).
 */
u8 CPU6502::ZP0()
{
	addr_abs = Read(pc++) & 0x00FF;
	return 0;
} // end ZP0


//=====================================================================|
/**
 * @brief Zero Page X is like Zero Page, only difference, the value at 
 *	the program couter is offset by the byte val store in the X register. 
 *	It's like array's in C.
 */
u8 CPU6502::ZPX()
{
	addr_abs = (Read(pc++) + x) & 0x00FF;
	return 0;
} // end ZPX


//=====================================================================|
/**
 * @brief ZPY is the same thing as ZPX but only the offset is the Y register.
 */
u8 CPU6502::ZPY()
{
	addr_abs = (Read(pc++) + y) & 0x00FF;
	return 0;
} // end ZPY


//=====================================================================|
/**
 * @brief This is absoulte addressing with the next word (16-bits) 
 *	containing an address to data for the instruction anywhere within the 
 *	allowed range of the 6502.
 */
u8 CPU6502::ABS()
{
	u16 lo = Read(pc++);
	u16 hi = Read(pc++);
	addr_abs = (hi << 8) | lo;
	return 0;
} // end ABS


//=====================================================================|
/**
 * @brief Like onto the ABS but with offset by the byte value at the X 
 *	register; however, since the addition may cause the system to flip 
 *	pages to the next page, we need to make sure to delay the clock cycle 
 *	accordingly
 */
u8 CPU6502::ABX()
{
	u16 lo = Read(pc++);
	u16 hi = Read(pc++);
	addr_abs = ((hi << 8) | lo) + x;

	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;		// more clock cycles since page changed

	return 0;
} // end ABX


//=====================================================================|
/**
 * @brief Same as ABX, and similar to ABS, with its only difference 
 *	being the offset now is at the Y register
 */
u8 CPU6502::ABY()
{
	u16 lo = Read(pc++);
	u16 hi = Read(pc++);
	addr_abs = ((hi << 8) | lo) + y;

	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;		// more clock cycles since page changed

	return 0;
} // end ABY


//=====================================================================|
/**
 * @brief These are effectively like pointers in C for the 6502.
 */
u8 CPU6502::IND()
{
	u16 ptr_lo = Read(pc++);
	u16 ptr_hi = Read(pc++);
	u16 ptr = (ptr_hi << 8) | ptr_lo;

	// 6502 bug: IND jumps never really go to the next page if the 
	// hibyte was to change the page during
	//	+1 operation, instead it wraps around the same page.
	if (ptr_lo == 0x00FF)
		addr_abs = ((Read(ptr & 0xFF00) << 8) | Read(ptr));
	else // is normal
		addr_abs = ((Read(ptr + 1) << 8) | Read(ptr));
	return 0;
} // end IND


//=====================================================================|
/**
 * @brief This is the most complex addressing mode in the 6502, the byte 
 *	at the current program counter references the zero page, this value 
 *	then when offset by the contents of the X register points to the 
 *	16-bit address that actually contains the data.
 */
u8 CPU6502::IZX()
{
	u16 t = Read(pc++);
	u16 lo = Read((t + x) & 0x00FF);
	u16 hi = Read((t + x + 1) & 0x00FF);
	addr_abs = (hi << 8) | lo;
	return 0;
} // end IZX


//=====================================================================|
/**
 * @brief Another complex addressing mode, like its counter part IZX the 
 *	pc points at zero page address but unlike IZX the data is reterived by 
 *	first reading the 16-bit address pointed by pc and offsetting that with
 *	the value stored in the Y register
 */
u8 CPU6502::IZY()
{
	u16 t = Read(pc++);
	u16 lo = Read(t + 0x00FF);
	u16 hi = Read((t + 1) & 0x00FF);
	addr_abs = ((hi << 8) | lo) + y;
	if ((addr_abs & 0xFF00) != (hi << 8))
		return 1;	// simulate page flipping
	return 0;
} // end IZY


//=====================================================================|
/**
 * @brief The relative addressing mode only occurs for branching 
 *	instructions, and it can only happen to a value that
 *	is within -/+127 bytes from the current address at pc.
 */
u8 CPU6502::REL()
{
	addr_rel = Read(pc++);
	if (addr_rel & 0x80)
		addr_rel |= 0xFF00;
	return 0;
} // end REL


//=====================================================================|
/**
 * @brief a little helper function that is used to fetch a data from 
 *	main memory, according to the addressing mode involved, this makes the 
 *	instructions/opcode excution uniform and easy to deal with at the expense of
 *	a little overhead.
 */
inline u8 CPU6502::Fetch()
{
	if (!(lookup[opcode].Addrmode == &CPU6502::IMP))
		fetched = Read(addr_abs);		// for all modes except implied
	return fetched;
} // end Fetch



//=====================================================================|
// OPCODES
//=====================================================================|
/**
 * @brief Add With Carry (the complications of 8-bit addition); 
 *	sets Z,V,N,C flags
 */
u8 CPU6502::ADC()
{
	u16 t = (u16)a + (u16)Fetch() + (u16)GET_FLAG(status, C);
	SET_FLAG(status, C, t > 255);
	SET_FLAG(status, Z, (t & 0x00FF) == 0x00);
	SET_FLAG(status, V, (~((u16)a ^ (u16)fetched) & ((u16)a ^ (u16)t)) & 0x0080);
	SET_FLAG(status, N, t & 0x80);
	a = t & 0x00FF;
	return 1;
} // end ADC


//=====================================================================|
/**
 * @brief Implements the logical AND operation on the accumulator 
 *	register, almost same as x86/x64: AND AL, m8 and sets the Zero and 
 *	Negative flags depending on the result of the operation
 */
u8 CPU6502::AND()
{
	a &= Fetch();
	SET_FLAG(status, Z, a == 0x00);
	SET_FLAG(status, N, a &= 0x80);
	return 1;
} // end AND


//=====================================================================|
/**
 * @brief Arithemtic Shift Left; sets Z, N, C flags
 */
u8 CPU6502::ASL()
{
	u16 t = (u16)Fetch() << 1;
	SET_FLAG(status, C, (t & 0xFF00) > 0);
	SET_FLAG(status, Z, (t & 0x00FF) == 0);
	SET_FLAG(status, N, t & 0x80);

	if (lookup[opcode].Addrmode == &CPU6502::IMP)
		a = t & 0x00FF;
	else
		Write(addr_abs, t & 0x00FF);
	return 0;
} // end ASL


//=====================================================================|
/**
 * @brief Branch if Carry Clear
 */
u8 CPU6502::BCC()
{
	if (!GET_FLAG(status, C))
	{
		++cycles;
		addr_abs = pc + addr_rel;
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;
		pc = addr_abs;
	} // end if

	return 0;
} // end BCC


//=====================================================================|
/**
 * @brief Branch if Carry Set
 */
u8 CPU6502::BCS()
{
	if (GET_FLAG(status, C))
	{
		++cycles;
		addr_abs = pc + addr_rel;
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;
		pc = addr_abs;
	} // end if

	return 0;
} // end BCS


//=====================================================================|
/**
 * @brief Branch if Equal; i.e. Z == 1
 */
u8 CPU6502::BEQ()
{
	if (GET_FLAG(status, Z))
	{
		++cycles;
		addr_abs = pc + addr_rel;
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;
		pc = addr_abs;
	} // end if

	return 0;
} // end BEQ


//=====================================================================|
/**
 * @brief sets N,Z,V flags based on and operation with accumulator register
 */
u8 CPU6502::BIT()
{
	u8 t = a & Fetch();
	SET_FLAG(status, Z, t == 0);
	SET_FLAG(status, N, fetched & 0x80);	// only 2 times I'll ever use 'fetched'
	SET_FLAG(status, V, fetched & 0x40);
	return 0;
} // end BIT


//=====================================================================|
/**
 * @brief Branch if Negative; i.e. N == 1
 */
u8 CPU6502::BMI()
{
	if (GET_FLAG(status, N))
	{
		++cycles;
		addr_abs = pc + addr_rel;
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;
		pc = addr_abs;
	} // end if

	return 0;
} // end BMI


//=====================================================================|
/**
 * @brief Branch if Not Equal; i.e. Z == 0
 */
u8 CPU6502::BNE()
{
	if (!GET_FLAG(status, Z))
	{
		++cycles;
		addr_abs = pc + addr_rel;
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;
		pc = addr_abs;
	} // end if

	return 0;
} // end BNE


//=====================================================================|
/**
 * @brief Branch if PLus (postive); i.e. N == 0
 */
u8 CPU6502::BPL()
{
	if (!GET_FLAG(status, N))
	{
		++cycles;
		addr_abs = pc + addr_rel;
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;
		pc = addr_abs;
	} // end if

	return 0;
} // end BPL


//=====================================================================|
/**
 * @brief Beak; program sourced interrupt, saves the current program 
 *	counter and status flag states; sets B,I flags
 */
u8 CPU6502::BRK()
{
	SET_FLAG(status, I, true);
	Write(0x0100 + sp--, (++pc >> 8) & 0x00FF); // HO
	Write(0x0100 + sp--, pc & 0x00FF);	// LO

	SET_FLAG(status, B, true);
	Write(0x0100 + sp--, status);
	SET_FLAG(status, B, false);

	pc = Read(0xFFFE) | ((u16)Read(0xFFFF) << 8);
	return 0;
} // end BRK


//=====================================================================|
/**
 * @brief Branch if Overflow Clear; i.e. V == 0
 */
u8 CPU6502::BVC()
{
	if (!GET_FLAG(status, V))
	{
		++cycles;
		addr_abs = pc + addr_rel;
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;
		pc = addr_abs;
	} // end if

	return 0;
} // end BVC


//=====================================================================|
/**
 * @brief Branch if Overflow Set; i.e. V == 1
 */
u8 CPU6502::BVS()
{
	if (GET_FLAG(status, V))
	{
		++cycles;
		addr_abs = pc + addr_rel;
		if ((addr_abs & 0xFF00) != (pc & 0xFF00))
			cycles++;
		pc = addr_abs;
	} // end if

	return 0;
} // end BVS


//=====================================================================|
/**
 * @brief Clear's the carry bit
 */
u8 CPU6502::CLC()
{
	SET_FLAG(status, C, false);
	return 0;
} // end CLC


//=====================================================================|
/**
 * @brief Clear's the interrupt flag
 */
u8 CPU6502::CLI()
{
	SET_FLAG(status, I, false);
	return 0;
} // end CLI


//=====================================================================|
/**
 * @brief Clear's the decimal flag, but if the decimal flag was not part of NES console, then why here?
 */
u8 CPU6502::CLD()
{
	SET_FLAG(status, D, false);
	return 0;
} // end CLD


//=====================================================================|
/**
 * @brief Clear's the overflow flag
 */
u8 CPU6502::CLV()
{
	SET_FLAG(status, V, false);
	return 0;
} // CLV


//=====================================================================|
/**
 * @brief CoMPare accumulator, sets Z,N,C flags
 */
u8 CPU6502::CMP()
{
	u16 t = (u16)a - (u16)Fetch();
	SET_FLAG(status, C, a >= fetched);
	SET_FLAG(status, Z, (t & 0x00FF) == 0);
	SET_FLAG(status, N, t & 0x0080);
	return 1;
} // end CMP


//=====================================================================|
/**
 * @brief CoMPare x register, sets Z,N,C flags
 */
u8 CPU6502::CPX()
{
	u16 t = (u16)x - (u16)Fetch();
	SET_FLAG(status, C, x >= fetched);
	SET_FLAG(status, Z, (t & 0x00FF) == 0);
	SET_FLAG(status, N, t & 0x0080);
	return 0;
} // end CPX


//=====================================================================|
/**
 * @brief CoMPare x register, sets Z,N,C flags
 */
u8 CPU6502::CPY()
{
	u16 t = (u16)y - (u16)Fetch();
	SET_FLAG(status, C, y >= fetched);
	SET_FLAG(status, Z, (t & 0x00FF) == 0);
	SET_FLAG(status, N, t & 0x0080);
	return 0;
} // end CPY


//=====================================================================|
/**
 * @brief Decrements the byte data at the memory location and set the 
 *	necessary flags for the operation.
 */
u8 CPU6502::DEC()
{
	u8 t = Fetch() - 1;
	Write(addr_abs, t);
	SET_FLAG(status, Z, t == 0x00);
	SET_FLAG(status, N, t & 0x80);
	return 0;
} // end Dec


//=====================================================================|
/**
 * @brief Decrements the X register and sets the Zero and Negative flags
 */
u8 CPU6502::DEX()
{
	--x;
	SET_FLAG(status, Z, x == 0);
	SET_FLAG(status, N, x & 0x80);
	return 0;
} // end DEX

//=====================================================================|
/**
 * @brief Same as DEX but for Y
 */
u8 CPU6502::DEY()
{
	--y;
	SET_FLAG(status, Z, y == 0);
	SET_FLAG(status, N, y & 0x80);
	return 0;
} // end DEY


//=====================================================================|
/**
 * @brief Performs exclusive or operation on the accumulator.
 */
u8 CPU6502::EOR()
{
	a ^= Fetch();
	SET_FLAG(status, Z, a == 0x00);
	SET_FLAG(status, N, a == 0x80);
	return 1;
} // end EOR


//=====================================================================|
/**
 * @brief Increments the data stored at memory location by 1 and 
 *	set's the valid flags
 */
u8 CPU6502::INC()
{
	u8 t = Fetch() + 1;
	Write(addr_abs, t);
	SET_FLAG(status, Z, t == 0x00);
	SET_FLAG(status, N, t & 0x80);
	return 0;
} // end INC


//=====================================================================|
/**
 * @brief Increments the X register by 1 and sets the Z, N flags
 */
u8 CPU6502::INX()
{
	++x;
	SET_FLAG(status, Z, x == 0x00);
	SET_FLAG(status, N, x & 0x80);
	return 0;
} // end INX


//=====================================================================|
/**
 * @brief Increments the Y register by 1 and sets Z, N flags
 */
u8 CPU6502::INY()
{
	++y;
	SET_FLAG(status, Z, y == 0x00);
	SET_FLAG(status, N, y & 0x80);
	return 0;
} // end INY


//=====================================================================|
/**
 * @brief Changes the current pc to the address provided; i.e. 
 *	implement jump instruction
 */
u8 CPU6502::JMP()
{
	pc = addr_abs;
	return 0;
} // end JMP


//=====================================================================|
/**
 * @brief Jump to Subroutine -- push the current pc to stack
 */
u8 CPU6502::JSR()
{
	--pc;
	Write(0x0100 + sp--, (pc >> 8) & 0x00FF);
	Write(0x0100 + sp--, pc & 0x00FF);

	pc = addr_abs;
	return 0;
} // end JSR


//=====================================================================|
/**
 * @brief Loads the accumulator from the value supplied at memory. 
 *	Sets Z,N flags.
 */
u8 CPU6502::LDA()
{
	a = Fetch();
	SET_FLAG(status, Z, a == 0x00);
	SET_FLAG(status, N, a & 0x80);
	return 1;
} // end LDA


//=====================================================================|
/**
 * @brief Loads the X register, sets N,Z flags
 */
u8 CPU6502::LDX()
{
	x = Fetch();
	SET_FLAG(status, Z, x == 0x00);
	SET_FLAG(status, N, x & 0x80);
	return 1;
} // end LDX


//=====================================================================|
/**
 * @brief Loads the Y register, sets N,Z flags
 */
u8 CPU6502::LDY()
{
	y = Fetch();
	SET_FLAG(status, Z, y == 0x00);
	SET_FLAG(status, N, y & 0x80);
	return 1;
} // end LDY


//=====================================================================|
/**
 * @brief Left shits operand by 1, affects C,Z,N flags.
 */
u8 CPU6502::LSR()
{
	SET_FLAG(status, C, Fetch() & 0x0001);
	u8 t = fetched >> 1;

	SET_FLAG(status, Z, t == 0);
	SET_FLAG(status, N, t & 0x0080);
	if (lookup[opcode].Addrmode == &CPU6502::IMP)
		a = t & 0x00FF;
	else
		Write(addr_abs, t);

	return 0;
} // end LSR


//=====================================================================|
/**
 * @brief NOP, no operation, do nothing; however some do nothings 
 *	according to specs at nesdev.wiki, can take more clock cycles depending 
 *	on the opcode.
 */
u8 CPU6502::NOP()
{
	switch (opcode)
	{
	case 0x1C:
	case 0x3C:
	case 0x5C:
	case 0x7C:
	case 0xDC:
	case 0xFC:
		return 1;
	} // end switch

	return 0;
} // end NOP


//=====================================================================|
/**
 * @brief Performs a logical OR operation on the accumulator register
 */
u8 CPU6502::ORA()
{
	a |= Fetch();
	SET_FLAG(status, Z, a == 0x00);
	SET_FLAG(status, N, a == 0x80);
	return 1;
} // end ORA


//=====================================================================|
/**
 * @brief Pushes the accumulator to the stack and updates the stack pointer
 */
u8 CPU6502::PHA()
{
	Write(0x0100 + sp, a);
	--sp;
	return 0;
} // end PHA


//=====================================================================|
/**
 * @brief Pushes the flags to the stack; clears B,U flags and updates 
 *	the stack pointer
 */
u8 CPU6502::PHP()
{
	Write(0x0100 + sp, status | B | U);
	SET_FLAG(status, B, false);
	SET_FLAG(status, U, false);
	--sp;
	return 0;
} // end PHP


//=====================================================================|
/**
 * @brief Pulls/pops the accumlator from the stack, updates the stack 
 *	pointer. Sets Z,N flags
 */
u8 CPU6502::PLA()
{
	a = Read(0x0100 + (++sp));
	SET_FLAG(status, Z, a == 0);
	SET_FLAG(status, N, a & 0x80);
	return 0;
} // end PLA


//=====================================================================|
/**
 * @brief Pops the flags register from the stack, sets U flag (don know why).
 */
u8 CPU6502::PLP()
{
	status = Read(0x0100 + (++sp));
	SET_FLAG(status, U, true);
	return 0;
} // end PLP


//=====================================================================|
/**
 * @brief Rotate Left
 */
u8 CPU6502::ROL()
{
	u16 t = (u16)((Fetch() << 1) | GET_FLAG(status, C));
	SET_FLAG(status, C, t & 0xFF00);
	SET_FLAG(status, Z, (t & 0x00FF) == 0x0000);
	SET_FLAG(status, N, t & 0x0080);
	if (lookup[opcode].Addrmode == &CPU6502::IMP)
		a = t & 0x00FF;
	else
		Write(addr_abs, t & 0x00FF);
	return 0;
} // end ROL


//=====================================================================|
/**
 * @brief Rotate right
 */
u8 CPU6502::ROR()
{
	u16 temp = (uint16_t)(GET_FLAG(status, C) << 7) | (Fetch() >> 1);
	SET_FLAG(status, C, fetched & 0x01);
	SET_FLAG(status, Z, (temp & 0x00FF) == 0x00);
	SET_FLAG(status, N, temp & 0x0080);
	if (lookup[opcode].Addrmode == &CPU6502::IMP)
		a = temp & 0x00FF;
	else
		Write(addr_abs, temp & 0x00FF);
	return 0;
} // end ROR


//=====================================================================|
/**
 * @brief Return from interrupt
 */
u8 CPU6502::RTI()
{
	status = Read(0x0100 + sp);
	status &= ~B;
	status &= ~U;

	pc = (uint16_t)Read(0x0100 + sp++);
	pc |= (uint16_t)Read(0x0100 + sp++) << 8;
	return 0;
} // end RTI


//=====================================================================|
/**
 * @brief Return from subroutine
 */
u8 CPU6502::RTS()
{
	pc = (uint16_t)Read(0x0100 + sp++);
	pc |= (uint16_t)Read(0x0100 + sp++) << 8;

	pc++;
	return 0;
} // end RTS


//=====================================================================|
/**
 * @brief Subtract with carray or should I say Borrow? sets Z,V,N,C flags
 */
u8 CPU6502::SBC()
{
	u16 t = (u16)a + ((u16)Fetch() ^ 0x00FF) + (u16)GET_FLAG(status, C);

	SET_FLAG(status, C, t > 255);
	SET_FLAG(status, Z, (t & 0x00FF) == 0x00);
	SET_FLAG(status, V, (~((u16)a ^ (u16)fetched) & ((u16)a ^ (u16)t)) & 0x0080);
	SET_FLAG(status, N, t & 0x80);
	a = t & 0x00FF;
	return 1;
} // end SBC


//=====================================================================|
/**
 * @brief Sets the carry flag to on/1
 */
u8 CPU6502::SEC()
{
	SET_FLAG(status, C, true);
	return 0;
} // end SEC


//=====================================================================|
/**
 * @brief Set's the decimal flag
 */
u8 CPU6502::SED()
{
	SET_FLAG(status, D, true);
	return 0;
} // end SED


//=====================================================================|
/**
 * @brief Sets the Interrupt flag
 */
u8 CPU6502::SEI()
{
	SET_FLAG(status, I, true);
	return 0;
} // end SEI


//=====================================================================|
/**
 * @brief Store accumulator at address
 */
u8 CPU6502::STA()
{
	Write(addr_abs, a);
	return 0;
} // end STA

//=====================================================================|
/**
 * @brief Store X register at address
 */
u8 CPU6502::STX()
{
	Write(addr_abs, x);
	return 0;
} // end STX


//=====================================================================|
/**
 * @brief Store register Y at address
 */
u8 CPU6502::STY()
{
	Write(addr_abs, y);
	return 0;
} // end STY


//=====================================================================|
/**
 * @brief Transfers the accumulator to X register; sets N,Z
 */
u8 CPU6502::TAX()
{
	x = a;
	SET_FLAG(status, Z, x == 0);
	SET_FLAG(status, N, x & 0x80);
	return 0;
} // end TAX


//=====================================================================|
/**
 * @brief Transfers accumulator to Y register, sets Z,V
 */
u8 CPU6502::TAY()
{
	y = a;
	SET_FLAG(status, Z, y == 0);
	SET_FLAG(status, N, y & 0x80);
	return 0;
} // end TAY


//=====================================================================|
/**
 * @brief Moves the stack pointer to X register; sets Z and N flags
 */
u8 CPU6502::TSX()
{
	x = sp;
	SET_FLAG(status, Z, x == 0);
	SET_FLAG(status, N, x & 0x80);
	return 0;
} // end TSX


//=====================================================================|
/**
 * @brief Transferes X register to accumulator, sets Z,N flags
 */
u8 CPU6502::TXA()
{
	a = x;
	SET_FLAG(status, Z, a == 0);
	SET_FLAG(status, N, a & 0x80);
	return 0;
} // end TXA


//=====================================================================|
/**
 * @brief Transfers X register to stack pointer
 */
u8 CPU6502::TXS()
{
	sp = x;
	return 0;
} // end sp


//=====================================================================|
/**
 * @brief Transfer Y register to accumulator, sets Z,N flags
 */
u8 CPU6502::TYA()
{
	a = y;
	SET_FLAG(status, Z, a == 0);
	SET_FLAG(status, N, a & 0x80);
	return 0;
} // end TYA


//=====================================================================|
/**
 * @brief The Unkown opcode
 */
u8 CPU6502::UNK()
{
	return 0;
} // end UNK