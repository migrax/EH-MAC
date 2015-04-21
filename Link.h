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
    Link(Node&) {}
    
    auto queuePacket(const Packet& p) {
        packet_queue_.push(p);
        
        return packet_queue_.size();
    }
    
    auto getQueueSize() const {
        return packet_queue_.size();
    }
    
    const Packet& getNextPacket() const {
        assert(packet_queue_.empty() == false);
        
        return packet_queue_.front();
    }
    
    auto removePacket() {
        assert(packet_queue_.empty() == false);
        
        packet_queue_.pop();
        
        return getQueueSize();
    }
    
    static double getCapacity() {
        return capacity_;
    }        
    
private:
    static const unsigned int capacity_; // 250 kbit/s is the current max. capacity of 802.15.4 physical layers.
    std::queue<Packet> packet_queue_;
};

#endif	/* LINK_H */

