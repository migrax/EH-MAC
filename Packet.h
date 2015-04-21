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
    using packetid_t = unsigned int ;
    
    // Caution: 0 should not ever be a valid packetId
    Packet(unsigned int src, unsigned int dst, Location dst_loc) : uniqueId(++uniqueIdCounter), size(128), src(src), dst(dst), dst_loc(dst_loc) {}
    
    auto getDestination() const {
        return dst;
    }
    
    auto getOrigin() const {
        return src;
    }
    
    auto getFinalLocation() const {
        return dst_loc;
    }
    
    auto getPSize() const {
        return size;
    }
    
    auto getId() const {
        return uniqueId;
    }
    
private:
    static packetid_t uniqueIdCounter;
    
    const packetid_t uniqueId;
    const unsigned int size; // Maximum size of IEEE 802.15.4 is 128 bytes
    const unsigned int src, dst;
    const Location dst_loc;
};

#endif	/* PACKET_H */

