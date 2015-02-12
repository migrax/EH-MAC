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
    RINode(const Location& loc) : Node(loc, std::shared_ptr<DutyDriver> (new RIDriver())) {}
    
private:

};

#endif	/* RINODE_H */

