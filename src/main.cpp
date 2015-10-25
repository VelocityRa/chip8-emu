#include <SFML/Graphics.hpp>

#include "cpu.h"
#include "memory.h"

//Resolution multiplier
#define RES_MULT 12

#define WIDTH_PIXELS 64
#define HEIGHT_PIXELS 32
#define H HEIGHT_PIXELS * RES_MULT
#define W WIDTH_PIXELS * RES_MULT

void appendDebug(std::string st);
void replace_debug(std::string st);

sf::Text debugText;
auto isDebug = true;

int main()
{
	//Setup Window creation
	sf::ContextSettings settings;
	settings.antialiasingLevel = 0;

	//Create Window
	sf::RenderWindow window(sf::VideoMode(W, H), "Chip-8 Emulator", \
							sf::Style::Titlebar | sf::Style::Close, \
							settings);
	window.setVerticalSyncEnabled(true);

	//Load a pixely font
	sf::Font mc_font;
	if (!mc_font.loadFromFile("resources/fonts/Minecraftia-Regular.ttf"))
	{
		//Couldn't load font
		window.close();
	}

	debugText.setFont(mc_font);
	debugText.setCharacterSize(16);
	debugText.setPosition(0 + 2, 8 + 2);

	//Initialize the emulator's modules
	mem::initialize();
	cpu::initialize();

	//Main Loop
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type){
				case sf::Event::Closed:
					window.close();
				default:
					break;
			}
		}

		window.clear();
		if (isDebug) { window.draw(debugText); }
		window.display();
	}

	return 0;
}

//Append string "st" to the debug string
void appendDebug(std::string st)
{
	debugText.setString(debugText.getString() + st + "\n" );
}

//Replace debug string with st
void replaceDebug(std::string st)
{
	debugText.setString(st);
}
