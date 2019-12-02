#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include "NetworkTemplate.h"
#include <iostream>
#include "Menu.h"

/*! \mainpage UDP Guessing Game 
 *
 * Game is a school project realizing 2<>1 relation between clients and a server.
 *
 * These files include documentation of all functions and variables necessary to understand back-end of whole program.
 *
 */

int main(){
	Menu menu;
	menu.start();
	std::cin.ignore();
	return 0;
}