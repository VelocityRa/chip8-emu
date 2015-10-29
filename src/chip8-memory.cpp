#include "chip8-memory.h"

/*
Chip 8's memory map:

0x000 - 0x1FF - Chip 8 interpreter(contains font set in emu)
0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
0x200 - 0xFFF - Program ROM and work RAM
*/

//Go to the header for details on the variables here


unsigned short	opcode;
unsigned char	memory[4096];
unsigned char	V[16];
unsigned short	I;
unsigned short	pc;
unsigned char	pixels[64 * 32];
unsigned char	delay_timer;
unsigned char	sound_timer;
unsigned short	stack[16];
unsigned short	sp;
unsigned char	key[16];

//Initialize everything
void initMem() {

	opcode = *new unsigned short();

	memory[4096] = { 0 };

	V[16] = { 0 };

	I = *new unsigned short();
	pc = *new unsigned short();

	pixels[64 * 32] = { 0 };

	delay_timer = *new unsigned char();
	sound_timer = *new unsigned char();

	stack[16] = { 0 };
	sp = *new unsigned short();

	key[16] = { 0 };
}