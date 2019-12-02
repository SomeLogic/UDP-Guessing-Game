#pragma once
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <bitset>
#include "NetworkTemplate.h"
#include "Packet.h"
#include <algorithm>
#include <random>

struct ClientTemplate {
	sf::IpAddress ip;
	unsigned short port;
	int id;
	int counter = 0;
	bool isLSent = false;
	bool isYourTurn = false;
	int range = 0;
};

class Server :
	public NetworkTemplate
{
public:
	sf::UdpSocket serverSocket;
	unsigned short serverPort = sf::Socket::AnyPort, lastSenderPort;
	sf::IpAddress serverIP = "192.168.1.69", lastSenderIP;
	Packet packet;
	std::array<ClientTemplate, 2> clients;
	int clientsCounter = 0;
	sf::Uint8 operation, answer;
	int secretNumber;
	std::vector<int> gameRange;
	bool isGameRange = false;
	int currentClient;



public:
	//! Default server constructor
	/*! Socket is bind on port specified by variable serverPort. 
	*   Displays server IP and port or error message.*/
	Server();
	//! Default server destructor.
	/*! More specified description. */
	~Server();
	void SendPacket(sf::Uint8 x);
	void SendAck();
	void ReceivePacket();
	void Init();
	void ReceiveRange();
	void ReceivingRange();
	void SendRangeToPlayers();
	void NewClient();
	void ManageGameplay();
	void SendGameEnded();
	void SendOutOfRange();
	void SendNotYourTurn();
	void SendYourTurn();
	void SendWrongAnswer();
};

