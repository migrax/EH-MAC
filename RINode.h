/*
 * Copyright (C) 2015 Miguel Rodríguez Pérez <miguel@det.uvigo.es>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

