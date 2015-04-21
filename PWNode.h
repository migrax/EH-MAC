/* 
 * File:   PWNode.h
 * Author: miguel
 *
 * Created on 24 de febrero de 2015, 10:36
 */

#ifndef PWNODE_H
#define	PWNODE_H

#include <memory>

#include "Node.h"
#include "PWDriver.h"

class PWNode : public Node {
public:

    PWNode(const Location& loc) : Node(loc), driver_(std::make_unique<PWDriver>(*this, 1 / Link::getCapacity())) {
        Calendar::newEvent(std::make_unique<BeaconEvent> (getDriver().scheduleRx(0), *this));
    }

private:
    std::unique_ptr<PWDriver> driver_;
    
    virtual DutyDriver& getDriver() {
        return *driver_;
    }
    
    virtual const DutyDriver& getDriver() const {
        return *driver_;
    }
};

#endif	/* PWNODE_H */

