/* 
 * File:   RINode.h
 * Author: miguel
 *
 * Created on 10 de febrero de 2015, 15:46
 */

#ifndef RINODE_H
#define	RINODE_H

#include "RIDriver.h"
#include "Node.h"

class RINode : public Node {
public:

    RINode(const Location& loc) : Node(loc), driver_(std::make_unique<RIDriver>(*this, 1 / Link::getCapacity())) {
        Calendar::newEvent(std::make_unique<BeaconEvent> (getDriver().scheduleRx(0), *this));
    }

private:
    std::unique_ptr<RIDriver> driver_;
    
    virtual DutyDriver& getDriver() {
        return *driver_;
    }
    
    virtual const DutyDriver& getDriver() const {
        return *driver_;
    }
};

#endif	/* RINODE_H */

