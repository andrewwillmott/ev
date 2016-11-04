//
//  File:       CLExpr.cpp
//
//  Function:   Basic expression evaluator
//
//  Author(s):  Andrew Willmott
//

#include "CLExpr.h"

#include <math.h>
#include <limits.h> // for int limits
#include <float.h>  // for float limits

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>  // for ReportError example

using namespace nCL;

namespace
{
    const double kPi = 3.14159265358979323846;

    inline double RadiansFromDegrees(double degs)
    { return degs * (kPi / 180); }

    inline double DegreesFromRadians(double rads)
    { return rads * (180 / kPi); }


    struct cEvalContext
    {
        const char* s;
        cEvalError* error;
        
        void Error(const char* message, size_t range = 0);
    };

    void cEvalContext::Error(const char* message, size_t range)
    {
        if (error && !error->mMessage) // add only if 'error' exists and we haven't already recorded something
        {
            error->mMessage = message;
            error->mBegin   = s;
            error->mEnd     = s + range;
        }
    }

    double EvalExpression(cEvalContext* context);
}


// ----------------------------------------------------------------------------
// Numerical expression parser
//
// Expression   -> Term [+|- Term]*
// Term         -> Factor [*/% Factor]*
// Factor       -> SignedNumber [^ Factor]
// SignedNumber ->    + SignedNumber
//                  | - SignedNumber
//                  | Number
//
// Number       ->    Function ( Expression )
//                  | Function ( Expression, Expression )
//                  | {. | digit} PositiveConstant
// ----------------------------------------------------------------------------

double nCL::EvalExpressionDouble(const char* s, cEvalError* error)
{
    cEvalContext context = { s, error };

    if (*s == 0)
        context.Error("Empty expression");
    
    double result = EvalExpression(&context);
    
    if (*context.s != 0)
        // we didn't get to the end of the string.
        context.Error("Garbage at end of expression", strlen(context.s));
    
    return result;
}

float nCL::EvalExpressionFloat(const char* s, cEvalError* error)
{
    cEvalContext context = { s, error };

    if (*s == 0)
        context.Error("Empty expression");
    
    double result = EvalExpression(&context);

    if (result < -FLT_MAX || result > FLT_MAX)
        context.Error("Float out of range");
    
    if (*context.s != 0)
        // we didn't get to the end of the string.
        context.Error("Garbage at end of expression", strlen(context.s));
    
    return float(result);
}

int32_t nCL::EvalExpressionInt32(const char* s, cEvalError* error)
{
    cEvalContext context = { s, error };

    if (*s == 0)
        context.Error("Empty expression");
    
    double result = EvalExpression(&context);

    if (result < INT32_MIN || result > INT32_MAX)
        context.Error("Signed integer out of range");
    
    if (*context.s != 0)
        // we didn't get to the end of the string.
        context.Error("Garbage at end of expression", strlen(context.s));
    
    return int32_t(result);
}

uint32_t nCL::EvalExpressionUInt32(const char* s, cEvalError* error)
{
    cEvalContext context = { s, error };

    if (*s == 0)
        context.Error("Empty expression");
    
    double result = EvalExpression(&context);

    if (result < 0 || result > UINT32_MAX)
        context.Error("Unsigned integer out of range");
    
    if (*context.s != 0)
        // we didn't get to the end of the string.
        context.Error("Garbage at end of expression", strlen(context.s));
    
    return uint32_t(result);
}


bool nCL::ReportError(const cEvalError& error)
{
    if (!error)
        return false;

    if (error.mEnd != error.mBegin)
        fprintf(stderr, "%s: %.*s\n", error.mMessage, int(error.mEnd - error.mBegin), error.mBegin);
    else
        fprintf(stderr, "%s: %s\n", error.mMessage, error.mBegin);
    
    return true;
}


// ----------------------------------------------------------------------------

namespace
{
    double EvalTerm             (cEvalContext* context);
    double EvalFactor           (cEvalContext* context);
    double EvalSignedNumber     (cEvalContext* context);
    double EvalNumber           (cEvalContext* context);
    double EvalPositiveConstant (cEvalContext* context);
    void   EvalCharConst        (cEvalContext* context, char c);
    
    double EvalParentheses (cEvalContext* context);
    void   EvalParentheses2(cEvalContext* context, double& arg1, double& arg2);
    
    inline void DiscardWhiteSpace(cEvalContext* context)
    {
        while (isspace(*context->s))
        {
            context->s++;
        }
    }
    
    double EvalExpression(cEvalContext* context)
    {
        double result = EvalTerm(context);
    
        while (1)
        {
            DiscardWhiteSpace(context);
    
            switch (*context->s)
            {
            case '+':
                context->s++;
                result += EvalTerm(context);
                break;
            case '-':
                context->s++;
                result -= EvalTerm(context);
                break;
            default:
                return result;
            }
        }
    }
    
    double EvalTerm(cEvalContext* context)
    {
        double result = EvalFactor(context);
    
        while (true)
        {
            DiscardWhiteSpace(context);
    
            switch (*context->s)
            {
            case '*':
                context->s++;
                result *= EvalFactor(context);
                break;
            case '/':
                context->s++;
                result /= EvalFactor(context);
                break;
            case '%':
                {
                    context->s++;
                    double mod = EvalFactor(context);
                    result = result - floor(result / mod) * mod;
                }
                break;
            default:
                return result;
            }
        }
    }
    
    double EvalFactor(cEvalContext* context)
    {
        double result = EvalSignedNumber(context);
    
        DiscardWhiteSpace(context);
    
        if (*context->s == '^')
        {
            context->s++;
            return pow(result, EvalFactor(context));    // right-associative
        }
    
    
        return result;
    }
    
    
    double EvalSignedNumber(cEvalContext* context)
    {
        DiscardWhiteSpace(context);
    
        switch (*context->s)
        {
        case '-':
            context->s++;
            return -EvalSignedNumber(context);
        case '+':
            context->s++;
            return EvalSignedNumber(context);
        }
    
        return EvalNumber(context);
    }
    
    inline bool IsIdentifier(char c)
    {
        return isalnum(c) || c == '_';
    }
    
    inline bool Token(const char* s0, const char* s1, const char* d)
    {
        for ( ; s0 < s1; s0++, d++)
            if (*s0 != *d)
                return false;
        
        return (s0 == s1) && *d==0;
    }

    double EvalNumber(cEvalContext* context)
    {
        DiscardWhiteSpace(context);
    
        if (isdigit(*context->s) || *context->s == '.')
            return EvalPositiveConstant(context);

        if (*context->s == '(')
            return EvalParentheses(context);
    
        if (isalpha(*context->s))
        {
            const char* s0 = context->s;
            const char* s1 = s0;
    
            while (IsIdentifier(*s1))
                s1++;

            context->s = s1;
    
            if (Token(s0, s1, "pi"))       return kPi;
            if (Token(s0, s1, "e"))        return exp(1.0);
    
            if (Token(s0, s1, "sqrt"))     return sqrt(EvalParentheses(context));
            if (Token(s0, s1, "exp"))      return exp (EvalParentheses(context));
            if (Token(s0, s1, "log"))      return log (EvalParentheses(context));
            if (Token(s0, s1, "abs"))      return fabs(EvalParentheses(context));
    
            if (Token(s0, s1, "sin"))      return sin (EvalParentheses(context));
            if (Token(s0, s1, "cos"))      return cos (EvalParentheses(context));
            if (Token(s0, s1, "tan"))      return tan (EvalParentheses(context));
            if (Token(s0, s1, "asin"))     return asin(EvalParentheses(context));
            if (Token(s0, s1, "acos"))     return acos(EvalParentheses(context));
            if (Token(s0, s1, "atan"))     return atan(EvalParentheses(context));
    
            if (Token(s0, s1, "sind"))     return sin (RadiansFromDegrees(EvalParentheses(context)));
            if (Token(s0, s1, "cosd"))     return cos (RadiansFromDegrees(EvalParentheses(context)));
            if (Token(s0, s1, "tand"))     return tan (RadiansFromDegrees(EvalParentheses(context)));
    
            if (Token(s0, s1, "dasin"))    return DegreesFromRadians(asin(EvalParentheses(context)));
            if (Token(s0, s1, "dacos"))    return DegreesFromRadians(acos(EvalParentheses(context)));
            if (Token(s0, s1, "datan"))    return DegreesFromRadians(atan(EvalParentheses(context)));
    
            if (Token(s0, s1, "floor"))    return floor(EvalParentheses(context));
            if (Token(s0, s1, "ceil"))     return ceil (EvalParentheses(context));
    
            if (Token(s0, s1, "sqr"))
            {
                double x = EvalParentheses(context);
                return x * x;
            }
    
            if (Token(s0, s1, "pow"))
            {
                double x, y;
                EvalParentheses2(context, x, y);
                return pow(x, y);
            }
    
            if (Token(s0, s1, "atan2"))
            {
                double x, y;
                EvalParentheses2(context, x, y);
                return atan2(x, y);
            }
    
            if (Token(s0, s1, "datan2"))
            {
                double x, y;
                EvalParentheses2(context, x, y);
                return DegreesFromRadians(atan2(x, y));
            }
    
            context->s = s0;    // roll back
            context->Error("Unknown function", s1 - s0);
        }
        else
            context->Error("Bad numerical expression");

        return 0.0;
    }
    
    double EvalParentheses(cEvalContext* context)
    {
        double result;
    
        EvalCharConst(context, '(');
        result = EvalExpression(context);
        EvalCharConst(context, ')');
    
        return result;
    }
    
    void EvalParentheses2(cEvalContext* context, double& arg1, double& arg2)
    {
        EvalCharConst(context, '(');
        arg1 = EvalExpression(context);
        EvalCharConst(context, ',');
        arg2 = EvalExpression(context);
        EvalCharConst(context, ')');
    }
    
    double EvalPositiveConstant(cEvalContext* context)
    {
        char* endS;
    
        double result;
        if (context->s[0] == '0' && context->s[1] == 'x') // have to handle hex numbers differently.
        {
            uint32_t resultx = strtoul(context->s, &endS, 0);
            result = resultx;
        }
        else
            result = strtod(context->s, &endS);
    
        context->s = endS;
    
        return result;
    }
    
    void EvalCharConst(cEvalContext* context, char c)
    {
        DiscardWhiteSpace(context);
    
        if (*context->s != c)
        {
            static char sErrorMessage[] = "Expected '?'";
            
            sErrorMessage[10] = c;
            context->Error(sErrorMessage, 1);
            return;
        }
    
        context->s++;
    }
}
