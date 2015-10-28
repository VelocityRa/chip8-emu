#include "chip8-memory.h"

/*
Chip 8's memory map:

0x000 - 0x1FF - Chip 8 interpreter(contains font set in emu)
0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
0x200 - 0xFFF - Program ROM and work RAM
*/

//Go to the header for details on the variables here

//Initialize everything

unsigned short
	opcode = *new unsigned short();

unsigned char
	memory[4096] = {0};

unsigned char
	V[16] = {0};

unsigned short
	I = *new unsigned short();
unsigned short
	pc = *new unsigned short();

unsigned char
	pixels[64 * 32] = {0};

unsigned char
	delay_timer = *new unsigned char();
unsigned char
	sound_timer = *new unsigned char();

unsigned short
	stack[16] = {0};
unsigned short
	sp = *new unsigned short();

unsigned char
	key[16] = {0};