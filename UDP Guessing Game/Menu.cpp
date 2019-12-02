#include "Menu.h"
#include <iostream>


//! Default Menu constructor. Displays information about possible choose options.
/*!
*/
Menu::Menu()
{
	std::cout << "Running UDP Protocol in relation 2->1, please choose your device:\n"
		<< "1. Client,\n"
		<< "2. Server (max. 1).\n";
}

//! Default Menu destructor.
Menu::~Menu()
{
}


//! Server's menu, creates new Server object.
/*!
	Creates new Client object with given values and calls Init() to start program.
*/
void Menu::serverHandling()
{
	Server server; 
	std::cout << "You choosed server.\n";

	server.Init();

}

//! Client's menu, creates new Client object.
/*!
	Asks user for server IP and port. Creates new Client object with given values and calls Init() to start program.
	\sa Client::Init()
*/
void Menu::clientHandling()
{
	sf::IpAddress serverip;
	unsigned short port;
	 char x = 'n';
	 std::cout << "You choosed client.\n";
	do{
		std::cout<< "Type the IP of the server.\n";
		std::cin >> serverip;
		std::cout << "Type the port of the server.\n";
		std::cin >> port;
		std::cout << "Are you sure? Type Y/N.\n";
		std::cin >> x;
	} while (x != 'y' && x != 'Y');

	Client client(serverip, port); //making new client

	client.Init();
}

//! Starts choosed program (client/server).
/*!
	Checks if entered number is right. If not program asks for other choice. 
	After enetering right number, program starts choosed part which is server or client.
	\sa clientHandling(), serverHandling()
*/
void Menu::start()
{
	int choice;
	std::cin >> choice;
	while (choice != 1 && choice != 2) {
		std::cout << "Wrong choice, please enter 1 or 2: ";
		std::cin >> choice;
	}

	if (choice == 1) {
		clientHandling();
	}
	else if (choice == 2) {
		serverHandling();
	}
}
	