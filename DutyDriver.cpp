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


#include "DutyDriver.h"
#include "Node.h"

#include <ostream>

using namespace std;

double DutyDriver::sleep_length_ = 1; // Seconds

namespace {

    ostream& operator<<(ostream& os, DutyDriver::status_t status) {
        switch (status) {
            case DutyDriver::status_t::LISTENING:
                os << "LISTENING";
                break;
            case DutyDriver::status_t::RECEIVING:
                os << "RECEIVING";
                break;
            case DutyDriver::status_t::SLEEPING:
                os << "SLEEPING";
                break;
            case DutyDriver::status_t::TRANSMITTING:
                os << "TRANSMITTING";
                break;
        }

        return os;
    }
}

void DutyDriver::logStatus(Event::evtime_t now) const {
    cout << "@" << now << " Node: " << node_.getId() << " goes " << curStatus << endl;
}