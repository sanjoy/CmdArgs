#ifndef __TEST__H
#define __TEST__H

#include <math.h>

// Test that first and second arguments are approximately equal by
// computing the difference, rounding to the given number of decimal
// places (defaults to 7)
int almost_equal_eps(double x, double y, double eps);
int almost_equal(double x, double y);

#endif /* __TEST__H */
