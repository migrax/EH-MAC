/* 
 * File:   main.cpp
 * Author: miguel
 *
 * Created on 4 de febrero de 2015, 13:29
 */

#include <iostream>
#include <string>
#include <unistd.h>

#include "Grid.h"
#include "RINode.h"
#include "PeriodicTrafficSource.h"
#include "HomogeneousSpatialPareto.h"
#include "PWNode.h"
#include "EnhNode.h"

using namespace std;

namespace {

    void usage(const string& progname) {
        cerr << progname << ": [-s seed] [-l side] [-m max_time] [-d node density] [-r per node rate] [-a R|P|E]" << endl;
    }
}

/*
 * 
 */
int main(int argc, char** argv) {
    auto seed = 0;
    auto size = 1;
    auto density = 10.;
    auto rate = 1 / 10.;
    auto max_time = 50;
    int opt;

    enum nodetype {
        RIMAC, PWMAC, ENHMAC
    } node = RIMAC;

    while ((opt = getopt(argc, argv, "s:l:m:d:r:a:")) != -1) {
        switch (opt) {
            case 's': seed = atoi(optarg);
                break;
            case 'l': size = atoi(optarg);
                break;
            case 'm': max_time = atoi(optarg);
                break;
            case 'd': density = atof(optarg);
                break;
            case 'r': rate = atof(optarg);
                break;
            case 'a': switch (optarg[0]) {
                    case 'R': node = RIMAC;
                        break;
                    case 'P': node = PWMAC;
                        break;
                    case 'E': node = ENHMAC;
                        break;
                };
                break;
            default: usage(argv[0]);
                return 1;
        }
    }

    auto& calendar = Calendar::getCalendar();
    calendar.setRandomSeed(seed);
    auto hsp = HomogeneousSpatialPareto<boost::mt19937>(density, Calendar::getCalendar().getRandomGenerator(), size);

    /*auto distance_info = hsp.getMaximumNeighbouringDistance();
    cerr << "The most separated neighboring nodes are: " << get<1>(distance_info) << " and " << get<2>(distance_info) << ". Separated by " << get<0>(distance_info) << endl;*/

    auto distance_info = hsp.getMaximumNeighbouringDistanceToOrigin();
    cerr << "The neighboring with the furthest next host are: " << get<1>(distance_info) << " and " << get<2>(distance_info) << ". Separated by " << get<0>(distance_info) << endl;

    auto grid = Grid(1.01 * get<0>(distance_info));

    for (auto p : hsp.getPoints()) {
        switch (node) {
            case RIMAC: grid.addNode(make_shared<RINode>(p));
                break;
            case PWMAC: grid.addNode(make_shared<PWNode>(p));
                break;
            case ENHMAC: grid.addNode(make_shared<EnhNode>(p));
                break;
        }
    }

    PeriodicTrafficSource(grid, rate);

    try {
        calendar.run(max_time);
    } catch (const SimulationException& e) {
        cerr << endl
                << e.getMessage() << endl;
        e.debug(cerr) << endl;
    }

    return 0;
}

