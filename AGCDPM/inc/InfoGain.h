#ifndef INFOGAIN_H
#define INFOGAIN_H

#include <math.h> 
#include <algorithm>

double InfoGain(const double& t, const double& p, const double& q);
double InfoGainUB(const double& p, const double& t);

#endif
