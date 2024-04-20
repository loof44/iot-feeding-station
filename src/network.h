/**
 * \file Network.h
 * \brief A File to handle network connectivity.
 *
 * This header file contains a declaration for the connectToNetwork function which is used to establish a WiFi connection.
 */

#ifndef NETWORK_H
#define NETWORK_H

/** 
 * \brief Function for connecting to a network
 *
 * This function tries to establish a connection to a specified WiFi network using provided SSID and password. 
 *
 * SSID (Service Set Identifier) is the name of your Wireless network, it's a sequence of characters that uniquely names your wireless local area network (WLAN). 
 *
 * The function uses pass-by-reference to return the state of the connection attempt, with 'true' indicating a successful connection & 'false' indicating failure. 
 * 
 * @param ssid A pointer to a constant character (the first character of your SSID string)
 * @param password A pointer to a constant character (the first character of your network password string)
 * @param state A boolean reference which is updated with the status of the network connection (True - Connection successful, False - Connection failed)
*/
void connectToNetwork(const char* ssid, const char* password, bool &state);

#endif //NETWORK_H
