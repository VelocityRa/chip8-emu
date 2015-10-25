#include "memory.h"

/*
Chip 8's memory map:

0x000 - 0x1FF - Chip 8 interpreter(contains font set in emu)
0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
0x200 - 0xFFF - Program ROM and work RAM
*/

//Go to the header for details on the variables here
namespace mem
{
	//Clean all the memory
	void initialize()
	{
		auto
		opcode = new unsigned short();

		unsigned char*
		memory[4096] = { 0 };

		unsigned char*
		V[16] = { 0 };

		auto
		I = new unsigned short();

		auto
		pc = new unsigned short();

		unsigned char*
		pixels[64 * 32] = { 0 };

		auto
		delay_timer = new unsigned char();
		auto
		sound_timer = new unsigned char();

		unsigned short*
		stack[16] = { 0 };
		auto
		sp = new unsigned short();

		unsigned char* key[16] = { 0 };

	}
} //namespace mem