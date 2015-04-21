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

