#include <fstream>
#include <sstream>
#include <iomanip>

#include "chip8-cpu.h"
#include "chip8-memory.h"
#include "sfTextTools.h"

void chip8::initCpu()
{
	std::fill_n(stack, 16, 0);
	opcode = 0;
	I = 0;
	pc = 0x200; // Starting adress where the game is loaded
	sp = 0;
	delay_timer = 0;
	sound_timer = 0;

	std::fill_n(buf, 256, 0);
	opcode_ss << std::hex << std::setfill('0') << std::uppercase;
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

void chip8::keyPress(unsigned char k)
{
	if (waitForKey) {
		waitForKey = false;
		isRunning = true;

		mem::V[(opcode & 0x0F00) >> 8] = k;
		mem::key[k] = true;

		pc += 2;
	}
	else
	{
		mem::key[k] = true;
	}
}

void chip8::keyRelease(const unsigned char k)
{
	mem::key[k] = false;
}

// If this returns false, we need to stop the emulation
bool chip8::emulateCycle()
{
	//Fetch opcode
	opcode = mem::memory[pc] << 8 |
		mem::memory[pc + 1];

	//Decode opcode
	//If decodeOpcode returns false, return false
	if (decodeOpcode(opcode)) {} else return false;

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

void chip8::detInfLoop()
{
	appendText(&debugText, "Infinite loop detected, game stopped.");
	stopEmulation();
}

bool chip8::decodeOpcode(unsigned short opcode)
{
	using namespace mem;	// We're gonna be using fields from mem:: a lot

	std::fill_n(buf, 256, 0);

	switch (opcode & 0xF000)
	{
	case 0x0000:
		switch (opcode & 0x0FFF)
		{
		case 0x00E0: // Clear screen
			sprintf_s(buf, 256, "Clear screen");
			std::fill_n(pixels, 64 * 32, 0);
			pc += 2; break;
		case 0x00EE: // Return from a subroutine
			pc = stack[--sp];
			pc += 2;
			sprintf_s(buf, 256, "RET from subroutine before %03X, sp:%d", pc, sp);
			break;
		default:
			goto uknown;
		}
		break;
	case 0x1000: // (1NNN) Jumps to address NNN
		pc = opcode & 0x0FFF;
		sprintf_s(buf, 256, "Jump to %03X", pc);
		if ((0x1000 | pc) == (memory[pc] << 8 | memory[(pc + 1) % 0x1000]) )
		{
			detInfLoop();	// Infinite loop detected (game stopped execution)
			return false;
		}
		break;
	case 0x2000: // (2NNN) Calls subroutine at NNN
		stack[sp++] = pc;
		pc = opcode & 0x0FFF;
		sprintf_s(buf, 256, "CALL subroutine %03X, sp:%d", pc, sp-1);
		break;
	case 0x3000: // (3XNN) Skips the next instruction if VX equals NN
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
		{
			sprintf_s(buf, 256, "V%X == %02X, so skip", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
			pc += 2;
		} else { sprintf_s(buf, 256, "V%X != %02X, so don't skip", (opcode & 0x0F00) >> 8, (opcode & 0x00FF)); }
		pc += 2; break;
	case 0x4000: // (4XNN) Skips the next instruction if VX doesn't equal NN
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
		{
			sprintf_s(buf, 256, "V%X != %02X, so skip", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
			pc += 2;
		} else { sprintf_s(buf, 256, "V%X == %02X, so don't skip", (opcode & 0x0F00) >> 8, (opcode & 0x00FF)); }
		pc += 2; break;
	case 0x5000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // (5XN0) Skips the next instruction if VX equals VY
			if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
			{
				sprintf_s(buf, 256, "V%X == V%X, so skip", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
				pc += 2;
			} else { sprintf_s(buf, 256, "V%X != V%X, so don't skip", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4); }
			pc += 2; break;
		default:
			goto uknown;
		}
		break;
	case 0x6000: // (6XNN) Sets VX to NN
		sprintf_s(buf, 256, "V%X = %02X", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
		V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
		pc += 2; break;
	case 0x7000: // (7XNN) Adds NN to VX.
		sprintf_s(buf, 256, "V%X += %02X", (opcode & 0x0F00) >> 8, (opcode & 0x00FF));
		V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
		pc += 2; break;
	case 0x8000:
		switch (opcode & 0x000F)
		{
		case 0x0000: // (8XY0) Sets VX to the value of VY.
			sprintf_s(buf, 256, "V%X = V%X", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			pc += 2; break;
		case 0x0001: // (8XY1) Sets VX to VX or VY.
			sprintf_s(buf, 256, "V%X |= V%X", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			pc += 2; break;
		case 0x0002: // (8XY2) Sets VX to VX and VY.
			sprintf_s(buf, 256, "V%X &= V%X", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			pc += 2; break;
		case 0x0003: // (8XY3) Sets VX to VX xor VY.
			sprintf_s(buf, 256, "V%X ^= V%X", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			pc += 2; break;
		case 0x0004: // (8XY4) Adds VY to VX. VF is set to 1 when there's a carry,
					 // and to 0 when there isn't.
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 1; //carry
			else { V[0xF] = 0; }
			sprintf_s(buf, 256, "V%X += V%X, carry=%d", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4, V[0xF]);
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			pc += 2; break;
		case 0x0005: // (8XY5) VY is subtracted from VX. VF is set to 0 when there's a borrow,
					 // and to 1 when there isn't
			if (V[(opcode & 0x00F0) >> 4] > (V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 0; //borrow
			else { V[0xF] = 1;}
			sprintf_s(buf, 256, "V%X -= V%X, carry=%d", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4, V[0xF]);
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			pc += 2; break;
		case 0x0006: // (8XY6) Shifts VX right by one. 
					 // VF is set to the value of the least significant bit of VX before the shift
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 1;
			V[(opcode & 0x0F00) >> 8] >>= 1;
			sprintf_s(buf, 256, "V%X >>= 1, VF=%X", (opcode & 0x0F00) >> 8, V[0xF]);
			pc += 2; break;
		case 0x0007: // (8XY7) Sets VX to VY minus VX. VF is set to 0 when there's a borrow,
					 // and 1 when there isn't
			if (V[(opcode & 0x00F0) >> 4] < (V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 0; //borrow
			else { V[0xF] = 1; }
			sprintf_s(buf, 256, "V%X -= V%X, carry=%d", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4, V[0xF]);
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			pc += 2; break;
		case 0x000E: // (8XYE) Shifts VX left by one. 
					 // VF is set to the value of the most significant bit of VX before the shift
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 128; // 128 comes from this:
													  // (1 << (sizeof(unsigned char) * 8 - 1));
			sprintf_s(buf, 256, "V%X <<= 1, VF=%X", (opcode & 0x0F00) >> 8, V[0xF]);
			V[(opcode & 0x0F00) >> 8] <<= 1;
			pc += 2; break;
		default:
			goto uknown;
		}
		break;
	case 0x9000: // (9XY0) Skips the next instruction if VX doesn't equal VY.
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
		{
			sprintf_s(buf, 256, "V%X != VF=%X, so skip", (opcode & 0x0F00) >> 8, (opcode & 0x00F0) >> 4);
			pc += 2;
		} else { sprintf_s(buf, 256, "V%X == V%X=%X, so don't skip",(opcode & 0x0F00) >> 8,
											 (opcode & 0x00F0) >> 4, (opcode & 0x00F0) >> 4); }
		pc += 2; break;
	case 0xA000: // (ANNN) Sets I to the address NNN
		I = opcode & 0x0FFF;
		sprintf_s(buf, 256, "I = %03X", I);
		pc += 2; break;
	case 0xB000: // (BNNN) Jumps to the address NNN plus V0.
		pc = (opcode & 0x0FFF) + V[0x0];
		sprintf_s(buf, 256, "Jump to %03X + V0 = %04X", (opcode & 0x0FFF), (opcode & 0x0FFF) + V[0x0]);
		break;
	case 0xC000: // (CXNN) Sets VX to the result of a bitwise and operation
				 // on a random number and NN.
		V[(opcode & 0x0F00) >> 8] = (rand() & 0x00FF) & (opcode & 0x00FF);
		sprintf_s(buf, 256, "Randomizing V%X", (opcode & 0x0F00) >> 8);
		pc += 2; break;
	case 0xD000: // (DXYN) Draws a sprite at coordinate (VX, VY) 
	{			 // that has a width of 8 pixels and a height of N pixels.
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		sprintf_s(buf, 256, "Drawing in X:%d, Y:%d, height:%d", x, y, height);

		V[0xF] = 0;
		for (auto yline = 0; yline < height; yline++)
		{
			pixel = memory[I + yline];
			for (auto xline = 0; xline < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0)
				{
					if (pixels[(x + xline + ((y + yline) * 64))] == 1)
						V[0xF] = 1;
					pixels[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}
		drawFlag = true;
		pc += 2; break;
	}
	case 0xE000:
		switch (opcode & 0x00FF)
		{
		case 0x009E: // (EX9E) Skips the next instruction if the key stored in VX is pressed.
			if (key[V[(opcode & 0x0F00) >> 8]])
			{
				sprintf_s(buf, 256, "Key in V%X is pressed, so skip", (opcode & 0x0F00) >> 8);
				key[V[(opcode & 0x0F00) >> 8]] = false;
				pc += 2;
			} else { sprintf_s(buf, 256, "Key in V%X is not pressed, so don't skip", (opcode & 0x0F00) >> 8); }
			pc += 2; break;
		case 0x00A1: // (EX9E) Skips the next instruction if the key stored in VX isn't pressed.
			if (!key[V[(opcode & 0x0F00) >> 8]])
			{
				sprintf_s(buf, 256, "Key in V%X is not pressed, so skip", (opcode & 0x0F00) >> 8);
				pc += 2;
			} else { sprintf_s(buf, 256, "Key in V%X is pressed, so don't skip", (opcode & 0x0F00) >> 8); }
			pc += 2; break;
		default:
			goto uknown;
		}
		break;
	case 0xF000:
		switch (opcode & 0x00FF)
		{
		case 0x0007: // (FX07) Sets VX to the value of the delay timer.
			V[(opcode & 0x0F00) >> 8] = delay_timer;
			sprintf_s(buf, 256, "V%X = delay_timer = %d", (opcode & 0x0F00) >> 8, delay_timer);
			pc += 2; goto ret;
		case 0x000A: // TODO: (FX0A) A key press is awaited, and then stored in VX.
			sprintf_s(buf, 256, "Waiting for key to be stored in V%X", (opcode & 0x0F00) >> 8);
			waitForKey = true;
			isRunning = false;
			goto ret;
		case 0x0015: // (FX15) Sets the delay timer to VX.
			sprintf_s(buf, 256, "delay_timer = V%X = %02X", delay_timer, (opcode & 0x0F00) >> 8), \
															V[(opcode & 0x0F00) >> 8];
			delay_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2; goto ret;
		case 0x0018: // (FX18) Sets the sound timer to VX.
			sprintf_s(buf, 256, "sound_timer = V%X = %02X", sound_timer, (opcode & 0x0F00) >> 8), \
															V[(opcode & 0x0F00) >> 8];
			sound_timer = V[(opcode & 0x0F00) >> 8];
			pc += 2; goto ret;
		case 0x001E: // (FX1E) Adds VX to I. VF is set to 1 when there's a carry,
					 // and to 0 when there isn't
			if (V[(opcode & 0x0F00) >> 8] > (0xFF - I))
			{
				V[0xF] = 1; //carry
			}
			else
			{
				V[0xF] = 0;
			}
			I += V[(opcode & 0x0F00) >> 8];
			sprintf_s(buf, 256, "I += V%X, carry=%d", (opcode & 0x0F00) >> 8, V[0xF]);
			pc += 2; goto ret;
		case 0x0029: // (FX29) Sets I to the location of the sprite for the character in VX
					 // Characters 0 - F(in hexadecimal) are represented by a 4x5 font.
			I = V[(opcode & 0x0F00) >> 8] * 5;
			sprintf_s(buf, 256, "I = %03X (loc of sprite for char %X)", I, (opcode & 0x0F00) >> 8);
			pc += 2; goto ret;
		case 0x0033: // (FX33) Stores the binary-coded decimal representation of
		{			 // VX at the addresses I, I plus 1, and I plus 2
			unsigned char X = V[(opcode & 0x0F00) >> 8];
			memory[I] = X / 100;
			memory[I + 1] = (X / 10) % 10;
			memory[I + 2] = (X % 100) % 10;
			sprintf_s(buf, 256, "I = BCD(V%X)", (opcode & 0x0F00) >> 8);
			pc += 2; goto ret;
		}
		case 0x0055: // (FX55) Stores V0 to VX in memory starting at address I
		{
			unsigned char X = (opcode & 0x0F00) >> 8;
			for (auto i = 0; i <= X; i++)
			{
				memory[I + i] = V[i];
			}
			sprintf_s(buf, 256, "Store V0 to V%X starting at I=%03X", (opcode & 0x0F00) >> 8, I);
			pc += 2; goto ret;
		}
		case 0x0065: // (FX65) Fills V0 to VX with values from memory starting at address I
		{
			unsigned char X = V[(opcode & 0x0F00) >> 8];
			for (auto i = 0; i <= X; i++)
			{
				V[i] = memory[I + i];
			}
			sprintf_s(buf, 256, "Fill V0 to V%X with values from I=%03X", (opcode & 0x0F00) >> 8, I);
			pc += 2; goto ret;
		}
		default:
			goto uknown;
		}
	default:
		uknown:
		opcode_ss.str("");
		opcode_ss << "Unknown opcode: 0x" << std::setw(4) << opcode;

		appendText(&debugText, &opcode_ss);
		return false; // We can't handle this opcode, so stop the emulation
	}
	ret: //The opcode is known, so exit the function normally
	opcode_ss.str("");
	opcode_ss << '(' << std::setw(4) << opcode << "): " << buf;
	appendText(&debugText, &opcode_ss);
	return true;
}

void chip8::stopEmulation()
{
	isRunning = false;
	appendText(&debugText, "Emulation stopped");
}
