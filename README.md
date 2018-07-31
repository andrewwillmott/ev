ev
==

Simple command-line expression parser.

To build and run:

    c++ CLExpr.cpp EV.cpp -o ev && ./ev

Help:

    ev [options] <expression>
      Evaluate the given expression.
    Options:
      -i     Show result as a 32-bit integer
      -u     Show result as an unsigned 32-bit integer
      -x     Show result as hex
      -p <n> Set output precision

Examples:

    ev "1 + 2 + 3 * 4 / sin(5.678)"
    ev -x "3735928559"
    ev -x "3735928559"
    ev -p 3 "1/3"

Supports:

    pi
    e

    sqrt(x)
    sqr(x)
    exp(x)
    pow(y, x)
    log(x)
    abs(x)

    sin(radians)
    cos(radians)
    tan(radians)

    asin(x)
    acos(x)
    atan(x)
    atan2(y, x)

    sind(degrees)
    cosd(degrees)
    tand(degrees)

    dasin(x)
    dacos(x)
    datan(x)
    datan2(y, x)

    floor(x)
    ceil(x)
