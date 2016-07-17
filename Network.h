#ifndef NET
#define NET

#include <iostream>
#include "polonet.h"
#include "allegro.h"
#include "def.h"
#include <Windows.h>
#include <ctime>

using namespace std;

#define PORT 15786
#define SIZEMAX	20
#define CLIENT 6
#define SERVER 7
#define TIME_OUT 10

class Network
{
public:
	//protected:
	PolonetConn connection;
	int port;
	char IP[SIZEMAX];
	char typeconn;
	bool abort;
};

class initNetwork : public Network
{
public:
	//initNetwork(){};
	initNetwork();
	~initNetwork();
	void initializeNet(ALL *allegro); //Initializes connection

protected:
	int randNum; // Random number generated by RandGenerator()
	bool endflag; //Flag that shows if user desired to finish connection
	void seedStart(); // Starts Seed - JV
	void randGenerator(); //Generates random number from 200 to 10k - JV
	void getIP(ALL* allegro); //Gets string of IP and saves it in IP -JV
};

#endif