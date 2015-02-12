/* 
 * File:   RIDriver.cpp
 * Author: miguel
 * 
 * Created on 10 de febrero de 2015, 13:35
 */

#include "RIDriver.h"
#include "Event.h"

#include <boost/random/uniform_real_distribution.hpp>

const double RIDriver::sleepLength = 1; // Seconds

/* In RI-MAC we sleep between ½ sleepLength and 1.5 sleepLength*/
Event::evtime_t RIDriver::getTimeUntilListen() {
    Calendar::randomGen& rng = Calendar::getCalendar().getRandomGenerator();
    
    boost::random::uniform_real_distribution<Event::evtime_t> distr(-0.5, 0.5);

    return sleepLength + distr(rng);
}