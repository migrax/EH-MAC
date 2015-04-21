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
    
    void addEntry(unsigned int destId, Node& nextNode) {        
        table.insert(std::pair<unsigned int, Node&>(destId, nextNode));               
    }
    
    auto getNext(unsigned int destId) const {
        auto ci = table.find(destId);
        return (ci != table.end()) ? &ci->second : nullNode;        
    }
private:

    std::map<unsigned int, Node&> table;
    Node *nullNode;
};

#endif	/* ROUTINGTABLE_H */

