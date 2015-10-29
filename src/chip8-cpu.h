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

public:
	bool drawFlag = false;

	void initCpu();
	void initialize();
	int  loadGame(const char* name) const;
	void emulateCycle();
};
#endif