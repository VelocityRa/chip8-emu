#include <fstream>

#include "chip8-cpu.h"
#include "chip8-memory.h"


void chip8::initialize() const
{
}

int chip8::loadGame(const char* name) const
{
	std::ifstream game(name, std::ios::in | std::ios::binary | std::ios::ate);
	std::streamsize size = game.tellg();
	game.seekg(0, std::ios::beg);

	game.read(reinterpret_cast<char*>(memory) + 512, size);

	return game.gcount();
}
