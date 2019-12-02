#pragma once
#include <SFML/System.hpp>
#include <SFML/Network.hpp>

class NetworkTemplate
{
public:
	enum operation {
		/*! 0000 */ gettingID = 0,
		/*! 0001 */ waitingForL = 1,
		/*! 0010 */ sendingNumber = 2,
		/*! 0011 */ gameBegin = 3,
		/*! 0100 */ playing = 4,
		/*! 0111 */ tooManyPlayers = 7,
		/*! 1111 */ gameEnd = 15
	};

	enum answer {
		/*! 0000 */ getID = 0,
		/*! 0001 */ idAssigned = 1,
		/*! 0010 */ outOfRange = 2,
		/*! 0011 */ win = 3,
		/*! 0100 */ notYourTurn = 4,
		/*! 0101 */ yourTurn = 5,
		/*! 0110 */ sendingL = 6,
		/*! 0111 */ sendingGuess = 7,
		/*! 1000 */ sendingHigherL = 8,
		/*! 1001 */ sendingLowerL = 9,
		/*! 1100 */ lose = 12,
		/*! 1101 */ wrong = 13,
		/*! 1111 */ ack = 15
	};

public:
	NetworkTemplate();
	~NetworkTemplate();
};

