/* 
 * File:   EnhDriver.cpp
 * Author: miguel
 * 
 * Created on 26 de febrero de 2015, 11:27
 */

#include "EnhDriver.h"
#include "EnhNode.h"


constexpr int EnhDriver::base_factor_ = 4;

const EnhNode& EnhDriver::getEnhNode() const {
    return static_cast<const EnhNode&> (getNode());
}

EnhDriver::EnhDriver(const EnhNode& node, Event::evtime_t bitlen) : PWDriver(node, bitlen), m_factor_(base_factor_) {
    sleep_length_ = base_factor_;
}

Event::evtime_t EnhDriver::getExpectedExactBeaconTime(Node::nodeid_t dst, Event::evtime_t now) {     
    auto neigh_factor = getEnhNode().getCurrentNeighbourFactor(dst) * base_factor_;
    if (neigh_factor == 0)
        neigh_factor = base_factor_;
    
    auto expected_beat = PWDriver::getExpectedExactBeaconTime(dst, now);
    
    if (now == 0)
        return expected_beat;
    
    auto prev_beat = getPrevBeaconTime(dst, expected_beat);
    
    std::cerr << getNode().getId() << " estimates node " << dst << " did " << prev_beat << " will do " << expected_beat << std::endl;
    
    assert (prev_beat < expected_beat);
    
    auto subinterval = (expected_beat - prev_beat) / static_cast<Event::evtime_t> (neigh_factor);
    auto next_beacon = prev_beat + subinterval;
    while (next_beacon + subinterval <= now || next_beacon < now)
        next_beacon += subinterval;

    return next_beacon;
}