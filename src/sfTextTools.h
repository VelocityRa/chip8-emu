#if _MSC_VER > 1000
#pragma once
#endif

#ifndef TEXTTOOLS_H
#define TEXTTOOLS_H

#include <SFML/Graphics.hpp>
#include <sstream>

extern sf::Text debugText;

// Replace Text with string st
inline void replaceText(sf::Text* text, std::string ss)
{
	text->setString(ss + '\n');
}

// Replace Text with stringstream ss
inline void replaceText(sf::Text* text, std::ostringstream* ss)
{
	text->setString(ss->str() + '\n');
}

// Append a string to the Text
inline void appendText(sf::Text* text, std::string st)
{
	if (text->getLocalBounds().height > 32 * 12 - 35)
	{
		replaceText(text, st);
	}
	else
	{
		text->setString(text->getString() + st + "\n");
	}
}

// Append a stringstream to the Text,
// in case we need something more formatted
inline void appendText(sf::Text* text, std::ostringstream* ss)
{
	if (text->getLocalBounds().height > 32 * 12 - 35)
	{
		replaceText(text, ss);
	}
	else
	{
		text->setString(text->getString() + ss->str() + "\n");
	}
}

#endif