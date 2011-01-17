#include "utils.h"

#include <math.h>

int
almost_equal_eps (double x, double y, double eps)
{
    if (x == 0) return fabs (x) < eps;
    if (y == 0) return fabs (y) < eps;
    double max = (fabs (x) > fabs (y)) ? fabs (x) : fabs (y);
    return (fabs (x - y) / max) < eps;
}
