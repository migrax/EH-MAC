/* 
 * File:   PoissonTrafficSource.h
 * Author: miguel
 *
 * Created on 3 de marzo de 2015, 18:50
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

