/* 
 * File:   EnhDriver.h
 * Author: miguel
 *
 * Created on 26 de febrero de 2015, 11:27
 */

#ifndef ENHDRIVER_H
#define	ENHDRIVER_H

#include "PWDriver.h"

#include <queue>
#include <cmath>

class EnhNode;

class EnhDriver : public PWDriver {
    int m_factor_;
    static const int base_factor_;
    std::queue<Event::evtime_t> next_beacons;

    const EnhNode& getEnhNode() const;

    void flushOldBeacons(Event::evtime_t now) {
        while (!next_beacons.empty() && next_beacons.front() <= now) {
            next_beacons.pop();
        }
    }

    auto arePendingBeacons(Event::evtime_t now) {
        flushOldBeacons(now);
        
        return next_beacons.empty() != true;
    }
    
public:

    EnhDriver(const EnhNode& node, Event::evtime_t bitlen);

    auto getFactor() const {
        return m_factor_ / static_cast<float> (base_factor_);
    }

    auto setFactor(float new_factor, Event::evtime_t now) {
        if (arePendingBeacons(now)) // Only change factor after a whole normal period. This is important so senders do not desynchronize themselves and expend useless time listening
            return m_factor_;
        
        m_factor_ = std::max(base_factor_, static_cast<int> (roundf(new_factor * base_factor_))); // Minimum rate is base_factor_
        return m_factor_;
    }

    virtual Event::evtime_t scheduleRx(Event::evtime_t now) {
        flushOldBeacons(now);
        if (!next_beacons.empty())
            return next_beacons.front();

        auto next_beat = now + getTimeUntilListen();
        auto period_length = next_beat - now;
        auto subinterval = period_length / static_cast<Event::evtime_t> (m_factor_);
        for (auto i = 1; i <= m_factor_; i++) {
            next_beacons.push(next_beat - period_length + i * subinterval);
        }

        return scheduleRx(now); // Now there should exist a valid beacon time
    }

    virtual Event::evtime_t getExpectedExactBeaconTime(Node::nodeid_t dst, Event::evtime_t now);

};

#endif	/* ENHDRIVER_H */

