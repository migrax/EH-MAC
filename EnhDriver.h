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


#ifndef ENHDRIVER_H
#define	ENHDRIVER_H

#include "PWDriver.h"

#include <queue>
#include <cmath>

class EnhNode;

class EnhDriver : public PWDriver {
    float max_beacon_rate_; // Max_rate in beacons/s
    float beacon_rate_;

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

    float calculateOptimumBeaconRate(float data_rate) const noexcept {
        const auto et = 8 * 5 / Link::getCapacity(); // Beacon transmission time. FIXME: Share a var with Node.h
        const auto epkt = 8 * Packet::getMaxPacketSize() / Link::getCapacity(); // Packet transmission time
        const auto eb = 10e-3; // Beacon listening time

        auto optimum_rate = ((2 * eb + 2 * epkt - et - M_E * et) * data_rate) / (eb + epkt + et);
        return optimum_rate;
    }

public:

    EnhDriver(const EnhNode& node, Event::evtime_t bitlen, float max_rate = 10);

    auto getBeaconRate() const {
        return beacon_rate_;
    }

    auto setBeaconRate(float estimated_data_rate, Event::evtime_t now) {
        if (arePendingBeacons(now)) // Only change factor after a whole normal period. This is important so senders do not desynchronize themselves and expend useless time listening
            return beacon_rate_;

        auto new_rate = calculateOptimumBeaconRate(estimated_data_rate);

        beacon_rate_ = std::min(new_rate, max_beacon_rate_);
        return beacon_rate_;
    }

    virtual Event::evtime_t scheduleRx(Event::evtime_t now) {
        flushOldBeacons(now);
        if (!next_beacons.empty())
            return next_beacons.front();

        const auto next_beat = now + getTimeUntilListen();
        const auto period_length = next_beat - now;
        const auto subinterval = 1 / (1. * max_beacon_rate_);
        /* We generate all possible beacons and then choose randomly which ones to 
         * observe. As the random seed we employ node_id<<16 + floor(now). That is
         * random enough and, at the same time, known by senders */
        auto rng = PseudoRNG((getNode().getId() << 16) + static_cast<int> (floor(now)));
        const auto threshold = getBeaconRate() / max_beacon_rate_ * rng.randMax();

        for (auto i = next_beat - period_length + subinterval; i < next_beat; i += subinterval)
            if (rng() <= threshold)
                next_beacons.push(i);

        next_beacons.push(next_beat); // This is the noral beacon that MUST be transmitted                

        return scheduleRx(now); // Now there should exist a valid beacon time
    }

    virtual Event::evtime_t getExpectedExactBeaconTime(Node::nodeid_t dst, Event::evtime_t now);

};

#endif	/* ENHDRIVER_H */

