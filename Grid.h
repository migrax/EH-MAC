/* 
 * File:   Grid.h
 * Author: miguel
 * 
 * The Universe where the Nodes reside
 */

#ifndef GRID_H
#define	GRID_H

#include "Location.h"

#include <memory>
#include <map>

#ifndef NDEBUG
#include <ostream>
#endif

class Node;

class Grid {
public:
    Grid(double transmissionRange);

    void addNode(const std::shared_ptr<Node>&);

#ifndef NDEBUG
    friend std::ostream& operator<<(std::ostream&, const Grid&);
#endif
private:
    const double txRange, sqTxRange;
    std::map<double, std::shared_ptr<Node > > nodes;
};

#endif	/* GRID_H */

