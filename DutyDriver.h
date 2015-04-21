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

class Node;

class DutyDriver {
public:

    enum class status_t {
        SLEEPING, LISTENING, RECEIVING, TRANSMITTING
    };

    void powerOff(); // No data to send

    auto getStatus() const {
        return curStatus;
    }

    virtual void newData(unsigned int nextHop, Event::evtime_t now) {
        activeHops_.insert(nextHop);
    }

    virtual void emptyQueue(unsigned int nextHop) {
        assert(activeHops_.find(nextHop) != activeHops_.end());

        activeHops_.erase(nextHop);
    }

    virtual Event::evtime_t scheduleTx(Event::evtime_t, int backoff) const = 0;

    virtual Event::evtime_t scheduleRx(Event::evtime_t) = 0;

    void setStatus(status_t newStatus, Event::evtime_t now) {
        curStatus = newStatus;
        
        logStatus(now);        
    }

    virtual void setIdlestMode(Event::evtime_t) = 0;
    
    auto& getNode() const {
        return node_;
    }

protected:

    DutyDriver(const Node& node, Event::evtime_t bitlen) : curStatus(status_t::SLEEPING), bitlen_(bitlen), node_(node) {}

    auto getBitLen() const {
        return bitlen_;
    }

    auto getActiveQueues() const {
        return activeHops_.size();
    }

    static const double sleep_length_;
    
private:
    status_t curStatus;
    std::set<unsigned int> activeHops_;
    const Event::evtime_t bitlen_;
    const Node& node_;

    virtual Event::evtime_t getTimeUntilListen() = 0;
    void logStatus(Event::evtime_t now) const;
};

#endif	/* DUTYDRIVER_H */

