/* 
 * File:   RINode.h
 * Author: miguel
 *
 * Created on 10 de febrero de 2015, 15:46
 */

#ifndef RINODE_H
#define	RINODE_H

#include "RIDriver.h"
#include "Node.h"

class RINode : public Node {
public:
    RINode(const Location& loc) : Node(loc, std::make_unique<RIDriver>(8/Link::getCapacity())) {}    
};

#endif	/* RINODE_H */

