#if _MSC_VER > 1000
#pragma once
#endif

#ifndef TEXTTOOLS_H
#define TEXTTOOLS_H

#include <SFML/Graphics.hpp>
#include <sstream>

extern sf::Text debugText;

//Append a string to the Text
inline void appendText(sf::Text* text, std::string st)
{
	text->setString(text->getString() + st + "\n");
}

//Append a stringstream to the Text
inline void appendText(sf::Text* text, std::ostringstream* ss)
{
	text->setString(text->getString() + ss->str() + "\n");
}

//Replace Text with string st
inline void replaceText(sf::Text* text, std::string ss)
{
	text->setString(ss);
}

//Replace Text with stringstream sss
inline void replaceText(sf::Text* text, std::ostringstream* ss)
{
	text->setString(ss->str());
}


#endif