//
//  File:       CLExpr.h
//
//  Function:   Basic expression evaluator
//
//  Author(s):  Andrew Willmott
//
//  Copyright:  1998-2015
//

#ifndef CL_EXPRESSION_H
#define CL_EXPRESSION_H

#include <stdint.h>

namespace nCL
{
    struct cEvalError;

    double   EvalExpressionDouble(const char* s, cEvalError* error = 0);
    float    EvalExpressionFloat (const char* s, cEvalError* error = 0);

    int32_t  EvalExpressionInt32 (const char* s, cEvalError* error = 0);
    uint32_t EvalExpressionUInt32(const char* s, cEvalError* error = 0);


    // Error control
    struct cEvalError
    {
        const char* mMessage = 0;    //!< String indicating error
        const char* mBegin   = 0;    //!< Start of error location
        const char* mEnd     = 0;    //!< end of error location, may be equal to mStart if we only have a cursor rather than a range
        
        operator bool() const { return mMessage != 0; }
    };

    bool ReportError(const cEvalError& error);  //!< Prints error to standard error stream. Meant as a simple example. Returns true if there was an error.
}


// -----------------------------------------------------------------------------
// Inlines
// -----------------------------------------------------------------------------




#endif
