#if _MSC_VER > 1000
#pragma once
#endif

#ifndef CPU_H
#define CPU_H

class chip8
{
private:

public:
	void initialize() const;
	int loadGame(const char* name) const;
};
#endif