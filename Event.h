/* 
 * File:   Event.h
 * Author: miguel
 *
 * Created on 5 de febrero de 2015, 17:00
 */

#ifndef EVENT_H
#define	EVENT_H

#include <memory>
#include <queue>
#include <limits>
#include <boost/random/mersenne_twister.hpp>

#ifndef NDEBUG
#include <iostream>
#endif

class Calendar;

class Event {
public:
    typedef long double evtime_t;
    typedef boost::mt19937 randomGen;

    virtual void process() = 0;

    evtime_t getDispatchTime() const {
        return dispatch_time;
    }

    bool operator>(const Event& ev2) const {
        return dispatch_time > ev2.dispatch_time;
    }

#ifndef NDEBUG
    virtual std::ostream& dump(std::ostream& os) const {
        os << "Time: " << getDispatchTime() << ' ';
        
        return os;
    }
#endif

protected:

    Event(evtime_t d_time);

    void newEvent(const std::shared_ptr<Event>& ev) const;
    
    randomGen& getRandomGenerator() const;

private:
    evtime_t dispatch_time;
    Calendar& calendar;
};



namespace std {

    template<>
    struct greater<shared_ptr<Event> > {

        bool operator()(const shared_ptr<Event>& a, const std::shared_ptr<Event>& b) const {
            return *a > *b;
        }
    };
}

class Calendar {
public:   
    typedef Event::randomGen randomGen;
    
    Calendar() {
    }

    void addEvent(const std::shared_ptr<Event>& ev) {
        events.push(ev);
    }

    Event::evtime_t getNextEventTime() const {
        if (events.empty())
            return std::numeric_limits<Event::evtime_t>::infinity();

        return events.top()->getDispatchTime();
    }

    std::shared_ptr<Event> getCurrentEvent(Event::evtime_t now) {
        if (now <= getNextEventTime()) {
            std::shared_ptr<Event> ev = events.top();
            events.pop();

            return ev;
        } else {
            return nullptr;
        }
    }

    static Calendar &getCalendar() {
        return calendar;
    }

    static void newEvent(const std::shared_ptr<Event>& ev) {
        getCalendar().events.push(ev);
    }

    static void newEvent(Event *ev) {
        return newEvent(std::shared_ptr<Event> (ev));
    }

    randomGen& getRandomGenerator() {
        return rng;
    }
    
    void run();

private:
    static Calendar &calendar;
    
    std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, std::greater<std::shared_ptr<Event>>> events;    
    randomGen rng;
};



#endif	/* EVENT_H */

