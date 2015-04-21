/* 
 * File:   Link.h
 * Author: miguel
 *
 * Created on 5 de febrero de 2015, 12:57
 */

#ifndef LINK_H
#define	LINK_H

#include "Packet.h"

#include <memory>
#include <queue>

#include <cassert>

class Node;

class Link {
public:
    Link(Node& dst) : dst(dst) {}
    
    auto queuePacket(const Packet& p) {
        packetQueue.push(p);
        
        return packetQueue.size();
    }
    
    auto getQueueSize() const {
        return packetQueue.size();
    }
    
    const Packet& getNextPacket() const {
        assert(packetQueue.empty() == false);
        
        return packetQueue.front();
    }
    
    auto removePacket() {
        assert(packetQueue.empty() == false);
        
        packetQueue.pop();
        
        return getQueueSize();
    }
    
    static auto getCapacity() {
        return static_cast<double> (capacity);
    }        
    
private:
    static const unsigned int capacity; // 250 kbit/s is the current max. capacity of 802.15.4 physical layers.
    Node& dst;
    std::queue<Packet> packetQueue;
};

#endif	/* LINK_H */

