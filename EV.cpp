//
//  File:       EV.cpp
//
//  Function:   Command-line access to expression evaluator
//
//  Author(s):  Andrew Willmott
//


#include "CLExpr.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

using namespace nCL;

int main(int argc, char const** argv)
{
    const char* command = argv[0];
    argc--;
    argv++;

    bool show_hex = false;
    bool show_int = false;
    bool show_uint = false;
    int precision = 17;

    while (argc > 0 && argv[0][0] == '-' && isalpha(argv[0][1]))
    {
        const char* option = argv[0] + 1;

        argc--;
        argv++;

        switch (option[0])
        {
        case 'x':
            show_hex = true;
            break;
        case 'i':
            show_int = true;
            break;
        case 'u':
            show_uint = true;
            break;
        case 'p':
            if (argc > 0)
            {
                precision = atoi(argv[0]);
                argc--;
                argv++;
            }
            break;
        default:
            printf("unknown option: %s\n", option);
            return -1;
        }
    }

    if (argc != 1)
    {
        printf("%s [options] <expression>\n"
            "  Evaluate the given expression.\n"
            "Options:\n"
            "  -i     Show result as a 32-bit integer\n"
            "  -u     Show result as an unsigned 32-bit integer\n"
            "  -x     Show result as hex\n"
            "  -p <n> Set output precision\n"
            "Example:\n"
            "  %s \"1 + 2 * 3\"\n"
            , command, command);

        return -1;
    }

    cEvalError error;

    if (show_hex)
    {
        double x = EvalExpressionDouble(argv[0], &error);

        if (!error)
            printf("0x%08X\n", (unsigned int)(x));
    }
    else if (show_int)
    {
        int32_t x = EvalExpressionInt32(argv[0], &error);

        if (!error)
            printf("%d\n", x);
    }
    else if (show_uint)
    {
        uint32_t x = EvalExpressionUInt32(argv[0], &error);

        if (!error)
            printf("%u\n", x);
    }
    else
    {
        double x = EvalExpressionDouble(argv[0], &error);

        if (!error)
            printf("%0.*g\n", precision, x);
    }

    if (error)
    {
        ReportError(error);
        return -1;
    }

    return 0;
}
