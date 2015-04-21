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
    
    const Node& getNode() const {
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

    static double sleep_length_;
    
private:
    status_t curStatus;
    std::set<unsigned int> activeHops_;
    const Event::evtime_t bitlen_;
    const Node& node_;

    virtual Event::evtime_t getTimeUntilListen() = 0;
    void logStatus(Event::evtime_t now) const;
};

#endif	/* DUTYDRIVER_H */

