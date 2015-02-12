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

    NodeEvent(evtime_t d_time, const std::shared_ptr<Node>& node) : Event(d_time), node(node) {
    }

#ifndef NDEBUG

    virtual std::ostream& dump(std::ostream& os) const;
#endif

protected:
    std::shared_ptr<Node> node;
};

class BeaconEvent : public NodeEvent {
public:

    enum class beacon_kind_t {
        BEACON_START/*, BEACON_END*/
    };

    BeaconEvent(evtime_t d_time, const std::shared_ptr<Node>& node, beacon_kind_t b_kind = beacon_kind_t::BEACON_START) : NodeEvent(d_time, node), bkind(bkind) {
    }

    void process();
#ifndef NDEBUG

    virtual std::ostream& dump(std::ostream& os) const {
        NodeEvent::dump(os);
        os << "Kind: Beacon_START ";

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

    DataEvent(evtime_t d_time, const std::shared_ptr<Node>& src_node, const std::shared_ptr<Node>& dst_node, const Packet& p, data_kind_t dkind = data_kind_t::DATA_START) : NodeEvent(d_time, src_node), dst(dst_node), packet(p), dkind(dkind) {
    }

    void process();

#ifndef NDEBUG

    virtual std::ostream& dump(std::ostream& os) const;
#endif

private:
    const Packet packet;
    data_kind_t dkind;
    std::shared_ptr<Node> dst;

    double getTxTime(const Packet& p) const {
        return 8 * p.getPSize() / static_cast<double> (Link::getCapacity());
    }
};

#ifndef NDEBUG
std::ostream& operator<<(std::ostream& os, const DataEvent::data_kind_t& kind); 
#endif

class Node {
public:
    friend class BeaconEvent;
    friend class DataEvent;
    typedef unsigned int nodeid_t;

    void addNeightbour(std::shared_ptr<Node> neigh) {
        neighbours.push_back(neigh);
        links[neigh->getId()] = std::make_shared<Link>(neigh);
    }

    nodeid_t getId() const {
        return uniqueId;
    }

    const Location &getLocation() const {
        return loc;
    }

    void Route(const Packet& p) {
        if (p.getDestination() == getId())
            return Process(p);

        std::shared_ptr<Node> nextHop = r_table.getNext(p.getDestination());
        if (nextHop == nullptr) {
            nextHop = getClosestNeighbour(p.getFinalLocation());
            r_table.addEntry(p.getDestination(), nextHop);
        }

        links[nextHop->getId()]->queuePacket(p);
        driver->newData(nextHop->getId());
    }

    Event *getBeacon(std::shared_ptr<Node>& node, Event::evtime_t now) {
        switch (driver->getStatus()) {
            case DutyDriver::status_t::RECEIVING:
            case DutyDriver::status_t::TRANSMITTING:
                // FIXME: This is a collision
                break;
            case DutyDriver::status_t::SLEEPING:
                break; // Nothing to do
            case DutyDriver::status_t::LISTENING:
                if (links[node->getId()]->getQueueSize() == 0)
                    break; // No data to send
                return new DataEvent(driver->scheduleTx(now), std::shared_ptr<Node> (this), node, links[node->getId()]->getNextPacket());
        }

        return nullptr;
    }

    void startReception(std::shared_ptr<Node>& orig) {
        if (driver->getStatus() != DutyDriver::status_t::LISTENING) {
            // FIXME: Colision or not ready
        }
        driver->setStatus(DutyDriver::status_t::RECEIVING); // Useful for Collision Avoidance
    }

    void endReception(std::shared_ptr<Node>& orig, const Packet& p) {
        auto newStatus = orig->ackReceived(getId()); // Simulate a /small/ unicast ACK
        driver->setStatus(newStatus);
        Route(p);
    }

    DutyDriver::status_t ackReceived(nodeid_t neighId) {
        auto qSize = links[neighId]->removePacket();

        // If we have more packets to send, ask receiver to go to listening state (Simulate a unicast beacon to it)
        if (qSize > 0)
            return DutyDriver::status_t::RECEIVING;
        else
            return DutyDriver::status_t::SLEEPING;
    }

    void Process(const Packet& p) {
        // FIXME: This is where packets end
    }
#ifndef NDEBUG
    friend std::ostream& operator<<(std::ostream&, const Node&);
#endif

protected:
    Node(const Location&, std::shared_ptr<DutyDriver> driver);

private:
    static nodeid_t uniqueIdCounter;

    RoutingTable r_table;
    const nodeid_t uniqueId;
    const Location loc;
    std::shared_ptr<DutyDriver> driver;
    std::vector<std::shared_ptr<Node> > neighbours;
    std::map<nodeid_t, std::shared_ptr<Link> > links;

    std::shared_ptr<Node> &getClosestNeighbour(const Location& dest) const;
};

#endif	/* NODE_H */

