#if _MSC_VER > 1000
#pragma once
#endif

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

public:
	bool isRunning = true;
	bool drawFlag = false;

	void initCpu();
	void initialize();
	int  loadGame(const char* name) const;
	bool emulateCycle();
	void stopEmulation();
};
#endif