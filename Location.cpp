/* 
 * File:   Location.cpp
 * Author: miguel
 * 
 * Created on 4 de febrero de 2015, 15:58
 */

#include "Location.h"

using namespace std;

ostream& operator<<(ostream& os, const Location& loc) {
    os << '(' << loc.getLongitude() << ", " << loc.getLatitude() << ')';
    
    return os;
}
