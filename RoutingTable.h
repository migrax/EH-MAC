/* 
 * File:   RoutingTable.h
 * Author: miguel
 *
 * Created on 5 de febrero de 2015, 11:00
 */

#ifndef ROUTINGTABLE_H
#define	ROUTINGTABLE_H

#include <memory>
#include <map>

class Node;

class RoutingTable {
public:
    RoutingTable() : nullNode(nullptr) {}
    
    void addEntry(unsigned int destId, std::shared_ptr<Node> nextNode) {
        table[destId] = nextNode;
    }
    
    std::shared_ptr<Node> getNext(unsigned int destId) const {
        auto ci = table.find(destId);
        return (ci != table.end()) ? ci->second : nullNode;        
    }
private:

    std::map<unsigned int, std::shared_ptr<Node> > table;
    const std::shared_ptr<Node> nullNode;
};

#endif	/* ROUTINGTABLE_H */

