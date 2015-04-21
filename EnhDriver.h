/* 
 * File:   EnhDriver.h
 * Author: miguel
 *
 * Created on 26 de febrero de 2015, 11:27
 */

#ifndef ENHDRIVER_H
#define	ENHDRIVER_H

#include "PWDriver.h"


class EnhDriver : public PWDriver {
public:
    EnhDriver(const Node& node, Event::evtime_t bitlen) : PWDriver(node, bitlen) {}
    
};

#endif	/* ENHDRIVER_H */

