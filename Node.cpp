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

Node::Node(const Location& loc, unique_ptr<DutyDriver> dutydriver) :
loc(loc), uniqueId(++uniqueIdCounter), rxingData(0), lastPacketId(0), n_rtx(0), colliding(false), driver(std::move(dutydriver)) { // 0 is not a valid PacketId number
    // Start the beacons!
    Calendar::newEvent(make_unique<BeaconEvent> (driver->scheduleRx(0), *this));
}

Node& Node::getClosestNeighbour(const Location& dest) const {
    Node *best = nullptr;
    double best_sq_distance = dest.getSquaredDistanceTo(getLocation());

    for (auto ne : neighbours) {
        Node& neigh = ne.get();
        double sq_distance = dest.getSquaredDistanceTo(neigh.getLocation());
        if (sq_distance < best_sq_distance) {
            best_sq_distance = sq_distance;
            best = &ne.get();
        }
    }

    /* Unreachable destination */
    assert(best != nullptr);

    return *best;
}

#ifndef NDEBUG

ostream& NodeEvent::dump(ostream& os) const {
    Event::dump(os);
    os << "Node: " << node.getId() << ' ';

    return os;
}
#endif

void BeaconEvent::process() {
    switch (bkind) {
        case beacon_kind_t::BEACON_START:
            /* Avoid obvious collissions
             * a) We are not receiving data, even if not directed to us
             * b) We are not transmitting data
             */
            if (node.rxingData == 0 && node.driver->getStatus() != DutyDriver::status_t::RECEIVING && node.driver->getStatus() != DutyDriver::status_t::TRANSMITTING) {
                auto endBEaconEvenet = node.sendBeacon(getDispatchTime());
                if (endBEaconEvenet != nullptr)
                    newEvent(move(endBEaconEvenet));
            }
            // Generate the next beacon
            newEvent(make_unique<BeaconEvent>(node.driver->scheduleRx(getDispatchTime()), node));
            break;
        case beacon_kind_t::BEACON_END:
            node.endListening();
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
    os << "Kind: DATA(" << dkind << ") Dst: " << dst.getId() << ' ';

    return os;
}
#endif

void DataEvent::process() {
    switch (dkind) {
        case data_kind_t::DATA_START:
            node.startTransmission(dst);

            // New DataEnd Event
            newEvent(std::make_unique<DataEvent>(getDispatchTime() + getTxTime(packet), node, dst, packet, data_kind_t::DATA_END));

            break;
        case data_kind_t::DATA_END:
            node.endTransmission(dst, packet, getDispatchTime());
            break;
    }
}

#ifndef NDEBUG

ostream& operator<<(ostream& os, const Node& n) {
    os << "Node id: " << n.getId() << " Location: " << n.getLocation() << endl;
    os << "  Neighbours: ";
    for (auto ne : n.neighbours) {
        os << ne.get().getId() << " ";
    }

    return os;
}
#endif