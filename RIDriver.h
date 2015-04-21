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
    static const double sleepLength;

    virtual Event::evtime_t getTimeUntilListen() const;
    Event::evtime_t scheduleTxSlowPath(Event::evtime_t now, int backoff) const;
    
public:

    RIDriver(Event::evtime_t bytelen) : DutyDriver(bytelen) {
    };

    virtual void newData(unsigned int nextHop) {
        DutyDriver::newData(nextHop);
    }

    virtual Event::evtime_t scheduleTx(Event::evtime_t now, int backoff) const {
        if (backoff == 0)
            return now;

        return scheduleTxSlowPath(now, backoff);
    }

    virtual Event::evtime_t scheduleRx(Event::evtime_t now) const {
        return now + getTimeUntilListen();
    }

    virtual void setIdlestMode() {
        if (getActiveQueues()) {
            setStatus(status_t::LISTENING);
        } else
            setStatus(status_t::SLEEPING);
    }
};

#endif	/* RIDRIVER_H */

