ev
==

Simple command-line expression parser.

To build and run:

    c++ CLExpr.cpp EV.cpp -o ev && ./ev

Or add those files to your favourite IDE.


Usage:

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
    ev -p 3 "1/3"

Operators:

    + -   * / %   ^

Constants:

    pi
    e

Functions:

    sqrt(x)
    sqr(x)
    exp(x)
    pow(y, x)
    log(x)
    erf(x)

    abs(x)
    floor(x)
    ceil(x)
    round(x)

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
