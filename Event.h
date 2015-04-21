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
#include <string>
#include <random>
#include <ostream>

#ifndef NDEBUG
#include <iostream>
#endif

class Calendar;

class SimulationException {
protected:
    const std::string message;
    
public:
    SimulationException(std::string message) : message(message) {}
    
    auto getMessage() const {
        return message;
    }
    
    virtual std::ostream& debug(std::ostream& os) const {
        return os;
    }
};

class Event {
public:
    using evtime_t = long double;
    using randomGen_t = std::mt19937;        
        
    virtual void process() = 0;

    auto getDispatchTime() const {
        return dispatch_time_;
    }

    auto operator>(const Event& ev2) const {
        return dispatch_time_ > ev2.dispatch_time_;
    }

#ifndef NDEBUG

    virtual std::ostream& dump(std::ostream& os) const {
        os << "Time: " << getDispatchTime() << ' ';

        return os;
    }
#endif

protected:

    Event(evtime_t d_time);

    void newEvent(std::unique_ptr<Event> ev) const;

    randomGen_t& getRandomGenerator() const;

private:
    Calendar& calendar_;
    evtime_t dispatch_time_;
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
    using randomGen_t = Event::randomGen_t;    

    Calendar() {
    }

    void setRandomSeed(int seed) {
        rng = randomGen_t(seed);
    }
    
    void addEvent(std::unique_ptr<Event> ev) {        
        events.push(std::move(ev));
    }

    auto getNextEventTime() const {
        if (events.empty())
            return std::numeric_limits<Event::evtime_t>::infinity();

        return events.top()->getDispatchTime();
    }

    auto getCurrentEvent(Event::evtime_t now) {
        if (now <= getNextEventTime()) {
            auto ev = events.top();
            events.pop();

            return ev.get();
        } else {
            return static_cast<Event *> (nullptr);
        }
    }

    static Calendar& getCalendar() {
        if (calendar == nullptr)
            calendar = std::make_unique<Calendar>();
        
        return *calendar;
    }

    static void newEvent(std::unique_ptr<Event> ev) {        
        getCalendar().addEvent(std::move(ev));
    }

    randomGen_t& getRandomGenerator() {
        return rng;
    }

    void run(Event::evtime_t finish = std::numeric_limits<Event::evtime_t>::infinity());

private:
    static std::unique_ptr<Calendar> calendar;
    Calendar(const Calendar&) {}; // Disable copy constructor

    std::priority_queue<std::shared_ptr<Event>, std::vector<std::shared_ptr<Event>>, std::greater<std::shared_ptr<Event>>> events;
    randomGen_t rng;
};



#endif	/* EVENT_H */

