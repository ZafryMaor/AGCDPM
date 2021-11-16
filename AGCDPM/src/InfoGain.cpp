#include "InfoGain.h"

double plogp(const double& p) {
	if (p <= 0)
		return 0;
	return p * log2(p);
}
double InfoGain(const double& t, const double& p, const double& q) {

	double EnDS = -(plogp(1 - p) + plogp(p));
	double s1 = -t * plogp(q);
	if (t == 0 || q == 1 || q == 0) s1 = 0;
	double s2 = -t * plogp(1 - q);
	if (t == 0 || q == 1 || q == 0) s2 = 0;
	double s3 = (t * q - p) * log2((p - t * q) / (1 - t));
	if (t == 1 || q * t == p || (p - t * q) == (1 - t)) s3 = 0;
	double s4 = (t * (1 - q) - (1 - p)) * log2(((1 - p) - t * (1 - q)) / (1 - t));
	if (t == 1 || (1 - p) == t * (1 - q)) s4 = 0;
	double condEn = s1 + s2 + s3 + s4;
	return EnDS - condEn;
}

double InfoGainUB(const double& p, const double& t) {
	if (t <= p) {
		return std::max(InfoGain(t, p, 0), InfoGain(t, p, 1));
	}
	return std::max(InfoGain(t, p, p / t), InfoGain(t, p, 1 - (1 - p) / t));
}