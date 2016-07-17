#include "fsmInit.h"

FSMI::FSMI(){
	exit=FALSE;//Para que en principio entre al while
	state=IDLE;//Estado inicial
	turnsort();
	nameready=FALSE;//Todavia sin nombre
}
void FSMI::turnsort(){
	turn= rand()%2+CLIENT;// Turno aleatorio.  El define "CLIENT" es el offset del numero que se genera (0/1 + CLIENT)
}
void FSMI::start(ALL* allegro){
	drawStartMenu(allegro);
	net.initializeNet(allegro);
	pck.connection = net.connection;

	if(net.typeconn == SERVER){//Dependiendo si se empieza como server o como client:
		turn = CLIENT;
		state = ASK4NAME;
	}
	else{
		turn = SERVER;
		state = W4NAME;
	}
	while(state != END) {
		switch(state)
		{
		case ASK4NAME:
			pck.name();
			pck.sendPack();
			changestate(W4STRING);
			break;
		case W4NAME:
			Sleep(300);
			data = pck.receivePack();
			if(data.ID == NAME)
				changestate(SENDNAME);
			break;
		case SENDNAME:
			///HICE CAMBIOS ACA QUE NO ME GUSTARON.... VER!!!
			char *a;
			printAllegro(allegro, "Please, enter your name:");
			a = getcharAllegro(allegro, 8, HEIGHT * allegro->screenHeight / MAXRESOLUTION_Y / 2);
	
			pck.nameis(a);
			Informacion.addName(a);
			pck.sendPack();
			changestate(W4ACK);
			break;
		case W4STRING:
			printAllegro(allegro,"Waiting for your friend's name...");
			Sleep(300);
			data = pck.receivePack();
			Informacion.addFriendName(data.name);
			if(data.ID == NAME_IS){
				printAllegro(allegro,"Your friend's name is:  ");
				Sleep(1000);
				printAllegro(allegro,data.name);
				Sleep(1000);
				changestate(SENDACK);
			}
			break;
		case SENDACK:
			pck.ack();
			pck.sendPack();
			switch(prevstate)
			{
				case W4STRING:
					if(nameready == FALSE){
						changestate(W4NAME);
						nameready = TRUE;
					}
					else{
						if(net.typeconn == CLIENT)
							changestate(W4STARTINFO);
						else
							changestate(STARTINFO);
					}
					break;
				case W4STARTINFO:
					pck.ack();
					pck.sendPack();
					state = W4TURN;
					break;
			}
		case W4ACK:
			Sleep(300);
			data = pck.receivePack();
			if(data.ID == ACK){
				switch(prevstate)
				{
					case SENDNAME:
						if(nameready == FALSE){
							changestate(ASK4NAME);
							nameready = TRUE;
						}
						else
							if(net.typeconn == CLIENT)
								changestate(W4STARTINFO);
							else
								changestate(STARTINFO);
						break;
					case STARTINFO:
						changestate(TURN);
						break;
					case TURN:
						changestate(END);
						break;
					
					}
				}
				break;
		case W4STARTINFO:
			data = pck.receivePack();	//Recibe
			Informacion.addInfo(data);	//Guardo informacion: tiles, roles, turno
			if(data.ID == START_INFO)
				changestate(SENDACK);
			break;
		case STARTINFO:
			data = pck.startinfo(allegro);	//Manda
			Informacion.addInfo(data);	//Guarda
			pck.sendPack();
			changestate(W4ACK);
			break;
		case TURN:
			if(turn == CLIENT){
				pck.youstart();
				printAllegro(allegro,"You start");
				Informacion.myTurn = I_START;
			}
			else{
				pck.istart();
				printAllegro(allegro, "Your friend starts");
				Informacion.myTurn = YOU_START;
			}
			pck.sendPack();
			changestate(W4ACK);
			break;
		case W4TURN:
			Sleep(300);
			data = pck.receivePack();	//Recibo 
			Informacion.myTurn = data.myTurn;	//Guardoo
			if (data.ID == I_START)
				printAllegro(allegro, "You start");
			else
				printAllegro(allegro, "Your friend starts");
			pck.ack();
			pck.sendPack();
			changestate(END);
			break;
		case END:
			//al_destroy_display(allegro->startMenuDisplay);
			return;
		}	
	}
}
void FSMI::restart(ALL *allegro) {
	if (net.typeconn == SERVER) {
		data = pck.startinfo(allegro);
		Informacion.addInfo(data);
		pck.sendPack();
		pck.waitForAck(allegro);
	}
	else {
		do {
			data = pck.receivePack();
			pck.cleanBuffer();
		} while (data.ID != START_INFO);
		Informacion.addInfo(data);
		pck.ack();
		pck.sendPack();
	}
}
void FSMI::checkdata(void){
	cout << "Turn:" << Informacion.myTurn << endl;
	cout << "Rol:" << Informacion.myRol << endl;
	cout << "Name:" << Informacion.name << endl;
	cout << "My friend's name is:"<< Informacion.friendName<<endl;
	print(Informacion);
}
Container FSMI::getInfo(void){
	return Informacion;
}
void FSMI::print(Container ex){
	for(int i = 0, k = 0; i < 5; i++){ 
		for(int j = 0 ; j < 5 ; j++, k++){
			cout << ex.tiles[k] << ' ';
		}
		cout << endl;
	}
}
void FSMI::changestate(char newstate){
	prevstate = state;
	state = newstate;
}