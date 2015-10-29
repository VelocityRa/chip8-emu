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

	//These 2 registers when set above zero,
	//they will count down to it at 60Hz
	extern unsigned char	delay_timer;

	//When the sound timer reaches zero, the buzzer sounds
	//TODO: Implement sound
	extern unsigned char	sound_timer;

	//State of the keypad
	extern unsigned char	key[16];
}
#endif
