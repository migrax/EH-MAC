/* 
 * File:   RIDriver.h
 * Author: miguel
 *
 * Created on 10 de febrero de 2015, 13:35
 */

#ifndef RIDRIVER_H
#define	RIDRIVER_H

#include "DutyDriver.h"
#include "Event.h"

/* Based on RI-MAC: 
 * Nodes enter listening whenever have data to transmit and flush their queues
 * Receivers enter a fixed sleep cycle of length L after each reception (random between ½ and 1½L)
 * We ignore the idle time after beacon transmission
 
 */

class RIDriver : virtual public DutyDriver {
private:    
    virtual Event::evtime_t getTimeUntilListen();
    Event::evtime_t scheduleTxSlowPath(Event::evtime_t now, int backoff) const;

public:

    RIDriver(const Node& node, Event::evtime_t bitlen) : DutyDriver(node, bitlen) {
    };

    virtual void newData(unsigned int next_hop, Event::evtime_t now) {
        DutyDriver::newData(next_hop, now);

        if (getStatus() == status_t::SLEEPING)
            setIdlestMode(now);
    }

    virtual Event::evtime_t scheduleTx(Event::evtime_t now, int backoff) const {
        if (backoff == 0)
            return now;

        return scheduleTxSlowPath(now, backoff);
    }

    virtual Event::evtime_t scheduleRx(Event::evtime_t now) {
        return now + getTimeUntilListen();
    }

    virtual void setIdlestMode(Event::evtime_t now) {
        if (getActiveQueues()) {
            setStatus(status_t::LISTENING, now);
        } else
            setStatus(status_t::SLEEPING, now);
    }
};

#endif	/* RIDRIVER_H */

