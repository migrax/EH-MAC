/* 
 * File:   PeriodicTrafficSource.cpp
 * Author: miguel
 * 
 * Created on 12 de febrero de 2015, 17:00
 */

#include "PeriodicTrafficSource.h"

#include "Grid.h"
#include "Event.h"
#include <random>

using namespace std;

namespace {

    class PeriodicTrafficSourceEvent : public Event {
    public:

        PeriodicTrafficSourceEvent(const PeriodicTrafficSource& periodic_source, Node::nodeid_t src, Node::nodeid_t dst, Event::evtime_t dispatch_time) : Event(dispatch_time), periodic_traffic_source_(periodic_source), src_(src), dst(dst) {            
        }

        virtual void process() {
            newEvent(periodic_traffic_source_.sendPacket(src_, dst, getDispatchTime()));
        }

#ifndef NDEBUG

        virtual ostream& dump(ostream& os) const {
            Event::dump(os);

            os << "Kind: PeriodicTraffic " << " Src: " << src_ << " Dst: " << dst << ' ';

            return os;
        }
#endif

    private:
        const PeriodicTrafficSource& periodic_traffic_source_;
        const Node::nodeid_t src_, dst;
    };
}

PeriodicTrafficSource::PeriodicTrafficSource(const Grid& grid, double rate) : TrafficSource(grid), rate_(rate) {
    auto nodes = grid.countDeployedNodes();
    auto dist = uniform_real_distribution<Event::evtime_t> (0, 1./rate);
    auto& rng = Calendar::getCalendar().getRandomGenerator();
    
    for (unsigned i = 2; i <= nodes; i++) {// The first node does not send traffic        
        auto start_time = dist(rng);
        Calendar::getCalendar().newEvent(make_unique<PeriodicTrafficSourceEvent>(*this, i, 1, start_time));
    }
}

Event::evtime_t PeriodicTrafficSource::getTimeToNextPacket() const {
    return 1./rate_;
}

unique_ptr<Event> PeriodicTrafficSource::sendPacket(Node::nodeid_t src, Node::nodeid_t dst, Event::evtime_t now) const {    
    Node& src_node = getGrid().getNodeById(src);

    src_node.Route(Packet(src, dst, getGrid().getNodeLocation(dst), now), now);
    return make_unique<PeriodicTrafficSourceEvent>(*this, src, dst, now + getTimeToNextPacket());   
}
