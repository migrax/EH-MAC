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

#include "PWDriver.h"

#include "Event.h"
#include "PWNode.h"

#include <random>

#ifndef NDEBUG
#include <ostream>
#endif

namespace {

    class PWDriverEvent : public Event {
        PWDriver& driver_;

    public:

        PWDriverEvent(Event::evtime_t listen, PWDriver& driver) : Event(listen), driver_(driver) {
        }

        virtual void process() {
            driver_.setStatus(DutyDriver::status_t::LISTENING, getDispatchTime());
        }

#ifndef NDEBUG

        virtual std::ostream& dump(std::ostream& os) const {
            Event::dump(os);
            os << "PWDriver(Listen): " << driver_.getNode().getId() << ' ';

            return os;
        }
#endif

    };
}

PWDriver::PWDriver(const Node& node, Event::evtime_t bitlen) : DutyDriver(node, bitlen), rng_(node.getId()) {
};

/* In PW-MAC we sleep between ½ sleepLength and 1.5 sleepLength */
Event::evtime_t PWDriver::getTimeUntilListen() {
    return sleep_length_ / 2. + sleep_length_ * (rng_() / (1. * rng_.randMax()));
}

void PWDriver::scheduleListen(Node::nodeid_t dst, Event::evtime_t now) {
    if (getExpectedBeaconTime(dst, 0) > now) // Hack: there is a current prediction for the future
        return;

    const auto fire_time = getExpectedBeaconTime(dst, now);
    Calendar::newEvent(std::make_unique<PWDriverEvent>(fire_time, *this));
#ifndef NDEBUG
    std::cerr << "Node " << getNode().getId() << " schedules wake up at " << fire_time << " with destination " << dst << std::endl;
#endif
}

Event::evtime_t PWDriver::scheduleTxSlowPath(Event::evtime_t now, int backoff) const {
    Calendar::randomGen_t& rng = Calendar::getCalendar().getRandomGenerator();

    std::uniform_int_distribution<int> distr(0, backoff);

    return now + getBitLen() * distr(rng);
}

Event::evtime_t PWDriver::getExpectedExactBeaconTime(Node::nodeid_t dst, Event::evtime_t now) {
    Event::evtime_t last_beacon = 0;
    int last_seed = dst;

    if (predictions.find(dst) != predictions.end()) {
        std::tie(last_beacon, last_seed) = predictions[dst];
    }

    auto rng = PseudoRNG(last_seed);
    while (last_beacon < now) {
        last_beacon += sleep_length_ / 2. + sleep_length_ * (rng() / (1. * rng_.randMax()));
        predictions[dst] = std::make_tuple(last_beacon, rng.getCurrentValue());
    }

    return last_beacon;
}

Event::evtime_t PWDriver::getPrevBeaconTime(Node::nodeid_t dst, Event::evtime_t next_beacon) {
    if(predictions.find(dst) == predictions.end()) {
        return 0;
    }

    Event::evtime_t last_beacon;
    int last_seed;

    std::tie(last_beacon, last_seed) = predictions[dst];

    auto prev_beacon_delta = sleep_length_ * last_seed / (1. * rng_.randMax()) + sleep_length_ / 2.;

    return next_beacon - prev_beacon_delta;
}