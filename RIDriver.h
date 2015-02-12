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
public:

    RIDriver() : DutyDriver() {
    };

    virtual void newData(unsigned int nextHop) {
        DutyDriver::newData(nextHop);
        
        if (getStatus() == status_t::SLEEPING)
            setStatus(status_t::LISTENING);
    }
        
    virtual Event::evtime_t scheduleTx(Event::evtime_t now) {
        return now; // We do not wait to start txing
    }

    virtual Event::evtime_t scheduleRx(Event::evtime_t now) {
        return now + getTimeUntilListen();
    }

private:
    static const double sleepLength;

    virtual Event::evtime_t getTimeUntilListen();
};

#endif	/* RIDRIVER_H */

