/* 
 * File:   TrafficSource.h
 * Author: miguel
 *
 * Created on 12 de febrero de 2015, 16:57
 */

#ifndef TRAFFICSOURCE_H
#define	TRAFFICSOURCE_H

#include <memory>

#include "Node.h"


class Grid;

class TrafficSource {
public:
    virtual std::unique_ptr<Event> sendPacket(Node::nodeid_t, Node::nodeid_t, Event::evtime_t) const = 0;
    
protected:

    TrafficSource(const Grid& grid) : grid(grid) {
    }
    
    const Grid& getGrid() const {
        return grid;
    }

private:
    friend class PeriodicTrafficSourceEvent;

    const Grid& grid;
};

#endif	/* TRAFFICSOURCE_H */

