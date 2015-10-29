#include "chip8-memory.h"
#include <algorithm>

/*
Chip 8's memory map:

0x000 - 0x1FF - Chip 8 interpreter(contains font set in emu)
0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
0x200 - 0xFFF - Program ROM and work RAM
*/

//Go to the header for details on the variables here

namespace mem
{

	unsigned char	memory[4096],
					V[16],
					key[16],
					pixels[64 * 32],
					delay_timer,
					sound_timer;

	unsigned short	stack[16],
					sp,
					opcode,
					I,
					pc;


	//Initialize everything
	void initMem() {

		// Use fill_n instead of array[x] = { 0 }
		// to prevent buffer overrun
		std::fill_n(memory, 4096, 0);
		std::fill_n(V, 16, 0);
		std::fill_n(pixels, 64 * 32, 0);
		std::fill_n(stack, 16, 0);
		std::fill_n(key, 16, 0);

		opcode,
		I,
		pc,
		delay_timer,
		sound_timer,
		sp = 0;
	}

}