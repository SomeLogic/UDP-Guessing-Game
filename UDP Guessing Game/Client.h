#pragma once
#include "NetworkTemplate.h"
#include "Packet.h"
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include <bitset>

class Client :
	public NetworkTemplate
{
public:
	sf::UdpSocket clientSocket;
	Packet packet;
	unsigned short serverPort;
	sf::IpAddress serverIP;
	sf::IpAddress clientIP = "169.254.247.66";
	int rangeBegin, rangeEnd;
	sf::Uint8 operation, answer;
	int counterOverflow = 0;

public:
	Client();
	Client(const sf::IpAddress &serverIP, const unsigned short serverPort);
	~Client();
	void SendPacket();
	void SendAck();
	void ReceivePacket();
	void Init();
	void SendL();
	void PlayGame();
};

