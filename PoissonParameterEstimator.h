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


#ifndef POISSONPARAMETERESTIMATOR_H
#define	POISSONPARAMETERESTIMATOR_H

#include <vector>
#include <utility>

template<typename IntervalType = double>
class PoissonParameterEstimator {
public:

    PoissonParameterEstimator(unsigned int window_size = 15) : window_size_(window_size), index_(0), samples_() {
        samples_.reserve(window_size_);
    }

    auto getParameter() const {
        IntervalType total_time = 0;
        unsigned int events = 0;

        for (auto sample : samples_) {
            total_time += sample.first;
            events += sample.second;
        }
        
        return events / total_time;
    }

    auto isGood() const {
        return samples_.size() == window_size_;        
    }

    auto newSample(const std::pair<IntervalType, unsigned int>& sample) {
        if (samples_.size() < window_size_)
            samples_.push_back(sample);
        else {
            samples_[index_++] = sample;
            index_ %= window_size_;
        }
    }

    auto newSample(IntervalType interval, unsigned int events) {
        return newSample(std::make_pair(interval, events));
    }

private:
    const unsigned int window_size_;
    unsigned int index_;
    std::vector<std::pair<IntervalType, unsigned int>> samples_;
};

#endif	/* POISSONPARAMETERESTIMATOR_H */

