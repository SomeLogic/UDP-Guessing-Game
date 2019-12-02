#pragma once
#include <array>
#include <SFML/System.hpp>
#include "NetworkTemplate.h"

class Packet
{
public:
	std::array<sf::Uint8, 2> header = { 0, 0 };
	sf::Uint8 data = { 0 };
	sf::Uint8 content[3] = { 0, 0, 0 };
	
public:
	Packet();
	~Packet();
	void setOperationField(NetworkTemplate::operation operation);
	void setOperationField(sf::Uint8 operation);
	void setSessionID(sf::Uint8 id);
	void setAnswerField(NetworkTemplate::answer answer);
	void setAnswerField(sf::Uint8 answer);
	void setCounter(sf::Uint8 counter);
	void setNumberField(sf::Uint8 number);
	sf::Uint8 getOperationField();
	sf::Uint8 getSessionID();
	sf::Uint8 getAnswerField();
	sf::Uint8 getCounter();
	sf::Uint8 getNumberField();
	void PrepareAfterReceive();
	void PrepareToSend();
};

/*
oooossss aaaacccc NNNNNNNN
oooossss aaaacccc NNNNNNNN
o - operation field
s - session ID
a - answer field
c - counter
N - number field

After sending range once, last 2 bytes are reseted, guessed number is being sent on 3rd byte
11110000(2) - 240(10)
00001111(2) - 15(10)
*/