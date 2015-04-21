/* 
 * File:   Node.cc
 * Author: miguel
 * 
 * Created on 4 de febrero de 2015, 15:56
 */

#include <algorithm>
#include <cassert>

#include "Node.h"

using namespace std;

Node::nodeid_t Node::unique_id_counter_ = 0;

Node::Node(const Location& loc) :
loc_(loc), unique_id_(++unique_id_counter_), receiving_data_(0), last_packet_id_(0), n_rtx_(0), colliding_(false), pending_beacon_(false) { // 0 is not a valid PacketId number
    
}

Node& Node::getClosestNeighbour(const Location& dest) {
    Node *best = nullptr;
    auto best_sq_distance = dest.getSquaredDistanceTo(getLocation());

    for (auto ne : neighbours_) {
        auto& neigh = ne.get();
        auto sq_distance = dest.getSquaredDistanceTo(neigh.getLocation());
        if (sq_distance < best_sq_distance) {
            best_sq_distance = sq_distance;
            best = &ne.get();
        }
    }

    /* Unreachable destination */
    if (best == nullptr) {
       throw RoutingException("Cannot find route", shared_from_this(), dest); 
    }

    return *best;
}

#ifndef NDEBUG

ostream& NodeEvent::dump(ostream& os) const {
    Event::dump(os);
    os << "Node: " << node_.getId() << ' ';

    return os;
}
#endif

void BeaconEvent::process() {
    switch (bkind_) {
        case beacon_kind_t::BEACON_START:
            /* Avoid obvious collissions
             * a) We are not receiving data, even if not directed to us
             * b) We are not transmitting data
             */
            if (node_.receiving_data_ == 0 && node_.getDriver().getStatus() != DutyDriver::status_t::RECEIVING && node_.getDriver().getStatus() != DutyDriver::status_t::TRANSMITTING) {
                auto endBEaconEvenet = node_.sendBeacon(getDispatchTime());
                if (endBEaconEvenet != nullptr)
                    newEvent(move(endBEaconEvenet));
            }
            // Generate the next beacon
            newEvent(make_unique<BeaconEvent>(node_.getDriver().scheduleRx(getDispatchTime()), node_));
            break;
        case beacon_kind_t::BEACON_END:
            node_.endListening(getDispatchTime());
            break;
    }
}

#ifndef NDEBUG

ostream& operator<<(ostream& os, const DataEvent::data_kind_t& kind) {
    switch (kind) {
        case DataEvent::data_kind_t::DATA_START:
            os << "DATA_START";
            break;
        case DataEvent::data_kind_t::DATA_END:
            os << "DATA_END";
            break;
    };
    return os;
}

ostream& DataEvent::dump(ostream& os) const {
    NodeEvent::dump(os);
    os << "Kind: DATA(" << dkind_ << ") Dst: " << dst_.getId() << ' ';

    return os;
}
#endif

void DataEvent::process() {
    switch (dkind_) {
        case data_kind_t::DATA_START:
            node_.startTransmission(dst_, getDispatchTime());

            // New DataEnd Event
            newEvent(std::make_unique<DataEvent>(getDispatchTime() + getTxTime(packet_), node_, dst_, packet_, data_kind_t::DATA_END));

            break;
        case data_kind_t::DATA_END:
            node_.endTransmission(dst_, packet_, getDispatchTime());
            break;
    }
}

std::ostream& NodeException::debug(ostream& os) const {
    SimulationException::debug(os);
    os << *node_ << endl;
    
    return os;
}

std::ostream& RoutingException::debug(ostream& os) const {
    NodeException::debug(os);    
    os << "Destination: " << dst_ << endl;

    for (auto ne : node_->neighbours_)
	os << ne.get().getId() << " " << ne.get().getLocation() << " Distance: " << ne.get().getLocation().getDistanceTo(dst_) << endl;

    cerr << "Node distance: " << node_->getLocation().getDistanceTo(dst_) << endl;
    
    return os;
}

ostream& operator<<(ostream& os, const Node& n) {
    os << "Node id: " << n.getId() << " Location: " << n.getLocation() << endl;
    os << "  Neighbours: ";
    for (auto ne : n.neighbours_) {
        os << ne.get().getId() << " ";
    }

    return os;
}
