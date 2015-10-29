#include <fstream>

#include "chip8-cpu.h"
#include "chip8-memory.h"


void chip8::initCpu()
{
	std::fill_n(stack, 16, 0);
	opcode,
	I,
	pc,
	sp = 0;
}

void chip8::initialize()
{
	initMem();
	initCpu();

}

int chip8::loadGame(const char* name) const
{
	std::ifstream game(name, std::ios::in | std::ios::binary | std::ios::ate);
	std::streamsize size = game.tellg();
	game.seekg(0, std::ios::beg);

	game.read(reinterpret_cast<char*>(mem::memory) + 512, size);

	return game.gcount();
}

void chip8::emulateCycle()
{

}
