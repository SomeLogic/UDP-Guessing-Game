#include "Client.h"
#include <iostream>

//! Client constructor.
/*!
	Binds socket to random available port, sets server port and IP to class variables.
	\param serverIP Server IP address written to class variable.
	\param serverPort Server port address written to class variable.
*/
Client::Client(const sf::IpAddress & serverIP, const unsigned short serverPort) : serverPort(serverPort), serverIP(serverIP)
{
	if (clientSocket.bind(sf::Socket::AnyPort) == sf::Socket::Done) {
		std::cout << "Socket bind on IP " << clientIP << ".\n";
		std::cout << "Data will be received on port " << clientSocket.getLocalPort() << ".\n";
	}
	else std::cout << "Something went wrong, failed to connect.\n";
}

//! Default client destructor.
/*!
*/
Client::~Client()
{
}

//! Sends packet to client.
/*!
	Sends previously set packet to client with given sessionID.
	\param x SessionID of client to which packet is being sent.
*/
void Client::SendPacket()
{
	packet.PrepareToSend();
	if (clientSocket.send(packet.content, sizeof(packet.content), serverIP, serverPort) != sf::Socket::Done) {
		std::cout << "Failed to send packet.\n";
	}
	//else {
	//	std::cout << "Data sent to server " << serverIP << " on port " << serverPort << ".\n";
	//	std::cout << "Data: " << std::bitset<8>(packet.content[0]) << " " << std::bitset<8>(packet.content[1]) << " " << std::bitset<8>(packet.content[2])
	//		<< " " << std::bitset<8>(packet.content[3]) << std::endl;
	//}
}

//! Sends acknowledgement to client.
/*!
	Sets answer field to ACK, sends packet and sets answer field back to what it was set before.
	Function also prints content of packet if it was sent succesfully or an error message otherwise.
*/
void Client::SendAck()
{
	packet.setAnswerField(answer::ack);
	SendPacket();
	packet.setAnswerField(answer);
}

//! Receives packet.
/*!
	Waits for packet to receive, then applies new values to class fields, sends his acknowledgement and prints content.
	\sa PrepareAfterReceive(), SendAck()
*/
void Client::ReceivePacket()
{
	sf::IpAddress sender;
	unsigned short _senderPort;
	size_t receivedSize;
	if (clientSocket.receive(packet.content, sizeof(packet.content), receivedSize, sender, _senderPort) != sf::Socket::Done) {
		std::cout << "Something went wrong with receiving packet :(\n";
	}
	else {
		packet.PrepareAfterReceive();
		operation = packet.getOperationField();
		answer = packet.getAnswerField();
		if (answer == answer::ack) {
			std::cout << "Received ACK.\n";
			ReceivePacket();
		}
		else {
		//	std::cout << "Data received from server " << sender << " on port " << _senderPort << ".\n";
		//	std::cout << "Data: " << std::bitset<8>(packet.content[0]) << " " << std::bitset<8>(packet.content[1]) << " " << std::bitset<8>(packet.content[2])
		//		<< " " << std::bitset<8>(packet.content[3]) << std::endl;
			SendAck();
		}
	}
}


//! Initializes game sequence.
/*!
	Sends empty packet to server to join a game. After that sends packet with L number to set game range. 
	If everything goes right, function calls next phase which is SendL().
	\sa SendL()
*/
void Client::Init()
{
	SendPacket();
	do {
		ReceivePacket();
	} while (answer != answer::idAssigned && operation != operation::tooManyPlayers);

	if (operation == operation::waitingForL && answer == answer::idAssigned) {
		std::cout << "Your id: " << int(packet.getSessionID()) << std::endl;
		SendL();
	}
	else if (operation == operation::tooManyPlayers) {
		std::cout << "Can't connect, too many clients.\n";
	}
}

//! Sends L number to server.
/*!
	Gets a number that belongs <0, 255> from user and sends it to server after changing 
	operation field to 'sendingNumber' and answer field to 'sendingL'. After that function
	waits for information about game begin from server when PlayGame() is called.
	\sa PlayGame(), SendPacket()
*/
void Client::SendL()
{
	int x;
	std::cout << "Type your L: ";
	do {
		std::cin >> x;
		if (x > 255 || x < 0)
			std::cout << "L must be between 0 and 255!\n";
	} while (x > 255 || x < 0);
	packet.setNumberField(x);
	rangeBegin = x;
	packet.setOperationField(operation::sendingNumber);
	packet.setAnswerField(answer::sendingL);
	SendPacket();

	int operation;
	int answer;
	do {
		ReceivePacket();
		operation = packet.getOperationField();
		answer = packet.getAnswerField();
		if (operation == operation::waitingForL && answer == answer::notYourTurn) {
			std::cout << "Wait for your turn, server is waiting for second client to response.\n";
		}
	} while (operation != operation::gameBegin && (answer != answer::sendingLowerL || answer != answer::sendingHigherL));

	PlayGame();
}


//! Manages gameplay phase.
/*!
	Receives game range, then in loop received packet is checked for operation and answer values. 
	If operation equals 'gameEnd' game is finished and program ends its run. Otherwise it is checked
	if answer was wrong, out of range or if it was given during other client's turn. In every loop
	repeat user enters new number from game range and counter is being updated.
	\sa SendPacket(), ReceivePacket()
*/
void Client::PlayGame()
{
	int x, temp = 0;
	if (answer == answer::sendingHigherL) {
		rangeEnd = packet.getNumberField();
	}
	else {
		rangeEnd = rangeBegin;
		rangeBegin = packet.getNumberField();
	}
	packet.setNumberField(0);
	std::cout << "Hidden number is between " << rangeBegin << " and " << rangeEnd << "\n";
	
	if (packet.getSessionID() == 1) {
		std::cout << "Counter: " << temp << std::endl;
		std::cout << "Type your guess: ";
		do {
			std::cin >> x;
			if (x > 255 || x < 0)
				std::cout << "Number must be between 0 and 255!\n";
		} while (x > 255 || x < 0);
		packet.setNumberField(x);
		packet.setOperationField(operation::sendingNumber);
		packet.setAnswerField(answer::sendingGuess);
		SendPacket();
	}
	else std::cout << "You joined 2nd, wait for your turn.\n";

	do {
		if (answer == answer::yourTurn) {
			if (packet.getCounter() == 15) {
				counterOverflow++;
			}
			int temp = 0;
			if (packet.getCounter() < 15) {
				temp = counterOverflow * 15 + int(packet.getCounter());
			}
			else {
				temp = counterOverflow * 15;
			}
			std::cout << "Counter: " << temp << std::endl;
			std::cout << "It's your turn.\n";
			std::cout << "Type your guess: ";
			do {
				std::cin >> x;
				if (x > 255 || x < 0)
					std::cout << "Number must be between 0 and 255!\n";
			} while (x > 255 || x < 0);
			packet.setNumberField(x);
			packet.setOperationField(operation::sendingNumber);
			packet.setAnswerField(answer::sendingGuess);
			SendPacket();
			ReceivePacket();
		}
		else {
			ReceivePacket();
		}

		switch (answer) {
		case (answer::notYourTurn): 
			std::cout << "Wait for you turn.\n"; break;
		case (answer::outOfRange): 
			std::cout << "Wrong answer, out of range.\n"; break;
		case (answer::wrong): 
			std::cout << "Wrong answer.\n"; break;
		}
	} while (operation == operation::playing);
	
	if (operation == operation::gameEnd) {
		switch (answer) {
		case answer::lose: std::cout << "You lost.\n\nPress enter to leave..."; break;
		case answer::win: std::cout << "You win.\n\nPress enter to leave..."; break;
		}
	}
	std::cin.ignore();
}
