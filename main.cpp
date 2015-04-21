/* 
 * File:   main.cpp
 * Author: miguel
 *
 * Created on 4 de febrero de 2015, 13:29
 */

#include <iostream>

#include "Grid.h"
#include "RINode.h"
#include "PeriodicTrafficSource.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    auto d = Grid(5);

    d.addNode(shared_ptr<Node> (new RINode(Location(1, 1))));
    d.addNode(shared_ptr<Node> (new RINode(Location(5, 5))));
    d.addNode(shared_ptr<Node> (new RINode(Location(10, 10))));
    d.addNode(shared_ptr<Node> (new RINode(Location(7, 7))));
    d.addNode(shared_ptr<Node> (new RINode(Location(2, 2))));
    d.addNode(shared_ptr<Node> (new RINode(Location(4.5, 4.5))));
    d.addNode(shared_ptr<Node> (new RINode(Location(6, 3))));
    d.addNode(shared_ptr<Node> (new RINode(Location(0, 0))));

    cerr << d << endl;

    auto tSource = make_shared<PeriodicTrafficSource>(d, 1/10.);
    
    Calendar::getCalendar().run();

    return 0;
}

