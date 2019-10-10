#pragma once
#include <iostream>
using namespace std;

class mac_time {
public:
	string MAC;
	string timestamp;
    mac_time(char* MAC,char* timestamp){
        this->MAC=MAC;
        this->timestamp= timestamp;
    }
    mac_time(const char* MAC, const char* timestamp){
        this->MAC = MAC;
        this->timestamp = timestamp;
    }

    bool operator == (const mac_time &r) const {
        if (this->MAC == r.MAC &&  this->timestamp == r.timestamp)
			return true;
		else
			return false;
	}

    bool operator <(const mac_time &r) const {
        if (this->MAC< r.MAC) {
            return true;
        } else {
			if(this->MAC== r.MAC && this->timestamp< r.timestamp)
			return true;
			else return false;
		}
	}
		
    friend ostream& operator << (ostream& os, const mac_time &dt)
		{
			os <<  dt.MAC.c_str()<< " - " << dt.timestamp.c_str();
			return os;
		}
};
