#include "Packet.h"

//! Default Packet constructor.
Packet::Packet()
{
}

//! Default Packet destructor.
Packet::~Packet()
{
}

//! Sets operation field to given value.
/*!
	Sets 4 bytes from the left to 0, then adds it (OR) to operation value moved by 4 bits to the left.
	\param operation Enum value defines current operation.
*/
void Packet::setOperationField(NetworkTemplate::operation operation)
{
	header.at(0) &= 15;
	header.at(0) |= (operation << 4);
}

//! Sets operation field to given value.
/*!
	Sets 4 bits from the left to 0, then adds it (OR) to operation value moved by 4 bits to the left.
	\param operation Uint8 value defines current operation.
*/
void Packet::setOperationField(sf::Uint8 operation)
{
	header.at(0) &= 15;
	header.at(0) |= (operation << 4);
}

//! Sets sessionID field to given value.
/*!
	Sets 4 bytes from the left to 0, then adds it (OR) to id value moved by 4 bits to the left.
	\param id Uint8 value defines current id value.
*/
void Packet::setSessionID(sf::Uint8 id)
{
	header.at(1) &= 15;
	header.at(1) |= (id << 4);
}

//! Sets answer field to given value.
/*!
	Sets 4 bytes from the right to 0, then adds it (OR) to id value.
	\param answer Enum value defines current answer.
*/
void Packet::setAnswerField(NetworkTemplate::answer answer)
{
	header.at(0) &= 240;
	header.at(0) |= answer;
}

//! Sets answer field to given value.
/*!
	Sets 4 bytes from the right to 0, then adds it (OR) to id value.
	\param answer Uint8 value defines current answer.
*/
void Packet::setAnswerField(sf::Uint8 answer)
{
	header.at(0) &= 240;
	header.at(0) |= answer;
}

//! Sets counter field to given value.
/*!
	Sets 4 bytes from the right to 0, then adds it (OR) to counter value.
	\param counter Uint8 value defines current counter value.
*/
void Packet::setCounter(sf::Uint8 counter)
{
	header.at(1) &= 240;
	header.at(1) |= counter;
}

//! Sets number field to given value.
/*!
	Sets third byte of packet to given value.
	\param number Uint8 value defines number value.
*/
void Packet::setNumberField(sf::Uint8 number)
{
	data = number;
}

//! Gets current operation field.
/*!
	Gets bits 1-4 of first header byte.
	\return Uint8 representing current operation value.
*/
sf::Uint8 Packet::getOperationField()
{
	return ((header.at(0) & 240) >> 4);
}

//! Gets current sessionID field.
/*!
	Gets bits 1-4 of second header byte.
	\return Uint8 representing current id value.
*/
sf::Uint8 Packet::getSessionID()
{
	return (header.at(1) & 240) >> 4;
}

//! Gets current answer field.
/*!
	Gets bits 5-8 of first header byte.
	\return Uint8 representing current answer value.
*/
sf::Uint8 Packet::getAnswerField()
{
	return (header.at(0) & 15);
}

//! Gets current counter field.
/*!
	Gets bits 1-4 of second header byte.
	\return Uint8 representing current counter value.
*/
sf::Uint8 Packet::getCounter()
{
	return (header.at(1) & 15);
}

//! Gets current number field.
/*!
	Gets third byte of packet.
	\return Uint8 representing current number value.
*/
sf::Uint8 Packet::getNumberField()
{
	return sf::Uint8(data);
}

//! Prepares packet to use after receive.
/*!
	Takes data from content and puts it inside containers header and data to make operations on packet easier.
*/
void Packet::PrepareAfterReceive()
{
	header.at(0) = content[0];
	header.at(1) = content[1];
	data = content[2];
}

//! Prepares packet to send before receive.
/*!
	Puts data from header and data to content which is then sent to a receiver. 
*/
void Packet::PrepareToSend()
{
	content[0] = header.at(0);
	content[1] = header.at(1);
	content[2] = data;
}
