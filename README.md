ev
==

Simple command-line expression parser

To build and run:

    c++ CLExpr.cpp EV.cpp -o ev && ./ev

This tool is capable of producing top-down hemisphere, cube-map, and panoramic
views. See "sunsky -h" for options. 

Examples:

    ev "1 + 2 + 3 * 4 / sin(5.678)"
