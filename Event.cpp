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


#include "Event.h"

using namespace std;

std::unique_ptr<Calendar> Calendar::calendar(nullptr);

Event::Event(evtime_t d_time) : calendar_(Calendar::getCalendar()), dispatch_time_(d_time) {
}

Event::randomGen_t& Event::getRandomGenerator() const {
    return calendar_.getRandomGenerator();
}

void Event::newEvent(unique_ptr<Event> ev) const {
    return Calendar::getCalendar().newEvent(move(ev));
}

void Calendar::run(Event::evtime_t finish) {
    while (events.empty() == false) {
        auto ev = events.top();
        if (ev->getDispatchTime() > finish)
            break;

#ifndef NDEBUG
        ev->dump(cerr);
        cerr << "Pending: " << events.size() << endl;
#endif
        ev->process();
#ifndef NDEBUG
        cerr << " Done!" << endl;
#endif

        events.pop();

    }
#ifndef NDEBUG    
    cerr << "Simulation ends at " << finish << 's' << endl;
#endif
}