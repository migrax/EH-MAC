/*
 * Copyright (C) 2015 Miguel Rodríguez Pérez <miguel@det.uvigo.es>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

