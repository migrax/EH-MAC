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

Node::nodeid_t Node::uniqueIdCounter = 0;

Node::Node(const Location& loc, shared_ptr<DutyDriver> driver) : loc(loc), driver(driver), uniqueId(++uniqueIdCounter) {
    // Start the beacons!
    Calendar::newEvent(new BeaconEvent(driver->scheduleRx(0), shared_ptr<Node> (this)));
}

shared_ptr<Node> Node::getClosestNeighbour(const Location& dest) const {
    shared_ptr<Node> best = nullptr;
    double best_sq_distance = dest.getSquaredDistanceTo(getLocation());

    for (auto ne : neighbours) {
        double sq_distance = dest.getSquaredDistanceTo(ne->getLocation());
        if (sq_distance < best_sq_distance) {
            best_sq_distance = sq_distance;
            best = ne;
        }
    }

    /* Unreachable destination */    
    assert(best != nullptr);

    return best;
}

#ifndef NDEBUG

ostream& NodeEvent::dump(ostream& os) const {
    Event::dump(os);
    os << "Node: " << node->getId() << ' ';

    return os;
}
#endif

void BeaconEvent::process() {
    if (bkind == beacon_kind_t::BEACON_START) {
        // Avoid obvious collissions
        if (node->driver->getStatus() != DutyDriver::status_t::RECEIVING && node->driver->getStatus() != DutyDriver::status_t::TRANSMITTING)
            for (auto ne : node->neighbours) {
                auto nev = ne->getBeacon(node, getDispatchTime());
                if (nev != nullptr)
                    newEvent(shared_ptr<Event> (nev));
            }
    }

    // Generate the next beacon
    newEvent(shared_ptr<Event> (new BeaconEvent(node->driver->scheduleRx(getDispatchTime()), node)));
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
    os << "Kind: DATA(" << dkind << ") Dst: " << dst->getId() << ' ';

    return os;
}
#endif

void DataEvent::process() {
    switch (dkind) {
        case data_kind_t::DATA_START:
            dst->startReception(node);

            // New DataEnd Event
            newEvent(std::shared_ptr<Event> (new DataEvent(getDispatchTime() + getTxTime(packet), node, dst, packet, data_kind_t::DATA_END)));

            break;
        case data_kind_t::DATA_END:
            dst->endReception(node, packet);
            break;
    }
}

#ifndef NDEBUG

ostream& operator<<(ostream& os, const Node& n) {
    os << "Node id: " << n.getId() << " Location: " << n.getLocation() << endl;
    os << "  Neighbours: ";
    for (auto ne : n.neighbours) {
        os << ne->getId() << " ";
    }

    return os;
}
#endif