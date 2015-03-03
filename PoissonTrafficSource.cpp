/* 
 * File:   PoissonTrafficSource.cpp
 * Author: miguel
 * 
 * Created on 3 de marzo de 2015, 18:50
 */

#include "PoissonTrafficSource.h"

#include "Grid.h"
#include "Event.h"

#include <boost/random/exponential_distribution.hpp>

using namespace std;

namespace {

    class PoissonTrafficSourceEvent : public Event {
    public:

        PoissonTrafficSourceEvent(const PoissonTrafficSource& poisson_source, Node::nodeid_t src, Node::nodeid_t dst, Event::evtime_t dispatch_time) : Event(dispatch_time), poisson_traffic_source_(poisson_source), src_(src), dst(dst) {            
        }

        virtual void process() {
            newEvent(poisson_traffic_source_.sendPacket(src_, dst, getDispatchTime()));
        }

#ifndef NDEBUG

        virtual ostream& dump(ostream& os) const {
            Event::dump(os);

            os << "Kind: PoissonTraffic " << " Src: " << src_ << " Dst: " << dst << ' ';

            return os;
        }
#endif

    private:
        const PoissonTrafficSource& poisson_traffic_source_;
        const Node::nodeid_t src_, dst;
    };
}

PoissonTrafficSource::PoissonTrafficSource(const Grid& grid, double rate) : TrafficSource(grid), rate_(rate) {
    auto nodes = grid.countDeployedNodes();

    for (unsigned i = 2; i <= nodes; i++) {// The first node does not send traffic        
        Calendar::getCalendar().newEvent(make_unique<PoissonTrafficSourceEvent>(*this, i, 1, getTimeToNextPacket()));
    }
}

Event::evtime_t PoissonTrafficSource::getTimeToNextPacket() const {
    boost::random::exponential_distribution<Event::evtime_t> exp_distr(rate_);
    
    return exp_distr(Calendar::getCalendar().getRandomGenerator());
}

unique_ptr<Event> PoissonTrafficSource::sendPacket(Node::nodeid_t src, Node::nodeid_t dst, Event::evtime_t now) const {
    Node& src_node = getGrid().getNodeById(src);

    src_node.Route(Packet(src, dst, getGrid().getNodeLocation(dst), now), now);
    return make_unique<PoissonTrafficSourceEvent>(*this, src, dst, now + getTimeToNextPacket());   
}
