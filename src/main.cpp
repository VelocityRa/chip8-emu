#include <SFML/Graphics.hpp>
#include <sstream>
#include <string>
#include <iomanip>


#include "cpu.h"
#include "memory.h"


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

int main()
{
	//Setup Window creation
	sf::ContextSettings settings;
	settings.antialiasingLevel = 0;

	//Create Window
	sf::RenderWindow window(sf::VideoMode(W, H), "Chip-8 Emulator",
	                        sf::Style::Titlebar | sf::Style::Close,
	                        settings);
	window.setVerticalSyncEnabled(true);

	//Load a pixely font
	sf::Font mc_font;
	if (!mc_font.loadFromFile("resources/fonts/Minecraftia-Regular.ttf"))
	{
		//Couldn't load font
		window.close();
	}

	sf::Text debugText;
	sf::Text regText;

	//Initialize the emulator's modules
	mem::initialize();
	//cpu::initialize();

	//Test registers, actual ones are V[16]
	unsigned char
		V2[16] = {0xDE, 0xAD, 0xBE, 0xEEF};

	if (isDebug)
	{
		debugText.setFont(mc_font);
		debugText.setCharacterSize(16);
		debugText.setPosition(0 + PAD, 8 + PAD);

		appendText(&debugText, "TEST");

		regText.setFont(mc_font);
		regText.setCharacterSize(8);
		regText.setPosition(W - 6 * 5 - PAD, 4 + PAD);
	}

	std::ostringstream out;
	out << std::hex << std::setfill('0') << std::uppercase;

	//Main Loop
	while (window.isOpen())
	{
		out.str("");
		out.clear();

		//Update register values to regText
		for (auto i = 0; i < 16; i++)
		{
			out << "V" << i << "="
				<< std::setw(2) << int(V2[i]) << "\n";
			replaceText(&regText, out.str());
		}

		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
				default:
					break;
			}
		}

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
void appendText(sf::Text* text, std::string st)
{
	text->setString(text->getString() + st + "\n");
}

//Replace debug string with st
void replaceText(sf::Text* text, std::string st)
{
	text->setString(st);
}
