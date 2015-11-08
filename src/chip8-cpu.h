#if _MSC_VER > 1000
#pragma once
#endif

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#ifndef CPU_H
#define CPU_H

class chip8
{
private:
	unsigned short
		stack[16],	//16-level Stack
		sp,			//Stack pointer
		opcode,		//Current opcode
		I,			//Index register
		pc;			//Program counter


	unsigned char
		delay_timer,   	//These 2 registers when set above zero,
						//they will count down to it at 60Hz
		sound_timer;	//When the sound timer reaches zero, the buzzer sounds
						//TODO: Implement sound
	char buf[256];
	std::ostringstream opcode_ss;

	sf::SoundBuffer sound_buffer;
	sf::Sound beep;

	bool decodeOpcode(unsigned short opcode);

public:
	bool isRunning = true;
	bool drawFlag = false;
	bool waitForKey = false;

	void initCpu();
	int initialize();
	static int  loadGame(const char* name);
	void keyPress(const unsigned char k);
	void keyRelease(const unsigned char k);
	bool emulateCycle(short cycles = 1);
	void detInfLoop();
	void stopEmulation();

};
#endif