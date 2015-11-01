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
	unsigned char
		memory[4096],
		V[16],
		key[16],
		pixels[64 * 32],
		chip8_fontset[80];

}

//Initialize everything
void initMem() {

	// Use fill_n instead of array[x] = { 0 }
	// to prevent buffer overrun
	std::fill_n(mem::memory, 4096, 0);
	std::fill_n(mem::V, 16, 0);
	std::fill_n(mem::pixels, 64 * 32, 0);
	std::fill_n(mem::key, 16, 0);

	// Load fontset
	for (int i = 0; i < 80; ++i)
		mem::memory[i] = mem::chip8_fontset[i];
}
