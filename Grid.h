/* 
 * File:   Grid.h
 * Author: miguel
 * 
 * The Universe where the Nodes reside
 */

#ifndef GRID_H
#define	GRID_H

#include "Location.h"
#include "Node.h"

#include <memory>
#include <map>

#include <cassert>

#ifndef NDEBUG
#include <ostream>
#endif

class Grid {
public:
    Grid(double transmissionRange);

    void addNode(std::shared_ptr<Node>);
    
    Node& getNodeById(Node::nodeid_t nodeId) const {
        assert(nodesById.find(nodeId) != nodesById.end());
        
        return nodesById.find(nodeId)->second;        
    }
    
    auto getNodeLocation(Node::nodeid_t nodeId) const {                        
        Node& node = getNodeById(nodeId);
        auto res = node.getLocation();
        
        return res;
    }
        
    auto countDeployedNodes() const {
        return nodesById.size();
    }

#ifndef NDEBUG
    friend std::ostream& operator<<(std::ostream&, const Grid&);
#endif
private:
    const double txRange, sqTxRange;
    std::map<double, std::shared_ptr<Node > > nodesByDistance;
    std::map<Node::nodeid_t, Node&> nodesById;
};

#endif	/* GRID_H */

