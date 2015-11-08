#include <SFML/Graphics.hpp>
#include <sstream>
#include <string>
#include <iomanip>
#include <iostream>

#include "chip8-cpu.h"
#include "chip8-memory.h"
#include "sfTextTools.h"


//Resolution multiplier
#define RES_MULT 12
//Padding pixels for debug strings
#define PAD 3

#define WIDTH_PIXELS 64
#define HEIGHT_PIXELS 32
#define height HEIGHT_PIXELS * RES_MULT
#define width WIDTH_PIXELS * RES_MULT

#define FG_COLOR 255, 255, 255
#define BG_COLOR 5, 15, 55


/*
#ifdef _DEBUG
auto isDebug = true;
#else
auto isDebug = false;
#endif
*/
auto isDebug = true;

void appendText(sf::Text* text, std::string st);
void replaceText(sf::Text* text, std::string st);

static void updRegText(std::ostringstream* ss, sf::Text* regText);

chip8 myChip8;
sf::Text debugText;

int main(int argc, char* argv[])
{
	std::string game_path;
	if (argc > 1)
	{
		game_path = argv[1];
	}
	else
	{
		//No path given
		return 1;
	}

	//Setup window creation
	sf::ContextSettings settings;
	settings.antialiasingLevel = 0;

	//Create window
	sf::RenderWindow window(sf::VideoMode(width, height), "Chip-8 Emulator",
	                        sf::Style::Titlebar | sf::Style::Close,
	                        settings);
	window.setFramerateLimit(60);

	//Load a pixely font
	sf::Font mc_font;

	if (!mc_font.loadFromFile("C:/Users/Nikos/Documents/Visual Studio 2015/Projects/chip8-emu/resources/fonts/Minecraftia-Regular.ttf"))
	{
		//Couldn't load font
		window.close();
		return 1;
	}

	sf::Clock Clock;

	sf::Text regText;
	sf::Text fpsText;
	unsigned short Framerate;

	if (isDebug)
	{
		debugText.setFont(mc_font);
		debugText.setCharacterSize(12);
		debugText.setPosition(0 + PAD, 12 * 3 + 6 + PAD);
		debugText.setColor(sf::Color(100, 100, 100));

		regText.setFont(mc_font);
		regText.setCharacterSize(8);
		regText.setPosition(width - 6 * 5 - PAD, 40 + PAD);
		regText.setColor(sf::Color::Red);

		fpsText.setFont(mc_font);
		fpsText.setCharacterSize(12);
		fpsText.setPosition(width - 6 * 3 - PAD, 8 + PAD);
		fpsText.setColor(sf::Color(sf::Color::Cyan));
	}

	srand(time(nullptr)); // use current time as seed for random generator

	myChip8.initialize();
	appendText(&debugText, "Initializing chip8");

	auto load_result = myChip8.loadGame(game_path.c_str());
	appendText(&debugText, "Loaded  " + std::to_string(load_result) + "  bytes to memory");

	std::vector<sf::RectangleShape> screen(64 * 32);
	
	//Create the stream/string for registry display in Debug mode
	std::ostringstream regSStream;
	regSStream << std::hex << std::setfill('0') << std::uppercase;

	for (size_t i = 0; i < 64 * 32; i++)
	{
		screen[i].setPosition(i % 64 * RES_MULT, i / 64 * RES_MULT);;
		screen[i].setSize(sf::Vector2f(RES_MULT, RES_MULT));
		screen[i].setFillColor(sf::Color(BG_COLOR));
	}

	//Signifies waiting for input
	sf::RectangleShape input_rec(sf::Vector2f(32, 7));
	input_rec.setPosition(width - 34, 21);
	input_rec.setFillColor(sf::Color::Yellow);

	//Signifies that the draw flag is set
	sf::RectangleShape draw_rec(sf::Vector2f(32, 7));
	draw_rec.setPosition(width - 34, 28);
	draw_rec.setFillColor(sf::Color::Green);

	myChip8.isRunning = false;

	//Main Loop
	while (window.isOpen())
	{

	//Event handling
	sf::Event event;
	while (window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			window.close();
			break;

		case sf::Event::KeyPressed:
			switch (event.key.code)
			{
			case sf::Keyboard::F1:
				myChip8.isRunning = !myChip8.isRunning;
				break;
			case sf::Keyboard::F2:
				myChip8.isRunning = false;
				myChip8.emulateCycle();
				break;
			case sf::Keyboard::F3:
				isDebug = !isDebug;
				break;

			case sf::Keyboard::Tab:
			{
				window.setFramerateLimit(0);
				break;
			}
			default:
				case sf::Keyboard::Num1: myChip8.keyPress(1); break;
				case sf::Keyboard::Num2: myChip8.keyPress(2); break;
				case sf::Keyboard::Num3: myChip8.keyPress(3); break;
				case sf::Keyboard::Num4: myChip8.keyPress(0xC); break;

				case sf::Keyboard::Q: myChip8.keyPress(4); break;
				case sf::Keyboard::W: myChip8.keyPress(5); break;
				case sf::Keyboard::E: myChip8.keyPress(6); break;
				case sf::Keyboard::R: myChip8.keyPress(0xD); break;

				case sf::Keyboard::A: myChip8.keyPress(7); break;
				case sf::Keyboard::S: myChip8.keyPress(8); break;
				case sf::Keyboard::D: myChip8.keyPress(9); break;
				case sf::Keyboard::F: myChip8.keyPress(0xE); break;

				case sf::Keyboard::Z: myChip8.keyPress(0xA); break;
				case sf::Keyboard::X: myChip8.keyPress(0); break;
				case sf::Keyboard::C: myChip8.keyPress(0xB); break;
				case sf::Keyboard::V: myChip8.keyPress(0xF); break;
			}
			break;
		case sf::Event::KeyReleased:
			switch (event.key.code)
			{
			case sf::Keyboard::Tab:
			{
				window.setFramerateLimit(60);
				break;
			}
			default:
				case sf::Keyboard::Num1: myChip8.keyRelease(1); break;
				case sf::Keyboard::Num2: myChip8.keyRelease(2); break;
				case sf::Keyboard::Num3: myChip8.keyRelease(3); break;
				case sf::Keyboard::Num4: myChip8.keyRelease(0xC); break;

				case sf::Keyboard::Q: myChip8.keyRelease(4); break;
				case sf::Keyboard::W: myChip8.keyRelease(5); break;
				case sf::Keyboard::E: myChip8.keyRelease(6); break;
				case sf::Keyboard::R: myChip8.keyRelease(0xD); break;

				case sf::Keyboard::A: myChip8.keyRelease(7); break;
				case sf::Keyboard::S: myChip8.keyRelease(8); break;
				case sf::Keyboard::D: myChip8.keyRelease(9); break;
				case sf::Keyboard::F: myChip8.keyRelease(0xE); break;

				case sf::Keyboard::Z: myChip8.keyRelease(0xA); break;
				case sf::Keyboard::X: myChip8.keyRelease(0); break;
				case sf::Keyboard::C: myChip8.keyRelease(0xB); break;
				case sf::Keyboard::V: myChip8.keyRelease(0xF); break;
			}
			break;
		}
	}

	//If emulateCycle returns false we need to stop the emulation
	if ( myChip8.isRunning && 
		!myChip8.emulateCycle(6) )
	{
		myChip8.stopEmulation();
	}

	updRegText(&regSStream, &regText);

	window.clear();

	// Sacrificing LoC/executable size, for speed
	if (myChip8.drawFlag)
	{
		for (size_t i = 0; i < 64 * 32; i++)
		{
			screen[i].setFillColor((mem::pixels[i]) ? sf::Color(FG_COLOR) : 
														sf::Color(BG_COLOR));
			window.draw(screen[i]);
		}
		if (isDebug) { window.draw(draw_rec); }
		myChip8.drawFlag = false;
	}
	else
	{
		for (size_t i = 0; i < 64 * 32; i++)
		{
			window.draw(screen[i]);
		}
	}

	//Draw to framebuffer and display
	if (isDebug)
	{
		// Get fps
		Framerate = 1.f / Clock.getElapsedTime().asSeconds();
		Clock.restart();
		replaceText(&fpsText, std::to_string(Framerate));

		// Draw all debug texts
		window.draw(debugText);
		window.draw(regText);
		window.draw(fpsText);

		if (myChip8.waitForKey) { window.draw(input_rec); }
	}

	window.display();
	}

	return 0;
}

//Update register values to regText
static void updRegText(std::ostringstream* ss, sf::Text* regText)
{
	ss->str("");
	ss->clear();

	for (auto i = 0; i < 16; i++)
	{
		*ss << "V" << i << "="
			<< std::setw(2) << int(mem::V[i]) % 256 << "\n";
		replaceText(regText, ss->str());
	}
}