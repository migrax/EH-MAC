/* 
 * File:   PWDriver.cpp
 * Author: miguel
 * 
 * Created on 24 de febrero de 2015, 10:37
 */

#include "PWDriver.h"

#include "Event.h"
#include "Node.h"

#include <boost/random/uniform_int_distribution.hpp>

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

/* In PW-MAC we sleep between ½ sleepLength and 1.5 sleepLength */
Event::evtime_t PWDriver::getTimeUntilListen() {
    return sleep_length_ / 2. + rng_() / (1. * rng_.randMax());
}

void PWDriver::scheduleListen(Node::nodeid_t dst, Event::evtime_t now) {
    if (getExpectedBeaconTime(dst, 0) > now) // Hack: there is a current prediction for the future
        return;

    Calendar::newEvent(std::make_unique<PWDriverEvent>(getExpectedBeaconTime(dst, now), *this));
}

Event::evtime_t PWDriver::scheduleTxSlowPath(Event::evtime_t now, int backoff) const {
    Calendar::randomGen_t& rng = Calendar::getCalendar().getRandomGenerator();

    boost::random::uniform_int_distribution<int> distr(0, backoff);

    return now + getBitLen() * distr(rng);
}