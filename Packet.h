/* 
 * File:   Packet.h
 * Author: miguel
 *
 * Created on 5 de febrero de 2015, 11:51
 */

#ifndef PACKET_H
#define	PACKET_H

#include "Location.h"
#include "Event.h"

#include <ostream>

class Packet {
public:
    using packetid_t = unsigned int;
    
    // Caution: 0 should not ever be a valid packetId
    Packet(unsigned int src, unsigned int dst, Location dst_loc, Event::evtime_t now) : 
    uniqueId_(++uniqueIdCounter_), size_(getMaxPacketSize()), src_(src), dst_(dst), dst_loc_(dst_loc), creationTime_(now) {}
    
    auto getDestination() const {
        return dst_;
    }
    
    auto getOrigin() const {
        return src_;
    }
    
    auto getFinalLocation() const {
        return dst_loc_;
    }
    
    auto getPSize() const {
        return size_;
    }
    
    auto getId() const {
        return uniqueId_;
    }
    
    auto getCreationTime() const {
        return creationTime_;
    }
    
    static int getMaxPacketSize() {
        return 128;
    }
    
    static auto getCounter() {
        return uniqueIdCounter_;
    }
    
private:
    static packetid_t uniqueIdCounter_;
    
    const packetid_t uniqueId_;
    const unsigned int size_; // Maximum size of IEEE 802.15.4 is 128 bytes
    const unsigned int src_, dst_;
    const Location dst_loc_;
    const Event::evtime_t creationTime_;
};

std::ostream& operator<<(std::ostream& os, const Packet& p);

#endif	/* PACKET_H */

