#include <SFML/Graphics.hpp>
#include <sstream>
#include <string>
#include <iomanip>

#include "chip8-cpu.h"
#include "chip8-memory.h"
#include "sfTextTools.h"


//Resolution multiplier
#define RES_MULT 12
//Padding pixels for debug strings
#define PAD 3


#define FG_COLOR 255, 255, 255
#define BG_COLOR 0, 40, 102


#ifdef _DEBUG
auto isDebug = true;
#else
auto isDebug = false;
#endif

void appendText(sf::Text* text, std::string st);
void replaceText(sf::Text* text, std::string st);

static void updRegText(std::ostringstream* ss, sf::Text* regText);

void createScreen();
void resizeScreen(bool isExtended);

chip8 myChip8;
std::vector<sf::RectangleShape> screen(64 * 32);
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
		return -1;
	}

	//Setup window creation
	sf::ContextSettings settings;
	settings.antialiasingLevel = 0;

	//Create window
	sf::RenderWindow window(sf::VideoMode(64 * RES_MULT, 32 * RES_MULT), "Chip-8 Emulator",
	                        sf::Style::Titlebar | sf::Style::Close,
	                        settings);
	window.setFramerateLimit(60);

	//Load a pixely font
	sf::Font mc_font;
	if (!mc_font.loadFromFile("resources/fonts/Minecraftia-Regular.ttf"))
	{
		//Couldn't load font
		window.close();
		return -1;
	}

	srand(static_cast<unsigned int>(time(nullptr))); // use current time as seed for random generator

	sf::Color fg_color(FG_COLOR);
	sf::Color bg_color(BG_COLOR);

	// Set up debugging stuff
	// -----------------------------------------------------------
	sf::Text regText;
	sf::Text fpsText;

	sf::Clock Clock;
	unsigned short Framerate;

	debugText.setFont(mc_font);
	debugText.setCharacterSize(12);
	debugText.setPosition(0 + PAD, 12 * 3 + 6 + PAD);
	debugText.setColor(sf::Color(100, 100, 100));

	regText.setFont(mc_font);
	regText.setCharacterSize(8);
	regText.setPosition(64*RES_MULT - 6 * 5 - PAD, 40 + PAD);
	regText.setColor(sf::Color::Red);

	fpsText.setFont(mc_font);
	fpsText.setCharacterSize(12);
	fpsText.setPosition(64*RES_MULT - 6 * 3 - PAD, 8 + PAD);
	fpsText.setColor(sf::Color(sf::Color::Cyan));

	//Create the stream/string for registry display in Debug mode
	std::ostringstream regSStream;
	regSStream << std::hex << std::setfill('0') << std::uppercase;

	//Signifies waiting for input
	sf::RectangleShape input_rec(sf::Vector2f(32, 7));
	input_rec.setPosition(64*RES_MULT - 34, 21);
	input_rec.setFillColor(sf::Color::Yellow);

	//Signifies that the draw flag is set
	sf::RectangleShape draw_rec(sf::Vector2f(32, 7));
	draw_rec.setPosition(64*RES_MULT - 34, 28);
	draw_rec.setFillColor(sf::Color::Green);

	// -----------------------------------------------------------

	appendText(&debugText, "Initializing chip8");
	if (myChip8.initialize() )
	{
		return -1;
	}

	auto load_result = myChip8.loadGame(game_path.c_str());
	appendText(&debugText, "Loaded  " + std::to_string(load_result) + "  bytes to memory");

	createScreen();

	//myChip8.isRunning = false;

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
				myChip8.emulateCycle(1, true);
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
				// Assign keys to Chip8 key codes 
				#define keypress(x) myChip8.keyPress(x); break;

				case sf::Keyboard::Num1: keypress(1); 
				case sf::Keyboard::Num2: keypress(2);
				case sf::Keyboard::Num3: keypress(3);
				case sf::Keyboard::Num4: keypress(0xC);

				case sf::Keyboard::Q: keypress(4);
				case sf::Keyboard::W: keypress(5);
				case sf::Keyboard::E: keypress(6);
				case sf::Keyboard::R: keypress(0xD);

				case sf::Keyboard::A: keypress(7);
				case sf::Keyboard::S: keypress(8);
				case sf::Keyboard::D: keypress(9);
				case sf::Keyboard::F: keypress(0xE);

				case sf::Keyboard::Z: keypress(0xA);
				case sf::Keyboard::X: keypress(0);
				case sf::Keyboard::C: keypress(0xB);
				case sf::Keyboard::V: keypress(0xF);
				#undef keypress
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
				// Assign keys to Chip8 key codes 
				#define keyrelease(x) myChip8.keyRelease(x); break;

				case sf::Keyboard::Num1: keyrelease(1);
				case sf::Keyboard::Num2: keyrelease(2);
				case sf::Keyboard::Num3: keyrelease(3);
				case sf::Keyboard::Num4: keyrelease(0xC);

				case sf::Keyboard::Q: keyrelease(4);
				case sf::Keyboard::W: keyrelease(5);
				case sf::Keyboard::E: keyrelease(6);
				case sf::Keyboard::R: keyrelease(0xD);

				case sf::Keyboard::A: keyrelease(7);
				case sf::Keyboard::S: keyrelease(8);
				case sf::Keyboard::D: keyrelease(9);
				case sf::Keyboard::F: keyrelease(0xE);

				case sf::Keyboard::Z: keyrelease(0xA);
				case sf::Keyboard::X: keyrelease(0);
				case sf::Keyboard::C: keyrelease(0xB);
				case sf::Keyboard::V: keyrelease(0xF);
				#undef keyrelease
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

	// Update screen with pixels[] values and draw it
	// Sacrificing LoC/executable size, for speed
	if (myChip8.drawFlag)
	{
		for (size_t i = 0; i < screen.size(); i++)
		{
			screen[i].setFillColor(mem::pixels[i] ? fg_color : bg_color);
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
		Framerate = short(1.f / Clock.getElapsedTime().asSeconds());
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

void createScreen()
{
	for (size_t i = 0; i < 64 * 32; i++)
	{
		screen[i].setPosition(i % 64 * RES_MULT, i / 64 * RES_MULT);;
		screen[i].setSize(sf::Vector2f(RES_MULT, RES_MULT));
		screen[i].setFillColor(sf::Color(BG_COLOR));
	}
}

void resizeScreen(bool isExtended)
{
	auto size = isExtended ? 128 * 64 : 64 * 32;
	auto sizeW = isExtended ? 128 : 64;

	screen.clear();
	screen.resize(size);
	for (size_t i = 0; i < size; i++)
	{
		screen[i].setPosition(i % sizeW * RES_MULT/2, i / sizeW * RES_MULT/2);
		screen[i].setSize(sf::Vector2f(RES_MULT/2, RES_MULT/2));
		screen[i].setFillColor(sf::Color(BG_COLOR));
	}
}