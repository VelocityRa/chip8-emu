#include <fstream>
#include <sstream>
#include <iomanip>

#include "chip8-cpu.h"
#include "chip8-memory.h"
#include "sfTextTools.h"


void chip8::initCpu()
{
	std::fill_n(stack, 16, 0);
	opcode,
		I,
		pc = 0x200;
		sp,
		delay_timer,
		sound_timer	= 0;

}

void chip8::initialize()
{
	initCpu();
	initMem();
}

int chip8::loadGame(const char* name) const
{
	std::ifstream game(name, std::ios::in | std::ios::binary | std::ios::ate);
	std::streamsize size = game.tellg();
	game.seekg(0, std::ios::beg);

	//Fill the memory with game data at location: 0x200 == 512
	game.read(reinterpret_cast<char*>(mem::memory) + 0x200, size);

	return int(game.gcount());
}

// If this returns false, we need to stop the emulation
bool chip8::emulateCycle()
{
	//Fetch opcode
	opcode = mem::memory[pc] << 8 | 
			 mem::memory[pc+1];

	//Decode opcode
	switch (opcode & 0xF000)
	{

	case 0xA000: //Axxx: Sets I to the address xxx
		I = opcode & 0x0FFF;
		pc += 2;
		break;
		



	default:
		std::ostringstream opcode_ss;
		opcode_ss << std::hex << std::setfill('0') << std::uppercase <<
			"Unknown opcode: 0x" <<  std::setw(4) << opcode;

		appendText(&debugText, &opcode_ss);
		return false;	// We can't handle this opcode, so stop the emulation
	}

	// Update timers
	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0)
	{
		if (sound_timer == 1)
			appendText(&debugText, "BEEP!");
		--sound_timer;
	}
	return true;
}

void chip8::stopEmulation()
{
	isRunning = false;
	appendText(&debugText, "Emulation stopped");
}