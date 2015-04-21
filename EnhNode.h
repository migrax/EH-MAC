/* 
 * File:   EnhNode.h
 * Author: miguel
 *
 * Created on 26 de febrero de 2015, 11:27
 */

#ifndef ENHNODE_H
#define	ENHNODE_H

#include "Node.h"
#include "EnhDriver.h"
#include <memory>

#ifndef NDEBUG
#include <iostream>
#endif

class EnhNode : public Node {
public:

    EnhNode(const Location& loc) : Node(loc), driver_(std::make_unique<EnhDriver>(*this, 1 / Link::getCapacity())), n_collisions_(0), n_receptions_(0), prev_beacon_(0) {
        Calendar::newEvent(std::make_unique<BeaconEvent> (getDriver().scheduleRx(0), *this));
    }

private:
    int n_collisions_, n_receptions_;
    Event::evtime_t prev_beacon_;

    virtual std::unique_ptr<BeaconEvent> sendBeacon(Event::evtime_t now) {
#ifndef NDEBUG
        std::cerr << "Rate node " << getId() << " @" << now << " Collisions: " << n_collisions_ << " Receptions: " << n_receptions_ << " Elapsed: " << now - prev_beacon_ << std::endl;

#endif
        n_collisions_ = n_receptions_ = 0;
        prev_beacon_ = now;

        return Node::sendBeacon(now);
    }

    virtual bool endReception(Node& orig, const Packet& p, Event::evtime_t now) {
        auto success = Node::endReception(orig, p, now);

        if (success)
            n_receptions_++;
        else
            n_collisions_++;

        return success;
    }

    std::unique_ptr<EnhDriver> driver_;

    virtual DutyDriver& getDriver() {
        return *driver_;
    }

    virtual const DutyDriver& getDriver() const {
        return *driver_;
    }
};

#endif	/* ENHNODE_H */

