/* 
 * File:   DutyDriver.h
 * Author: miguel
 *
 * Created on 10 de febrero de 2015, 13:31
 */

#ifndef DUTYDRIVER_H
#define	DUTYDRIVER_H

#include "Event.h"

#include <set>
#include <cassert>

class DutyDriver {
public:

    enum class status_t {
        SLEEPING, LISTENING, RECEIVING, TRANSMITTING
    };

    void powerOff(); // No data to send

    virtual void newData(unsigned int nextHop) {
        activeHops.insert(nextHop);
    }

    virtual void emptyQueue(unsigned int nextHop) {
        assert(activeHops.find(nextHop) != activeHops.end());
        
        activeHops.erase(nextHop);
    }
    
    virtual Event::evtime_t scheduleTx(Event::evtime_t) = 0;
    
    virtual Event::evtime_t scheduleRx(Event::evtime_t) = 0;
    
    status_t getStatus() {
        return curStatus;
    }

    void setStatus(status_t newStatus) {
        curStatus = newStatus;
    }

protected:

    DutyDriver() : curStatus(status_t::SLEEPING) {
    }

private:
    status_t curStatus;
    std::set<unsigned int> activeHops;

    virtual Event::evtime_t getTimeUntilListen() = 0;
};

#endif	/* DUTYDRIVER_H */

