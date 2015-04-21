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
#include <iostream>

class NodeException : public SimulationException {
public:

    NodeException(std::string message, std::shared_ptr<Node> node) : SimulationException(message), node_(node) {
    }

    virtual std::ostream& debug(std::ostream& os) const;
protected:
    std::shared_ptr<Node> node_;
};

class RoutingException : public NodeException {
public:

    RoutingException(std::string message, std::shared_ptr<Node> node, Location dst) : NodeException(message, node), dst_(dst) {
    }

    virtual std::ostream& debug(std::ostream& os) const;
protected:
    Location dst_;
};

class NodeEvent : public Event {
public:

    NodeEvent(evtime_t d_time, Node& node) : Event(d_time), node_(node) {
    }

#ifndef NDEBUG
    virtual std::ostream& dump(std::ostream& os) const;
#endif

protected:
    Node& node_;
};

class BeaconEvent : public NodeEvent {
public:

    enum class beacon_kind_t {
        BEACON_START, BEACON_END
    };

    BeaconEvent(evtime_t d_time, Node& node, beacon_kind_t bkind = beacon_kind_t::BEACON_START) : NodeEvent(d_time, node), bkind_(bkind) {
    }

    void process();
#ifndef NDEBUG

    virtual std::ostream& dump(std::ostream& os) const {
        NodeEvent::dump(os);
        os << "Kind: Beacon_";
        switch (bkind_) {
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
    beacon_kind_t bkind_;
};

class DataEvent : public NodeEvent {
public:

    enum class data_kind_t {
        DATA_START, DATA_END
    };

    DataEvent(evtime_t d_time, Node& src_node, Node& dst_node, const Packet& p, data_kind_t dkind = data_kind_t::DATA_START) : NodeEvent(d_time, src_node), dst_(dst_node), packet_(p), dkind_(dkind) {
    }

    void process();

#ifndef NDEBUG

    virtual std::ostream& dump(std::ostream& os) const;
#endif

private:
    Node& dst_;
    const Packet packet_;
    data_kind_t dkind_;

    auto getTxTime(const Packet& p) const {
        return 8 * p.getPSize() / static_cast<double> (Link::getCapacity());
    }
};

#ifndef NDEBUG
std::ostream& operator<<(std::ostream& os, const DataEvent::data_kind_t& kind);
#endif

class Node : public std::enable_shared_from_this<Node> {
public:
    using nodeid_t = unsigned int;

private:
    friend class RoutingException;
    static nodeid_t unique_id_counter_;

    RoutingTable routing_table_;
    const Location loc_;
    const nodeid_t unique_id_;
    unsigned int receiving_data_;
    int n_rtx_;
    bool colliding_;
    bool pending_beacon_;

    std::vector<std::reference_wrapper<Node>> neighbours_;
    std::map<nodeid_t, std::shared_ptr<Link> > links_;

    Node& getClosestNeighbour(const Location& dest);

    // In pkts

    auto getBackOff() const {
        return (1 << n_rtx_) - 1; // 2**n_rtx - 1                
    }
    
    auto txingBeacon() const {
        return pending_beacon_;
    }

public:
    friend class BeaconEvent;
    friend class DataEvent;

    auto getId() const {
        return unique_id_;
    }

    void addNeightbour(Node& neigh) {
        neighbours_.push_back(neigh);
        links_[neigh.getId()] = std::make_shared<Link>(neigh);
    }

    auto getLocation() const {
        return loc_;
    }

    void Route(const Packet& p, Event::evtime_t now) {
        if (p.getDestination() == getId())
            return Process(p, now);

        auto next_hop = routing_table_.getNext(p.getDestination());
        if (next_hop == nullptr) {
            next_hop = &getClosestNeighbour(p.getFinalLocation());
            routing_table_.addEntry(p.getDestination(), *next_hop);
        }

        links_[next_hop->getId()]->queuePacket(p);
        getDriver().newData(next_hop->getId(), now);
    }

    friend std::ostream& operator<<(std::ostream&, const Node&);

private:

    void endListening(Event::evtime_t now) {
        if (receiving_data_ == 0)
            getDriver().setIdlestMode(now);
        colliding_ = false;
        pending_beacon_ = false;
    }

    void startTransmission(Node& dst, Event::evtime_t now) {
        dst.startReception(getId(), now);

        getDriver().setStatus(DutyDriver::status_t::TRANSMITTING, now);
    }

    void endTransmission(Node& dst, const Packet& p, Event::evtime_t now) {
        dst.endReception(*this, p, now);
    }

    void startReception(nodeid_t, Event::evtime_t now) {
        if (getDriver().getStatus() != DutyDriver::status_t::LISTENING) {
            assert(getDriver().getStatus() != DutyDriver::status_t::SLEEPING);
        }
        receiving_data_ += 1;
        getDriver().setStatus(DutyDriver::status_t::RECEIVING, now); // Useful for Collision Avoidance
    }

    auto ackReceived(Node& neigh, Event::evtime_t now, bool success = true) {
        if (success == false) {
            std::cout << "Collission when transferring from node " << getId() << " to node " << neigh.getId() << " @" << now << std::endl;
            // We have to wait until next beacon
            getDriver().setIdlestMode(now);
            // If it is not RIC-MAC, we have to announce that there is pending data
            getDriver().newData(neigh.getId(), now);
            return DutyDriver::status_t::SLEEPING;
        }

        auto queue_size = links_[neigh.getId()]->removePacket();

        // If we have more packets to send, ask receiver to go to listening state (Simulate a unicast beacon to it)
        if (queue_size > 0) {
            // Add new event to transmit the next packet
            Calendar::newEvent(std::make_unique<DataEvent> (getDriver().scheduleTx(now, getBackOff()), *this, neigh, links_[neigh.getId()]->getNextPacket()));
            getDriver().setStatus(DutyDriver::status_t::LISTENING, now);
            return DutyDriver::status_t::RECEIVING;
        } else {
            getDriver().emptyQueue(neigh.getId());
            getDriver().setIdlestMode(now);
            return DutyDriver::status_t::SLEEPING;
        }
    }

    void Process(const Packet& p, Event::evtime_t now) {
        std::cout << "Packet " << p << " got delivered @" << now << " transit time: " << now - p.getCreationTime() << std::endl;
    }

protected:
    Node(const Location&);

    virtual DutyDriver& getDriver() = 0;
    virtual const DutyDriver& getDriver() const = 0;

    virtual std::unique_ptr<DataEvent> getBeacon(Node& orig, Event::evtime_t now, int backoff) {
        switch (getDriver().getStatus()) {
            case DutyDriver::status_t::RECEIVING:
                // Collision
                if (colliding_ == false)
                    n_rtx_++;
                colliding_ = true;
                break;
            case DutyDriver::status_t::TRANSMITTING:
                // This may a collision, but on a receiver
                break;
            case DutyDriver::status_t::SLEEPING:
                break; // Nothing to do
            case DutyDriver::status_t::LISTENING:
                if (pending_beacon_)
                    break; // No not transmit while waiting for reception
                if (links_[orig.getId()]->getQueueSize() == 0)
                    break; // No data to send
                auto packet = links_[orig.getId()]->getNextPacket();
                getDriver().setStatus(DutyDriver::status_t::SLEEPING, now); // In case of backoff, we can rest a little until wakeup
                return std::make_unique<DataEvent>(getDriver().scheduleTx(now, backoff * 8 * packet.getPSize()), *this, orig, packet);
        }

        return std::unique_ptr<DataEvent>(nullptr);
    }

    virtual std::unique_ptr<BeaconEvent> sendBeacon(Event::evtime_t now) {
        if (getDriver().getStatus() != DutyDriver::status_t::SLEEPING && getDriver().getStatus() != DutyDriver::status_t::LISTENING)
            return nullptr; // We do not send beacons when we are txing or rxing traffic

        colliding_ = false;
        pending_beacon_ = true;
        getDriver().setStatus(DutyDriver::status_t::LISTENING, now);

        int backoff = getBackOff();

        for (auto ne : neighbours_) {
            auto nev = ne.get().getBeacon(*this, now, backoff);
            if (nev != nullptr)
                Calendar::newEvent(std::move(nev));
        }

        // Wait for 5 bytes after the end of the backoff period.
        return std::make_unique<BeaconEvent>((8 * 5 + backoff * 8 * Packet::getMaxPacketSize()) / Link::getCapacity() + now, *this, BeaconEvent::beacon_kind_t::BEACON_END);
    }

    virtual bool endReception(Node& orig, const Packet& p, Event::evtime_t now) {
        assert(receiving_data_ > 0);

        if (receiving_data_-- > 1 || colliding_) { // Collision
            if (colliding_ == false) {
                n_rtx_ += 1;
                colliding_ = true;
            }
            if (receiving_data_ == 0)
                colliding_ = false;

            getDriver().setStatus(pending_beacon_ ? DutyDriver::status_t::LISTENING : DutyDriver::status_t::SLEEPING, now);
            orig.ackReceived(*this, now, false);

            return false;
        } else {
            n_rtx_ = 0;
            auto newStatus = orig.ackReceived(*this, now); // Simulate a /small/ unicast ACK
            getDriver().setStatus(pending_beacon_ ? DutyDriver::status_t::LISTENING : newStatus, now);
            Route(p, now);

            return true;
        }
    }
};

#endif	/* NODE_H */

