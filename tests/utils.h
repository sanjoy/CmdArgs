#ifndef __CMDARGS__UTILS__H
#define __CMDARGS__UTILS__H

/* Test that first and second arguments are approximately equal by
 * computing the difference, rounding to the given number of decimal
 * places (defaults to 7) */

#ifndef CMDARGS_UTIL_DEFAULT_TOLERANCE
/* This can be defined before including the header,
 * to override default tolerance. */

#define CMDARGS_UTIL_DEFAULT_TOLERANCE 1e-7
#endif

int almost_equal_eps (double x, double y, double eps);

#define almost_equal(x, y) almost_equal_eps(x, y, CMDARGS_UTIL_DEFAULT_TOLERANCE)

#endif /* __CMDARGS__UTILS__H */
