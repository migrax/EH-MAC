/* 
 * File:   Location.h
 * Author: miguel
 *
 * Created on 4 de febrero de 2015, 15:58
 */

#ifndef LOCATION_H
#define	LOCATION_H

#include <cmath>
#include <cassert>
#include <ostream>
#include <algorithm>

class Location {
public:

    Location(double lat, double lon) : lat(lat), lon(lon) {
        assert(lat >= 0);
        assert(lon >= 0);
    }    

    double getLatitude() const {
        return lat;
    }

    double getLongitude() const {
        return lon;
    }

    double getSquaredDistanceTo(const Location& origin) const {
        return (lat - origin.lat)*(lat - origin.lat) + (lon - origin.lon)*(lon - origin.lon);
    }

    double getDistanceTo(const Location& origin) const {
        return std::sqrt(getSquaredDistanceTo(origin));
    }

    double getDistanceToOrigin() const {
        return getDistanceTo(Location(0, 0));
    }

    Location operator+(const Location& b) const {
        return Location(lat + b.lat, lon + b.lon);
    }

    Location operator-(const Location& b) const {
        return Location(std::max(0., lat - b.lat), std::max(0., lon - b.lon));
    }


private:
    const double lat, lon;
};

std::ostream& operator<<(std::ostream& os, const Location& loc);

#endif	/* LOCATION_H */

