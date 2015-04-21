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


#ifndef POISSONTRAFFICSOURCE_H
#define	POISSONTRAFFICSOURCE_H

#include "TrafficSource.h"

/* A Poisson traffic source that send traffic to nodeId=1 */
class PoissonTrafficSource : public TrafficSource {
public:
    PoissonTrafficSource(const Grid& grid, double rate); // In pkts./s

    virtual std::unique_ptr<Event> sendPacket(Node::nodeid_t, Node::nodeid_t, Event::evtime_t) const;
    
private:
    const double rate_;
    
    Event::evtime_t getTimeToNextPacket() const;
};

#endif	/* POISSONTRAFFICSOURCE_H */

