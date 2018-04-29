enmac
=====

A simulator for the Enhanced Predictive Wakeup MAC Algorithm

Overview
--------

This is the simulated for the Enhanced Predictive MAC Algorithm described in the Paper ["A Self-Tuning Receiver Initiated MAC protocol for
Wireless Sensor Networks"], currently being reviewed for publication.

Usage
-----

The simulator simulates a field of wireless sensor in an square field with an arbitrary sensor density. The program is able
to simulate three different MAC algorithms: the original RI-MAC, PW-MAC and our Enhanced version.
Nodes are placed in the field according to a homogeneous Poisson distribution, while packet transmission at originating
nodes can either be deterministic or follow a Poisson distribution.

Invocation
----------

`enmac [-s seed] [-l side] [-m max_time] [-d node density] [-r per node rate] [-a R|P|E]`

where side is the length of each side of the field, max_time is the siomulation time in seconds,
and R, P and E correspond to the three implemented MAC algorithms.


Output
------

The simulator outputs a line for every important event. The output should be self-explanatory.

Legal
-----

Copyright ⓒ Miguel Rodríguez Pérez <miguel@det.uvigo.es> 2015

This simulator is licensed under the GNU General Public License, version 3 (GPL-3.0). For for information see COPYING.
