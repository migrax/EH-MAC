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

