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
    using value_type = double;
    
    Location(value_type lat, value_type lon) : lat_(lat), lon_(lon) {
        assert(lat >= 0);
        assert(lon >= 0);
    }
    
    auto getLatitude() const {
        return lat_;
    }

    auto getLongitude() const {
        return lon_;
    }

    auto getSquaredDistanceTo(const Location& origin) const {
        return (lat_ - origin.lat_)*(lat_ - origin.lat_) + (lon_ - origin.lon_)*(lon_ - origin.lon_);
    }

    auto getDistanceTo(const Location& origin) const {
        return std::sqrt(getSquaredDistanceTo(origin));
    }

    auto getDistanceToOrigin() const {
        return getDistanceTo(Location(0, 0));
    }

    auto operator+(const Location& b) const {
        return Location(lat_ + b.lat_, lon_ + b.lon_);
    }

    auto operator-(const Location& b) const {
        return Location(std::max(0., lat_ - b.lat_), std::max(0., lon_ - b.lon_));
    }

    auto operator==(const Location& b) const {
        return lat_ == b.lat_ && lon_ == b.lon_;
    }
    
private:
    value_type lat_, lon_;
};

std::ostream& operator<<(std::ostream& os, const Location& loc);

#endif	/* LOCATION_H */

