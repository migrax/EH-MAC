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


#ifndef HOMOGENEOUSSPATIALPARETO_H
#define	HOMOGENEOUSSPATIALPARETO_H

#include <utility>
#include <vector>
#include <functional>

#include <random>

#include "Location.h"

template<typename RandomGenType>
class HomogeneousSpatialPareto {
public:
    using value_type = Location::value_type;

    HomogeneousSpatialPareto(value_type lambda, RandomGenType& rng, value_type side_len = 1) : lambda_(lambda), height_(side_len), width_(side_len), rng_(rng) {
        auto poissonDistr = std::poisson_distribution<int> (lambda * (width_ * height_)); // Parameter: number of expected points in the area
        auto uniformXDistr = std::uniform_real_distribution<value_type> (0, width_);
        auto uniformYDistr = std::uniform_real_distribution<value_type> (0, height_);

        auto nPoints = poissonDistr(rng);
        points_.reserve(nPoints);
        for (auto i = 0; i < nPoints; i++) {
            points_.push_back(Location(uniformXDistr(rng), uniformYDistr(rng)));
        }
    }

    auto& getPoints() const {
        return points_;
    }

    auto getMaximumNeighbouringDistance() const {
        auto first = Location(0, 0);
        auto second = first;
        auto distance = value_type {0};

        for (auto pivot : points_) {
            auto closestDistance = std::numeric_limits<value_type>::infinity();
            Location closestNeigh = Location(0, 0);
            
            for (auto neigh : points_) {
                if (neigh == pivot)
                    continue;
                
                if (pivot.getSquaredDistanceTo(neigh) < closestDistance) {
                    closestDistance = pivot.getSquaredDistanceTo(neigh);
                    closestNeigh = Location(neigh);
                }
            }
            
            if (closestDistance > distance) {
                distance = closestDistance;
                first = pivot;
                second = closestNeigh;
            }
        }       

        return std::make_tuple(first.getDistanceTo(second), first, second);
    }

 auto getMaximumNeighbouringDistanceToOrigin() const {
        auto first = Location(0, 0);
        auto second = first;
        auto distance = value_type {0};

        for (auto pivot : points_) { 
            if (pivot == points_[0])
	        continue;


            auto closest_distance = std::numeric_limits<value_type>::infinity();
	    auto pivot_distance = pivot.getSquaredDistanceTo(points_[0]);
            Location closest_neigh = Location(0, 0);

            for (auto neigh : points_) {
                if (neigh == pivot)
                    continue;

                if (pivot.getSquaredDistanceTo(neigh) < closest_distance && neigh.getSquaredDistanceTo(points_[0]) < pivot_distance) {
                    closest_distance = pivot.getSquaredDistanceTo(neigh);
                    closest_neigh = Location(neigh);
                }
            }

            if (closest_distance > distance) {
                distance = closest_distance;
                first = pivot;
                second = Location(closest_neigh);
            }
        }

        return std::make_tuple(first.getDistanceTo(second), first, second);
    }


private:
    const value_type lambda_, height_, width_;
    RandomGenType& rng_;
    std::vector<Location> points_;
};

#endif	/* HOMOGENEOUSSPATIALPARETO_H */

