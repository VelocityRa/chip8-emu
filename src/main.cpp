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
#define H HEIGHT_PIXELS * RES_MULT
#define W WIDTH_PIXELS * RES_MULT

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
	sf::RenderWindow window(sf::VideoMode(W, H), "Chip-8 Emulator",
	                        sf::Style::Titlebar | sf::Style::Close,
	                        settings);
	window.setFramerateLimit(60);

	//Load a pixely font
	sf::Font mc_font;

	if (!mc_font.loadFromFile("resources/fonts/Minecraftia-Regular.ttf"))
	{
		//Couldn't load font
		window.close();
		return 1;
	}

	sf::Text regText;

	if (isDebug)
	{
		debugText.setFont(mc_font);
		debugText.setCharacterSize(12);
		debugText.setPosition(0 + PAD, 6 + PAD);
		debugText.setColor(sf::Color(100, 100, 100));

		regText.setFont(mc_font);
		regText.setCharacterSize(8);
		regText.setPosition(W - 6 * 5 - PAD, 4 + PAD);
		regText.setColor(sf::Color::Red);
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
		screen[i].setPosition(i%64*RES_MULT,i/64*RES_MULT);
		screen[i].setSize(sf::Vector2f(RES_MULT, RES_MULT));
	}

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
				case sf::Keyboard::F3:
				{
					isDebug=!isDebug;
				}
				}
			default:
				break;
			}
		}

		//If emulateCycle returns false we need to stop the emulation
		if ( myChip8.isRunning && 
			!myChip8.emulateCycle() )
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
				screen[i].setFillColor((mem::pixels[i]) ? sf::Color::White : 
														  sf::Color::Black);
				window.draw(screen[i]);
			}
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
			window.draw(debugText);
			window.draw(regText);
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