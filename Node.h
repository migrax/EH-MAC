/* 
 * File:   Node.h
 * Author: miguel
 *
 * Created on 4 de febrero de 2015, 15:56
 */

#ifndef NODE_H
#define	NODE_H

#include "Location.h"
#include "RoutingTable.h"
#include "Packet.h"
#include "Link.h"
#include "Event.h"
#include "DutyDriver.h"

#include <vector>
#include <memory>

#ifndef NDEBUG
#include <ostream>
#endif

class NodeEvent : public Event {
public:

    NodeEvent(evtime_t d_time, Node& node) : Event(d_time), node(node) {
    }

#ifndef NDEBUG

    virtual std::ostream& dump(std::ostream& os) const;
#endif

protected:
    Node& node;
};

class BeaconEvent : public NodeEvent {
public:

    enum class beacon_kind_t {
        BEACON_START, BEACON_END
    };

    BeaconEvent(evtime_t d_time, Node& node, beacon_kind_t bkind = beacon_kind_t::BEACON_START) : NodeEvent(d_time, node), bkind(bkind) {
    }

    void process();
#ifndef NDEBUG

    virtual std::ostream& dump(std::ostream& os) const {
        NodeEvent::dump(os);
        os << "Kind: Beacon_";
        switch (bkind) {
            case beacon_kind_t::BEACON_START:
                os << "START ";
                break;
            case beacon_kind_t::BEACON_END:
                os << "END ";
                break;
        }

        return os;
    }
#endif
private:
    beacon_kind_t bkind;
};

class DataEvent : public NodeEvent {
public:

    enum class data_kind_t {
        DATA_START, DATA_END
    };

    DataEvent(evtime_t d_time, Node& src_node, Node& dst_node, const Packet& p, data_kind_t dkind = data_kind_t::DATA_START) : NodeEvent(d_time, src_node), dst(dst_node), packet(p), dkind(dkind) {
    }

    void process();

#ifndef NDEBUG

    virtual std::ostream& dump(std::ostream& os) const;
#endif

private:
    Node& dst;
    const Packet packet;
    data_kind_t dkind;

    auto getTxTime(const Packet& p) const {
        return 8 * p.getPSize() / static_cast<double> (Link::getCapacity());
    }
};

#ifndef NDEBUG
std::ostream& operator<<(std::ostream& os, const DataEvent::data_kind_t& kind);
#endif

class Node {
public:
    using nodeid_t = unsigned int;

private:
    static nodeid_t uniqueIdCounter;

    RoutingTable r_table;
    const Location loc;    
    const nodeid_t uniqueId;
    unsigned int rxingData;
    Packet::packetid_t lastPacketId;
    int n_rtx;
    bool colliding;
    std::unique_ptr<DutyDriver> driver;

    std::vector<std::reference_wrapper<Node>> neighbours;
    std::map<nodeid_t, std::shared_ptr<Link> > links;

    Node& getClosestNeighbour(const Location& dest) const;

    auto getBackOff() const {
        return (1 << n_rtx) - 1; // 2**n_rtx - 1
    }

public:
    friend class BeaconEvent;
    friend class DataEvent;

    auto getId() const {
        return uniqueId;
    }

    void addNeightbour(Node& neigh) {
        neighbours.push_back(neigh);
        links[neigh.getId()] = std::make_shared<Link>(neigh);
    }

    auto getLocation() const {
        return loc;
    }

    void Route(const Packet& p) {
        if (p.getDestination() == getId())
            return Process(p);

        auto nextHop = r_table.getNext(p.getDestination());
        if (nextHop == nullptr) {
            nextHop = &getClosestNeighbour(p.getFinalLocation());
            r_table.addEntry(p.getDestination(), *nextHop);
        }

        links[nextHop->getId()]->queuePacket(p);
        driver->newData(nextHop->getId());
    }

    void endListening() {
        driver->setIdlestMode();
        colliding = false;
    }

    auto getBeacon(Node& orig, Event::evtime_t now, int backoff) {
        switch (driver->getStatus()) {
            case DutyDriver::status_t::RECEIVING:
                // Collision
                colliding = true;
            case DutyDriver::status_t::TRANSMITTING:
                // This may a collision, but on a receiver
                break;
            case DutyDriver::status_t::SLEEPING:
                break; // Nothing to do
            case DutyDriver::status_t::LISTENING:
                if (links[orig.getId()]->getQueueSize() == 0)
                    break; // No data to send
                return std::make_unique<DataEvent>(driver->scheduleTx(now, getBackOff()), *this, orig, links[orig.getId()]->getNextPacket());
        }

        return std::unique_ptr<DataEvent>(nullptr);
    }

    auto sendBeacon(Event::evtime_t now) {
        driver->setStatus(DutyDriver::status_t::LISTENING);

        int backoff = getBackOff();

        for (auto ne : neighbours) {            
            auto nev = ne.get().getBeacon(*this, now, backoff);
            if (nev != nullptr)
                Calendar::newEvent(std::move(nev));
        }

        // Wait for 5 bytes after the end of the backoff period.
        return std::make_unique<BeaconEvent>((5 + backoff) / Link::getCapacity() + now, *this, BeaconEvent::beacon_kind_t::BEACON_END);
    }

    void startTransmission(Node& dst) {
        dst.startReception(getId());

        driver->setStatus(DutyDriver::status_t::TRANSMITTING);
    }

    void endTransmission(Node& dst, const Packet& p, Event::evtime_t now) {
        dst.endReception(*this, p, now);
    }

    void startReception(nodeid_t) {
        rxingData += 1;
        if (driver->getStatus() != DutyDriver::status_t::LISTENING) {
            // FIXME: Colision or not ready
        }
        driver->setStatus(DutyDriver::status_t::RECEIVING); // Useful for Collision Avoidance
    }

    auto ackReceived(Node& neigh, Event::evtime_t now, bool success = true) {
        if (success == false) {
            n_rtx += 1;
            // We have to wait until next beacon
            return DutyDriver::status_t::LISTENING; // FIXME: This can be different in other protocols
        }

        auto qSize = links[neigh.getId()]->removePacket();
        n_rtx = 0;

        // If we have more packets to send, ask receiver to go to listening state (Simulate a unicast beacon to it)
        if (qSize > 0) {
            // Add new event to transmit the next packet
            Calendar::newEvent(std::make_unique<DataEvent> (driver->scheduleTx(now, getBackOff()), *this, neigh, links[neigh.getId()]->getNextPacket()));
            return DutyDriver::status_t::RECEIVING;
        } else {
            driver->emptyQueue(neigh.getId());
            return DutyDriver::status_t::SLEEPING;
        }
    }

    void endReception(Node& orig, const Packet& p, Event::evtime_t now) {
        assert(rxingData > 0);

        if (rxingData-- > 1) { // Collision
            colliding = true;
            driver->setStatus(DutyDriver::status_t::SLEEPING);
            orig.ackReceived(orig, now, false);
        } else {
            auto newStatus = orig.ackReceived(*this, now); // Simulate a /small/ unicast ACK
            driver->setStatus(newStatus);
            Route(p);
        }
    }

    void Process(const Packet& p) {
        // FIXME: This is where packets end
    }
#ifndef NDEBUG
    friend std::ostream& operator<<(std::ostream&, const Node&);
#endif

protected:
    Node(const Location&, std::unique_ptr<DutyDriver> driver);
};

#endif	/* NODE_H */

