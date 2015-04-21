/* 
 * File:   Event.cpp
 * Author: miguel
 * 
 * Created on 5 de febrero de 2015, 17:00
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