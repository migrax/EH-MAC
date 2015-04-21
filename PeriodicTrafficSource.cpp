/* 
 * File:   PeriodicTrafficSource.cpp
 * Author: miguel
 * 
 * Created on 12 de febrero de 2015, 17:00
 */

#include "PeriodicTrafficSource.h"

#include "Grid.h"
#include "Event.h"

using namespace std;

namespace {

    class PeriodicTrafficSourceEvent : public Event {
    public:

        PeriodicTrafficSourceEvent(const PeriodicTrafficSource& pSource, Node::nodeid_t src, Node::nodeid_t dst, Event::evtime_t dispatchTime) : Event(dispatchTime), pSource(pSource), src(src), dst(dst) {            
        }

        virtual void process() {
            newEvent(pSource.sendPacket(src, dst, getDispatchTime()));
        }

#ifndef NDEBUG

        virtual ostream& dump(ostream& os) const {
            Event::dump(os);

            os << "Kind: PeriodicTraffic " << " Src: " << src << " Dst: " << dst << ' ';

            return os;
        }
#endif

    private:
        const PeriodicTrafficSource& pSource;
        const Node::nodeid_t src, dst;
    };
}

PeriodicTrafficSource::PeriodicTrafficSource(const Grid& grid, double rate) : TrafficSource(grid), rate(rate) {
    auto nodes = grid.countDeployedNodes();

    for (unsigned i = 2; i <= nodes; i++) {// The first node does not send traffic        
        Calendar::getCalendar().newEvent(make_unique<PeriodicTrafficSourceEvent>(*this, i, 1, getTimeToNextPacket()));
    }
}

Event::evtime_t PeriodicTrafficSource::getTimeToNextPacket() const {
    return 1./rate_;
}

unique_ptr<Event> PeriodicTrafficSource::sendPacket(Node::nodeid_t src, Node::nodeid_t dst, Event::evtime_t now) const {
    Node& srcNode = getGrid().getNodeById(src);

    srcNode.Route(Packet(src, dst, getGrid().getNodeLocation(dst)));
    return make_unique<PeriodicTrafficSourceEvent>(*this, src, dst, now + getTimeToNextPacket());   
}
