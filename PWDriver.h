/* 
 * File:   PWDriver.h
 * Author: miguel
 *
 * Created on 24 de febrero de 2015, 10:37
 */

#ifndef PWDRIVER_H
#define	PWDRIVER_H

#include "DutyDriver.h"
#include "Event.h"
#include "Node.h"

#include <map>
#include <tuple>

/* Based on PW-MAC: 
 * Nodes wait for listening until predicted wake-up time of the receiver, if known
 * Receivers enter a fixed sleep cycle of length L after each reception (random between ½ and 1½L)
 * Each receiver uses a different rng
 * We ignore the idle time after beacon transmission
 *
 */

class PWNode;

class PWDriver : public DutyDriver {
private:
    std::map<Node::nodeid_t, std::tuple<Event::evtime_t, unsigned int>> predictions;
    Event::evtime_t scheduleTxSlowPath(Event::evtime_t now, int backoff) const;

    void scheduleListen(Node::nodeid_t dst, Event::evtime_t now);

protected:

    class PseudoRNG {
        const unsigned int a_, c_, m_;
        unsigned int x_;

    public:

        PseudoRNG(unsigned int seed) : a_(1103515245), c_(12345), m_(1 << 31), x_(seed) {
        }

        auto getCurrentValue() const {
            return x_;
        }

        auto operator()() {
            x_ = (a_ * x_ + c_) % m_;
            return x_;
        }

        auto randMax() const {
            return m_;
        }

    } rng_;

    virtual Event::evtime_t getExpectedExactBeaconTime(Node::nodeid_t dst, Event::evtime_t now);
    Event::evtime_t getTimeUntilListen();

    Event::evtime_t getExpectedBeaconTime(Node::nodeid_t dst, Event::evtime_t now) {
        auto beacon_time = getExpectedExactBeaconTime(dst, now);
        return std::max(now, beacon_time - 10e-3); // 10ms error máx
    }

    Event::evtime_t getPrevBeaconTime(Node::nodeid_t dst, Event::evtime_t next_beacon);

public:

    PWDriver(const Node& node, Event::evtime_t bitlen);

    virtual void newData(unsigned int next_hop, Event::evtime_t now) {
        DutyDriver::newData(next_hop, now);

        scheduleListen(next_hop, now);
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
        // We wake-up ourselves when appropriate        
        setStatus(status_t::SLEEPING, now);
    }
};

#endif	/* PWDRIVER_H */

