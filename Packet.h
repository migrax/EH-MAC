/* 
 * File:   Packet.h
 * Author: miguel
 *
 * Created on 5 de febrero de 2015, 11:51
 */

#ifndef PACKET_H
#define	PACKET_H

#include "Location.h"

class Packet {
public:
    Packet(unsigned int src, unsigned int dst, Location dst_loc) : size(128), src(src), dst(dst), dst_loc(dst_loc) {}
    
    unsigned int getDestination() const {
        return dst;
    }
    
    unsigned int getOrigin() const {
        return src;
    }
    
    const Location& getFinalLocation() const {
        return dst_loc;
    }
    
    const unsigned int getPSize() const {
        return size;
    }
    
private:
    const unsigned int size; // Maximum size of IEEE 802.15.4 is 128 bytes
    const unsigned int src, dst;
    const Location dst_loc;
};

#endif	/* PACKET_H */

