/* 
 * File:   Event.cpp
 * Author: miguel
 * 
 * Created on 5 de febrero de 2015, 17:00
 */

#include "Event.h"

using namespace std;

namespace {
    Calendar mainCalendar;
}

Calendar &Calendar::calendar = mainCalendar;

Event::Event(evtime_t d_time) : calendar(Calendar::getCalendar()), dispatch_time(d_time) {
}

Event::randomGen& Event::getRandomGenerator() const {
    return calendar.getRandomGenerator();
}

void Event::newEvent(const shared_ptr<Event>& ev) const {
    return Calendar::getCalendar().newEvent(ev);
}

void Calendar::run(Event::evtime_t finish) {
    while (events.empty() == false) {
        auto ev = events.top();
        if (ev->getDispatchTime() > finish)
            break;

        ev->process();

#ifndef NDEBUG
        ev->dump(cerr);
        cerr << "Pending: " << events.size() << endl;
#endif


        events.pop();
    }
#ifndef NDEBUG    
    cerr << "Simulation ends at " << finish << 's' << endl;
#endif
}