//
//  rational.c
//  CPT_S 360 Lab 4
//
//  Created by Raul Martinez on 2/9/25.
//
#include <stdio.h>  // For snprintf()
#include <stdlib.h> // For abs()
#include <limits.h> // For INT_MAX and INT_MIN
#include "rational.h"

// Helper function to compute GCD using Euclid's algorithm
static int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Simplifies a Rational number
static Rational rtnl_simplify(Rational rtnl) {
    if (rtnl.denom == 0) {
        fprintf(stderr, "Error: Division by zero in Rational number.\n");
        exit(EXIT_FAILURE);
    }
    
    int divisor = gcd(abs(rtnl.num), abs(rtnl.denom));
    rtnl.num /= divisor;
    rtnl.denom /= divisor;

    // Ensure denominator is positive
    if (rtnl.denom < 0) {
        rtnl.num = -rtnl.num;
        rtnl.denom = -rtnl.denom;
    }

    return rtnl;
}

// Initializes a Rational number and simplifies it
Rational rtnl_init(int num, int denom) {
    Rational rtnl = { num, denom };
    return rtnl_simplify(rtnl);
}

// Adds two Rational numbers
Rational rtnl_add(Rational rtnl0, Rational rtnl1) {
    Rational result;
    result.num = rtnl0.num * rtnl1.denom + rtnl1.num * rtnl0.denom;
    result.denom = rtnl0.denom * rtnl1.denom;
    return rtnl_simplify(result);
}

// Subtracts two Rational numbers
Rational rtnl_sub(Rational rtnl0, Rational rtnl1) {
    return rtnl_add(rtnl0, rtnl_init(-rtnl1.num, rtnl1.denom));
}

// Multiplies two Rational numbers
Rational rtnl_mul(Rational rtnl0, Rational rtnl1) {
    Rational result;
    result.num = rtnl0.num * rtnl1.num;
    result.denom = rtnl0.denom * rtnl1.denom;
    return rtnl_simplify(result);
}

// Divides two Rational numbers
Rational rtnl_div(Rational rtnl0, Rational rtnl1) {
    if (rtnl1.num == 0) {
        fprintf(stderr, "Error: Division by zero in Rational division.\n");
        exit(EXIT_FAILURE);
    }
    
    Rational result;
    result.num = rtnl0.num * rtnl1.denom;
    result.denom = rtnl0.denom * rtnl1.num;
    return rtnl_simplify(result);
}

// Raises a Rational number to an integer power
Rational rtnl_ipow(Rational rtnl, int ipow) {
    if (ipow == 0) return rtnl_init(1, 1);

    if (ipow < 0) {
        if (rtnl.num == 0) {
            fprintf(stderr, "Error: Zero raised to a negative power.\n");
            exit(EXIT_FAILURE);
        }
        Rational inverse = { rtnl.denom, rtnl.num };
        return rtnl_ipow(inverse, -ipow);
    }

    Rational result = rtnl_init(1, 1);
    while (ipow--) {
        // Check for overflow before multiplying
        if ((long long)result.num * rtnl.num > INT_MAX || (long long)result.num * rtnl.num < INT_MIN ||
            (long long)result.denom * rtnl.denom > INT_MAX || (long long)result.denom * rtnl.denom < INT_MIN) {
            fprintf(stderr, "Error: Integer overflow detected at exponentiation step.\n");
            exit(EXIT_FAILURE);
        }

        result.num *= rtnl.num;
        result.denom *= rtnl.denom;
        result = rtnl_simplify(result);
    }

    return result;
}

// Converts a Rational number to a string representation
char *rtnl_asStr(Rational rtnl, char buf[RTNL_BUF_SIZE]) {
    snprintf(buf, RTNL_BUF_SIZE, "(%d/%d)", rtnl.num, rtnl.denom);
    return buf;
}
