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


#include "RIDriver.h"
#include "Event.h"

#include <random>

/* In RI-MAC we sleep between ½ sleepLength and 1.5 sleepLength*/
Event::evtime_t RIDriver::getTimeUntilListen() {
    Calendar::randomGen_t& rng = Calendar::getCalendar().getRandomGenerator();

    std::uniform_real_distribution<Event::evtime_t> distr(-0.5, 0.5);

    return sleep_length_ + distr(rng);
}

Event::evtime_t RIDriver::scheduleTxSlowPath(Event::evtime_t now, int backoff) const {
    Calendar::randomGen_t& rng = Calendar::getCalendar().getRandomGenerator();

    std::uniform_int_distribution<int> distr(0, backoff);

    return now + getBitLen() * distr(rng);
}
