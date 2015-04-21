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


#ifndef ROUTINGTABLE_H
#define	ROUTINGTABLE_H

#include <map>

class Node;

class RoutingTable {
public:
    RoutingTable() : nullNode(nullptr) {}
    
    void addEntry(unsigned int destId, Node& next_node) {        
        table.insert(std::pair<unsigned int, Node&>(destId, next_node));               
    }
    
    auto getNext(unsigned int dest_id) const {
        auto ci = table.find(dest_id);
        return (ci != table.end()) ? &ci->second : nullNode;        
    }
private:

    std::map<unsigned int, Node&> table;
    Node *nullNode;
};

#endif	/* ROUTINGTABLE_H */

