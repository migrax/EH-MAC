/* 
 * File:   PoissonParameterEstimator.h
 * Author: miguel
 *
 * Created on 10 de marzo de 2015, 11:55
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

