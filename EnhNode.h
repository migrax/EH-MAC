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
#include "PWNode.h"
#include "PoissonParameterEstimator.h"
#include <memory>
#include <map>

#ifndef NDEBUG
#include <iostream>
#endif

class EnhNode : public Node {

    class Cycle {
        Event::evtime_t start_time_;
        int n_total_rx_, n_total_col_;

    public:

        Cycle(Event::evtime_t start_time) : start_time_(start_time), n_total_rx_(0), n_total_col_(0) {
        }

        auto newPeriod(int n_receptions, int n_collisions) {
            n_total_rx_ += n_receptions;
            n_total_col_ += n_collisions;

            return n_collisions == 0 && n_total_rx_ > 0;
        }

        auto getSummary(Event::evtime_t now) const {
            return std::make_pair(now - start_time_, n_total_rx_);
        }
    } cycle_;

public:

    EnhNode(const Location& loc) : Node(loc), cycle_(0), n_collisions_(0), n_receptions_(0), prev_beacon_(0), driver_(std::make_unique<EnhDriver>(*this, 1 / Link::getCapacity())) {
        Calendar::newEvent(std::make_unique<BeaconEvent> (getDriver().scheduleRx(0), *this));
    }

    auto getCurrentNeighbourBeaconRate(Node::nodeid_t neigh_id) const {
        auto rate_it = neighbour_beacon_rates_.find(neigh_id);
        return rate_it != neighbour_beacon_rates_.end() ? rate_it->second : 1;
    }
private:
    int n_collisions_, n_receptions_;
    Event::evtime_t prev_beacon_;
    PoissonParameterEstimator<Event::evtime_t> rate_estimator_;

    virtual std::unique_ptr<BeaconEvent> sendBeacon(Event::evtime_t now) {
#ifndef NDEBUG
        std::cerr << "Rate node " << getId() << " @" << now << " Collisions: " << n_collisions_ << " Receptions: " << n_receptions_ << " Elapsed: " << now - prev_beacon_ << std::endl;

#endif
        if (cycle_.newPeriod(n_receptions_, n_collisions_)) {
            rate_estimator_.newSample(cycle_.getSummary(now));
            std::cout << "@" << now << " node " << getId() << " senses " << rate_estimator_.getParameter() << " pkts/s Rate (status: " << rate_estimator_.isGood() << ')' << std::endl;
            if (rate_estimator_.isGood())
                driver_->setBeaconRate(rate_estimator_.getParameter(), now);
            cycle_ = Cycle(now);
        }

        n_collisions_ = n_receptions_ = 0;
        prev_beacon_ = now;

        return Node::sendBeacon(now);
    }

protected:

    virtual bool endReception(Node& orig, const Packet& p, Event::evtime_t now) {
        auto success = Node::endReception(orig, p, now);

        if (success)
            n_receptions_++;
        else
            n_collisions_++;

        return success;
    }

    std::unique_ptr<DataEvent> getBeacon(EnhNode& orig, Event::evtime_t now, int backoff) {
        neighbour_beacon_rates_[orig.getId()] = orig.getDriver().getBeaconRate();

        return Node::getBeacon(orig, now, backoff);
    }

    virtual std::unique_ptr<DataEvent> getBeacon(Node& orig, Event::evtime_t now, int backoff) {
        return getBeacon(static_cast<EnhNode&> (orig), now, backoff);
    }

    virtual EnhDriver& getDriver() {
        return *driver_;
    }

    virtual const EnhDriver& getDriver() const {
        return *driver_;
    }

private:
    std::unique_ptr<EnhDriver> driver_;
    std::map<Node::nodeid_t, float> neighbour_beacon_rates_;
};

#endif	/* ENHNODE_H */

