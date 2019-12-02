#include "Server.h"

std::random_device rd;  //Will be used to obtain a seed for the random number engine
std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()

//! Default server constructor
/*! Socket is bind on port specified by variable serverPort.
*   Displays server IP and port or error message.*/
Server::Server()
{
	if (serverSocket.bind(serverPort) == sf::Socket::Done) {
		std::cout << "Socket bind on ip " << serverIP << ".\n";
		std::cout << "Data will be received on port " << serverSocket.getLocalPort() << ".\n";
	}
	else std::cout << "Something went wrong, failed to connect.\n";
}

//! Default server destructor.
/*! More specified description. */
Server::~Server()
{
}

//! Function randomizing number that belongs to <min, max>.
/*!
	\param min Lowest number of range.
	\param max Highest number of range.
	\return Randomized number. 
*/
int randomInt(int min, int max) {
	std::uniform_int_distribution<> dis(min, max);
	return dis(gen);
}

//! Function sending packet to client.
/*!
	Sending previously set packet to client with given sessionID.
	\param x SessionID of client to which packet is being sent.
*/
void Server::SendPacket(sf::Uint8 x)
{
	x--;
	packet.PrepareToSend();
	if (serverSocket.send(packet.content, sizeof(packet.content), clients.at(x).ip, clients.at(x).port) != sf::Socket::Done) { //Sending packet
		std::cout << "Failed to send packet.\n";
	}
	else {
		std::cout << "Data sent to client " << clients.at(x).ip << " on port " << clients.at(x).port << ".\n";
		std::cout << "Data: " << std::bitset<8>(packet.content[0]) << " " << std::bitset<8>(packet.content[1]) << " "  //Showing what was sent
			<< std::bitset<8>(packet.content[2]) << std::endl;
	}
}

//! Function sending receive acknowledgement to client.
/*!
	Setting answer field to ACK, sending packet and setting answer field back to what it was set before. 
	Function also prints content of packet if it was sent succesfully or an error message otherwise.
*/
void Server::SendAck()
{
	packet.setAnswerField(answer::ack);
	packet.PrepareToSend();
	if (serverSocket.send(packet.content, sizeof(packet.content), lastSenderIP, lastSenderPort) != sf::Socket::Done) {
		std::cout << "Failed to send packet.\n";
	}
	else {
		std::cout << "Data sent to client " << lastSenderIP << " on port " << lastSenderPort << ".\n";
		std::cout << "Data: " << std::bitset<8>(packet.content[0]) << " " << std::bitset<8>(packet.content[1]) << " " 
			<< std::bitset<8>(packet.content[2]) << std::endl;
	}
	packet.setAnswerField(answer);
}

//! Function receiving packet.
/*!
	Waiting for packet to receive, then applying new values to class fields, sending his acknowledgement and printing content.
	\sa PrepareAfterReceive(), SendAck()
*/
void Server::ReceivePacket()
{
	size_t receivedSize;
	if (serverSocket.receive(packet.content, sizeof(packet.content), receivedSize, lastSenderIP, lastSenderPort) != sf::Socket::Done) { //Receiving packet
		std::cout << "Something went wrong with receiving packet :(\n";
	}
	else {
		packet.PrepareAfterReceive();
		operation = packet.getOperationField();
		answer = packet.getAnswerField();
		if (answer == answer::ack) {
			std::cout << "Received ACK from " << lastSenderPort << "\n";//If received packet == ACK, then receive one more time
			ReceivePacket();
		}
		else {
			std::cout << "Data received from client " << lastSenderIP << " on port " << lastSenderPort << ".\n";
			std::cout << "Data: " << std::bitset<8>(packet.content[0]) << " " << std::bitset<8>(packet.content[1]) << " " //Showing what was received
				<< std::bitset<8>(packet.content[2]) << std::endl;
			SendAck();
		}
	}
}

//! Function creating new client.
/*!
	Creating new client if it is possible (max. 2 clients at a time). New entity is added to vector clients and packet with confirmation is sent. 
	If there is too many clients, functions sends a packet with this information without creating new entity.
	\sa SendPacket()
*/
void Server::NewClient()
{
	clientsCounter++;
	if (operation == operation::gettingID) {
		if (clientsCounter == 3) {
			clientsCounter--;                                    //if server has already 2 clients, send information about too many players on server
			packet.setOperationField(operation::tooManyPlayers);
			packet.PrepareToSend();
			if (serverSocket.send(packet.content, sizeof(packet.content), lastSenderIP, lastSenderPort) != sf::Socket::Done) { //Sending packet
				std::cout << "Failed to send packet.\n";
			}
			else {
				std::cout << "Data (too many players) sent to client " << lastSenderIP << " on port " << lastSenderPort << ".\n"; // Showing what was sent
				std::cout << "Data: " << std::bitset<8>(packet.content[0]) << " " << std::bitset<8>(packet.content[1]) << " "
					<< std::bitset<8>(packet.content[2]) << std::endl;
			}

		}
		else {
			ClientTemplate _client;
			_client.ip = lastSenderIP;
			_client.port = lastSenderPort;      //If server has less than two players:
			_client.id = clientsCounter;		//Create new player and push it into clients vector
			if (_client.id == 1) {
				_client.counter = -1;
				_client.isYourTurn = true;
			}
			clients.at(clientsCounter - 1) = _client;  //Pushing in right order(first client on 0, second at 1)
			packet.setOperationField(operation::waitingForL);
			packet.setAnswerField(answer::idAssigned);
			packet.setSessionID(_client.id);
			SendPacket(_client.id);                    //Sending information about assigned ID
		}
	}
}

//! Initializing game sequence.
/*!
	Receiving packets until 2 clients join a game. Function allows first client to 
	give L number before second client joins.
*/
void Server::Init()     //First state od server
{
	do {
		ReceivePacket();
		if (operation == operation::gettingID) { //If client send 0000, create new player
			NewClient();
		}
		else if (operation == operation::sendingNumber && answer == answer::sendingL) {
			if (clients.at(packet.getSessionID() - 1).isLSent == false) {
				clients.at(packet.getSessionID() - 1).isLSent = true;					//Player can send only one L
				clients.at(packet.getSessionID() - 1).range = packet.getNumberField();
				gameRange.push_back(packet.getNumberField());
				packet.setOperationField(operation::waitingForL);    //Pushing L into vector and sending message to player that he has to wait for his turn
				packet.setAnswerField(answer::notYourTurn);
				SendPacket(packet.getSessionID());
			}
			else {
				packet.setOperationField(operation::waitingForL);
				packet.setAnswerField(answer::notYourTurn);
				SendPacket(packet.getSessionID());
			}
		}

	} while (clientsCounter < 2);   //Until there are two players on server

	ReceivingRange();
}

//! Setting game range.
/*!
	Checks if received packet has L number. If it does then function checks if received packet is from right client. 
	Avoids situation when one client would try to send L twice. If both clients sent range numbers, isGameRange is set to true.
	\sa SendPacket()
*/
void Server::ReceiveRange()
{
	if (operation == operation::sendingNumber && answer == answer::sendingL) {
		if (clients.at(packet.getSessionID() - 1).isLSent == false) {
			clients.at(packet.getSessionID() - 1).isLSent = true;
			clients.at(packet.getSessionID() - 1).range = packet.getNumberField(); //Pushing L into vector and sending message to player that he has to wait for his turn
			gameRange.push_back(packet.getNumberField());
		}
		else {
			packet.setOperationField(operation::waitingForL);
			packet.setAnswerField(answer::notYourTurn);			 //sending message to player that he has to wait for his turn
			SendPacket(packet.getSessionID());
		}
	}

	if (gameRange.size() == 2) {
		isGameRange = true;
	}
}

//! Managing receiving range.
/*!
	Function is using ReceiveRange() to set game range. If it does happen, range is being sent to both clients and ManageGameplay() is called.
	\sa ReceiveRange(), SendRangeToPlayers(), ManageGameplay()
*/
void Server::ReceivingRange() {
	do {
		ReceivePacket();
		if (operation == operation::gettingID) {
			NewClient();							//Checking for new player, he can still join at this moment
		}											//or if new player want to join send message that he has to wait
		ReceiveRange();
	} while (isGameRange != true);      //Until two players send their Range

	SendRangeToPlayers();  //Send range to players
	ManageGameplay();      //Begin the game
}

//! Sends range to both players.
/*!
	Sends one of two L numbers to each client depending on which one was sent by him earlier, 
	he gets the other one with information if it is lower or higher number.
	\sa SendPacket()
*/
void Server::SendRangeToPlayers()
{
	int clientIDlowerL, clientIDhigherL;
	if (clients.at(0).range < clients.at(1).range) {
		clientIDlowerL = clients.at(0).id;
		clientIDhigherL = clients.at(1).id;			//Checking who sent lowerL anf higherL
	}
	else {
		clientIDlowerL = clients.at(1).id;
		clientIDhigherL = clients.at(0).id;
	}
	packet.setOperationField(operation::gameBegin);

	packet.setSessionID(clientIDlowerL);
	packet.setNumberField(clients.at(clientIDhigherL - 1).range);
	packet.setAnswerField(answer::sendingHigherL);
	SendPacket(clientIDlowerL);

	packet.setSessionID(clientIDhigherL);
	packet.setNumberField(clients.at(clientIDlowerL - 1).range);	//Sending lowerL and higherL to correct players
	packet.setAnswerField(answer::sendingLowerL);
	SendPacket(clientIDhigherL);
	gameRange.at(0) = clients.at(clientIDlowerL - 1).range;      //Pushning range to the server vector
	gameRange.at(1) = clients.at(clientIDhigherL - 1).range;
	secretNumber = randomInt(gameRange.at(0), gameRange.at(1));  //Drawing number
}

//! Manages whole gameplay sequence until game is ended.
/*!
	Receives and answers for packets as long as an answer is not guessed right. 
	In every loop repeat received packet is checked and specific answer is being sent 
	to a client including information about whose turn it is in current moment.
	\sa NewClient(), SendNotYourTurn(), SendGameEnded(), SendOutOfRange(), SendYourTurn(), SendWrongAnswer()
*/
void Server::ManageGameplay()
{
	bool isGameFinished = false;
	do {
		std::cout << "###### Secret number: " << secretNumber << " ######" << std::endl;
		ReceivePacket();
		if (operation == operation::gettingID) {		//Still checking for new players
			NewClient();
		}
		else if (operation == operation::sendingNumber && answer == answer::sendingGuess) {
			int guess = packet.getNumberField();
			int ID = packet.getSessionID() - 1;

			if (clients.at(ID).isYourTurn == false) { //Not your turn
				SendNotYourTurn();
			}
			if (guess == secretNumber) { //Someone has won
				SendGameEnded();
				isGameFinished = true;
				std::cout << "Game has ended.\n\nPress enter to leave...";
				std::cin.ignore();
			}
			else if (guess < gameRange.at(0) || guess > gameRange.at(1)) {
				SendOutOfRange();
				SendYourTurn();
			}
			else { //If none of above -> wrong answer
				if (clients.at(ID).counter == 15) {
					clients.at(ID).counter = 1;
				}
				else {
					clients.at(ID).counter++;
				}
				packet.setCounter(clients.at(ID).counter);
				SendWrongAnswer();
				if (packet.getSessionID() == 1) {
					clients.at(1).isYourTurn = true;
					clients.at(0).isYourTurn = false;
				}
				else {
					clients.at(1).isYourTurn = false;
					clients.at(0).isYourTurn = true;
				}
				SendYourTurn();
			}

		}
	} while (isGameFinished == false);
}

//! Sends information who won and who lost a game.
/*!
	Sets number field to 0, changes operation field to 'gameEnd' and answer field to 'win' or 'lose'.  
	Checks which client won a game and sends right information to both of them.
	\sa SendPacket()
*/
void Server::SendGameEnded() //Sending packet with info about win and lose
{
	int clientID = packet.getSessionID();
	packet.setOperationField(operation::gameEnd);
	packet.setAnswerField(answer::win);
	packet.setNumberField(0);
	SendPacket(clientID);

	if (clientID == 2) clientID = 1;
	else clientID = 2;
	packet.setAnswerField(answer::lose);
	SendPacket(clientID);
}

//! Sends information that received number is out of range.
/*!
	Sets number field to 0, changes operation field to 'playing' and answer field to 'outOfRange'. 
	Then sends packet to a client whos packet was received last.
	\sa SendPacket()
*/
void Server::SendOutOfRange() //Sending packet with info about out of range
{
	int clientID = packet.getSessionID();
	packet.setOperationField(operation::playing);
	packet.setAnswerField(answer::outOfRange);
	packet.setNumberField(0);
	SendPacket(clientID);
}

//! Sends information that server is waiting for other client's move.
/*!
	Sets number field to 0, changes operation field to 'playing' and answer field to 'notYourTurn'.
	Then sends packet to a client whos packet was received last.
	\sa SendPacket()
*/
void Server::SendNotYourTurn()  //Sending packet with info about not your turn
{
	packet.setOperationField(operation::playing);
	packet.setAnswerField(answer::notYourTurn);
	packet.setNumberField(0);
	SendPacket(packet.getSessionID());
}

//! Sends information that it is receiver's turn.
/*!
	Sets number field to 0, changes operation field to 'playing' and answer field to 'yourTurn'.
	Then sends packet to a cllient with value isYourTurn set to true.
	\sa SendPacket()
*/
void Server::SendYourTurn() //Sending packet with info about your turn
{
	int clientID;
	if (clients.at(packet.getSessionID() - 1).isYourTurn == true)
		clientID = packet.getSessionID();
	else {
		if (packet.getSessionID() == 1) {
			clientID = 2;
			packet.setSessionID(2);
		}
		else {
			clientID = 1;
			packet.setSessionID(1);
		}
	}

	packet.setOperationField(operation::playing);
	packet.setAnswerField(answer::yourTurn);
	packet.setNumberField(0);
	SendPacket(clientID);
}

//! Sends information that received number is not a secret number.
/*!
	Sets number field to 0, changes operation field to 'playing' and answer field to 'wrong'.
	Then sends packet to a client whos packet was received last.
	\sa SendPacket()
*/
void Server::SendWrongAnswer() //Sending packet with info about wrong answer
{
	int clientID = packet.getSessionID();
	packet.setOperationField(operation::playing);
	packet.setAnswerField(answer::wrong);
	packet.setNumberField(0);
	SendPacket(clientID);
}
