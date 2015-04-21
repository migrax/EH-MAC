/* 
 * File:   Grid.cpp
 * Author: miguel
 * 
 * Created on 4 de febrero de 2015, 15:56
 */

#include "Grid.h"
#include "Node.h"

#include <assert.h>

using namespace std;

Grid::Grid(double transmissionRange) : txRange(transmissionRange), sqTxRange(transmissionRange*transmissionRange) {
    assert(txRange > 0);
}

void Grid::addNode(shared_ptr<Node> newNode) {
    nodesById.insert(make_pair(newNode->getId(), ref(*newNode)));    
    auto loc = newNode->getLocation();
    // Add it to a map of containers ordeder by size to the origin (0, 0)
    auto ci = nodesByDistance.insert(make_pair(loc.getDistanceToOrigin(), newNode)).first;
    
    /* Find possible neighbours
     * 
     * We only find those nodes that are further that (lat-range. lon-range) from
     * the origin, but closer than (lat+range, lon+range */
    const double minDistance = (loc - Location(txRange, txRange)).getDistanceToOrigin();
    const double maxDistance = (loc + Location(txRange, txRange)).getDistanceToOrigin();  
    
    auto ri = reverse_iterator<map<double, shared_ptr<Node > >::iterator>(ci);    
    for (auto i = ri; i != nodesByDistance.rend() && i->first >= minDistance; i++) {
        if (newNode->getLocation().getSquaredDistanceTo(i->second->getLocation()) <= sqTxRange) {
            newNode->addNeightbour(*i->second);
            i->second->addNeightbour(*newNode);
        }
    }
    for (auto i = next(ci); i != nodesByDistance.end() && i->first <= maxDistance; i++) {
        if (newNode->getLocation().getSquaredDistanceTo(i->second->getLocation()) <= sqTxRange) {
            newNode->addNeightbour(*i->second);
            i->second->addNeightbour(*newNode);
        }        
    }
}

#ifndef NDEBUG
    ostream& operator<<(ostream& os, const Grid& g) {
        os << "Grid with transmission range: " << g.txRange << endl;
        for (auto n : g.nodesByDistance)
            os << *n.second << endl;
        
        return os;
    }
#endif