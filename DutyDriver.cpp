/* 
 * File:   DutyDriver.cpp
 * Author: miguel
 * 
 * Created on 10 de febrero de 2015, 13:31
 */

#include "DutyDriver.h"
#include "Node.h"

#include <ostream>

using namespace std;

const double DutyDriver::sleep_length_ = 1; // Seconds

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