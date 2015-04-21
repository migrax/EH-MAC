/* 
 * File:   PeriodicTrafficSource.h
 * Author: miguel
 *
 * Created on 12 de febrero de 2015, 17:00
 */

#ifndef PERIODICTRAFFICSOURCE_H
#define	PERIODICTRAFFICSOURCE_H

#include "TrafficSource.h"

/* A deterministic traffic source that send traffic to nodeId=1 */
class PeriodicTrafficSource : public TrafficSource {
public:
    PeriodicTrafficSource(const Grid& grid, double rate); // In pkts./s

    virtual std::unique_ptr<Event> sendPacket(Node::nodeid_t, Node::nodeid_t, Event::evtime_t) const;
    
private:
    const double rate_;
    
    Event::evtime_t getTimeToNextPacket() const;
};

#endif	/* PERIODICTRAFFICSOURCE_H */

