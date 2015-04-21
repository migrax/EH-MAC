/* 
 * File:   Packet.cpp
 * Author: miguel
 * 
 * Created on 5 de febrero de 2015, 11:51
 */

#include "Packet.h"

using namespace std;

Packet::packetid_t Packet::uniqueIdCounter_ = 0;

ostream& operator<<(ostream& os, const Packet& p) {
    os << p.getId() << " (" << p.getOrigin() << " → " << p.getDestination() << ")";
    
    return os;
}