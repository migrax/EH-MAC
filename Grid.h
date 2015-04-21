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
    Grid(Location::value_type transmission_range);

    void addNode(std::shared_ptr<Node>);
    
    Node& getNodeById(Node::nodeid_t node_id) const {
        assert(nodes_by_id_.find(node_id) != nodes_by_id_.end());
        
        return nodes_by_id_.find(node_id)->second;        
    }
    
    auto getNodeLocation(Node::nodeid_t node_id) const {                        
        Node& node = getNodeById(node_id);
        auto res = node.getLocation();
        
        return res;
    }
        
    auto countDeployedNodes() const {
        return nodes_by_id_.size();
    }

#ifndef NDEBUG
    friend std::ostream& operator<<(std::ostream&, const Grid&);
#endif
private:
    const Location::value_type tx_range_, sq_tx_range_;
    std::map<Location::value_type, std::shared_ptr<Node > > nodes_by_distance_;
    std::map<Node::nodeid_t, Node&> nodes_by_id_;
};

#endif	/* GRID_H */

