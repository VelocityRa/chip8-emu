#include <SFML/Graphics.hpp>
#include <sstream>
#include <string>
#include <iomanip>

#include "chip8-cpu.h"
#include "chip8-memory.h"


//Resolution multiplier
#define RES_MULT 12
//Padding pixels for debug strings
#define PAD 3

#define WIDTH_PIXELS 64
#define HEIGHT_PIXELS 32
#define H HEIGHT_PIXELS * RES_MULT
#define W WIDTH_PIXELS * RES_MULT

#ifdef _DEBUG
auto isDebug = true;
#else
auto isDebug = false;
#endif

void appendText(sf::Text* text, std::string st);
void replaceText(sf::Text* text, std::string st);

chip8 myChip8;

int main(int argc, char* argv[])
{
	std::string game_path;

	{
		if (argc > 1)
		{
			game_path = argv[1];
		}
		else
		{
			if (isDebug)
			{
				game_path = "C:/Users/Nikos/Documents/Visual Studio 2015/Projects/chip8-emu/roms/PONG";
			}
			else
			{
				return 1;
			}
		}
	}
	//Setup Window creation
	sf::ContextSettings settings;
	settings.antialiasingLevel = 0;

	//Create Window
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
	}

	sf::Text debugText;
	sf::Text regText;

	if (isDebug)
	{
		debugText.setFont(mc_font);
		debugText.setCharacterSize(16);
		debugText.setPosition(0 + PAD, 8 + PAD);

		regText.setFont(mc_font);
		regText.setCharacterSize(8);
		regText.setPosition(W - 6 * 5 - PAD, 4 + PAD);
	}

	myChip8.initialize();
	auto load_result =
		myChip8.loadGame(game_path.c_str());
	appendText(&debugText, "Loaded  " + std::to_string(load_result) + "  bytes to memory");

	std::ostringstream out;
	out << std::hex << std::setfill('0') << std::uppercase;


	//Main Loop
	while (window.isOpen())
	{
		out.str("");
		out.clear();

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


		//Update register values to regText
		for (auto i = 0; i < 16; i++)
		{
			out << "V" << i << "="
				<< std::setw(2) << int(V[i]) % 256 << "\n";
			replaceText(&regText, out.str());
		}

		//Draw to framebuffer and display
		window.clear();
		if (isDebug)
		{
			window.draw(debugText);
			window.draw(regText);
		}
		window.display();
	}

	return 0;
}

//Append string "st" to the debug string
inline void appendText(sf::Text* text, std::string st)
{
	text->setString(text->getString() + st + "\n");
}

//Replace debug string with st
inline void replaceText(sf::Text* text, std::string st)
{
	text->setString(st);
}
