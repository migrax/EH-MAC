/* 
 * File:   Grid.cpp
 * Author: miguel
 * 
 * Created on 4 de febrero de 2015, 15:56
 */

#include "Grid.h"
#include "Node.h"

#include <iostream>
#include <cassert>

using namespace std;

Grid::Grid(Location::value_type transmission_range) : tx_range_(transmission_range), sq_tx_range_(transmission_range*transmission_range) {
    assert(tx_range_ > 0);
}

void Grid::addNode(shared_ptr<Node> new_node) {
    nodes_by_id_.insert(make_pair(new_node->getId(), ref(*new_node)));
    auto loc = new_node->getLocation();
    // Add it to a map of containers ordeder by size to the origin (0, 0)
    auto ci = nodes_by_distance_.insert(make_pair(loc.getDistanceToOrigin(), new_node)).first;

    if (new_node->getId() != 1) {
        cout << "+Node " << new_node->getId() << " @ distance " << loc.getDistanceTo(getNodeById(1).getLocation()) << endl;
    } else {
        cout << "+Node " << new_node->getId() << " @ distance " << 0. << endl;
    }
    /* Find possible neighbours
     * 
     * We only find those nodes that are further that (lat-range. lon-range) from
     * the origin, but closer than (lat+range, lon+range */
    const double minDistance = (loc - Location(tx_range_, tx_range_)).getDistanceToOrigin();
    const double maxDistance = (loc + Location(tx_range_, tx_range_)).getDistanceToOrigin();

    auto ri = reverse_iterator<map<double, shared_ptr<Node > >::iterator>(ci);
    for (auto i = ri; i != nodes_by_distance_.rend() && i->first >= minDistance; i++) {
        if (new_node->getLocation().getSquaredDistanceTo(i->second->getLocation()) <= sq_tx_range_) {
            new_node->addNeightbour(*i->second);
            i->second->addNeightbour(*new_node);
        }
    }
    for (auto i = next(ci); i != nodes_by_distance_.end() && i->first <= maxDistance; i++) {
        if (new_node->getLocation().getSquaredDistanceTo(i->second->getLocation()) <= sq_tx_range_) {
            new_node->addNeightbour(*i->second);
            i->second->addNeightbour(*new_node);
        }
    }
}

#ifndef NDEBUG

ostream& operator<<(ostream& os, const Grid& g) {
    os << "Grid with transmission range: " << g.tx_range_ << endl;
    for (auto n : g.nodes_by_distance_)
        os << *n.second << endl;

    return os;
}
#endif