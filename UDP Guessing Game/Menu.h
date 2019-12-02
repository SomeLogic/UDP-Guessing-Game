#pragma once
#include <SFML/System.hpp>
#include <SFML/Network.hpp>
#include "NetworkTemplate.h"
#include "Client.h"
#include "Server.h"
#include "Packet.h"
class Menu
{
public:
	Menu();
	~Menu();
	void serverHandling();
	void clientHandling();
	void start();
};

