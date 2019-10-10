#include "schema_original.h"
#include<string>


schema_original::schema_original(char* ID, char* MAC,char* isPub,char* RSSI,char* timestamp,char*root) {
	this->ID = atoi(ID);
	this->MAC= MAC;	
	this->isPub = atoi(isPub);
	this->RSSI = atoi(RSSI);
	this->timestamp=timestamp;
	this->root =root;

}
