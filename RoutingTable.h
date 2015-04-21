/* 
 * File:   RoutingTable.h
 * Author: miguel
 *
 * Created on 5 de febrero de 2015, 11:00
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

