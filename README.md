ev
==

Simple command-line expression parser

To build and run:

    c++ CLExpr.cpp EV.cpp -o ev && ./ev

Examples:

    ev "1 + 2 + 3 * 4 / sin(5.678)"
    ev -x "3735928559"
