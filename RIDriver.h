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

