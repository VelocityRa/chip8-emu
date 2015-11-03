#if _MSC_VER > 1000
#pragma once
#endif

#ifndef MEMORY_H
#define MEMORY_H

extern void initMem();

namespace mem
{
	//4K of RAM
	extern unsigned char	memory[4096];

	//15 registers + 1 carry flag
	extern unsigned char	V[16];

	//Pixel state
	extern unsigned char	pixels[64 * 32];

	//State of the keypad
	extern  bool			key[16];

	//Fontset
	extern unsigned char	chip8_fontset[80];
}
#endif
