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

    auto getStatus() const {
        return curStatus;
    }

    virtual void newData(unsigned int nextHop) {
        activeHops.insert(nextHop);

        if (getStatus() == status_t::SLEEPING)
            setIdlestMode();
    }

    virtual void emptyQueue(unsigned int nextHop) {
        assert(activeHops.find(nextHop) != activeHops.end());

        activeHops.erase(nextHop);
    }

    virtual Event::evtime_t scheduleTx(Event::evtime_t, int backoff) const = 0;

    virtual Event::evtime_t scheduleRx(Event::evtime_t) const = 0;

    void setStatus(status_t newStatus) {
        curStatus = newStatus;
    }

    virtual void setIdlestMode() = 0;

protected:

    DutyDriver(Event::evtime_t bytelen) : curStatus(status_t::SLEEPING) {
    }

    auto getByteLen() const {
        return bytelen;
    }

    auto getActiveQueues() const {
        return activeHops.size();
    }

private:
    status_t curStatus;
    std::set<unsigned int> activeHops;
    Event::evtime_t bytelen;

    virtual Event::evtime_t getTimeUntilListen() const = 0;
};

#endif	/* DUTYDRIVER_H */

