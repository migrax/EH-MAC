/* 
 * File:   EnhDriver.cpp
 * Author: miguel
 * 
 * Created on 26 de febrero de 2015, 11:27
 */

#include "EnhDriver.h"
#include "EnhNode.h"


const EnhNode& EnhDriver::getEnhNode() const {
    return static_cast<const EnhNode&> (getNode());
}

EnhDriver::EnhDriver(const EnhNode& node, Event::evtime_t bitlen, float max_beacon_rate) : PWDriver(node, bitlen), max_beacon_rate_(max_beacon_rate), beacon_rate_(1) {  
}

Event::evtime_t EnhDriver::getExpectedExactBeaconTime(Node::nodeid_t dst, Event::evtime_t now) {     
    const auto neigh_beacon_rate = getEnhNode().getCurrentNeighbourBeaconRate(dst);
    const auto expected_beat = PWDriver::getExpectedExactBeaconTime(dst, now);
    
    if (now == 0)
        return expected_beat;
    
    auto prev_beat = getPrevBeaconTime(dst, expected_beat);
    
#ifndef NDEBUG
    std::cerr << getNode().getId() << " estimates node " << dst << " did " << prev_beat << " will do " << expected_beat << std::endl;
#endif
    
    assert (prev_beat < expected_beat);
    
    auto rng = PseudoRNG((dst << 16) + static_cast<int>(floor(prev_beat)));
    const auto subinterval = 1/max_beacon_rate_;
    const auto threshold = neigh_beacon_rate/max_beacon_rate_ * rng.randMax();
    
    auto next_beacon = prev_beat + subinterval;
    auto valid_beacon = prev_beat; // Any value < now will do
    while (valid_beacon < now && next_beacon < expected_beat) {
        if (rng() <= threshold)
            valid_beacon = next_beacon;
        next_beacon += subinterval;
    }
    if (valid_beacon < now)
        valid_beacon = expected_beat;
    
    return valid_beacon;
}