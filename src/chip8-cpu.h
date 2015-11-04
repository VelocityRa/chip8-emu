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
	char buf[256];
	std::ostringstream opcode_ss;

	bool decodeOpcode(unsigned short opcode);

public:
	bool isRunning = true;
	bool drawFlag = false;
	bool waitForKey = false;

	void initCpu();
	void initialize();
	int  loadGame(const char* name) const;
	void keyPress(const unsigned char k);
	bool emulateCycle();
	void detInfLoop();
	void stopEmulation();
};
#endif