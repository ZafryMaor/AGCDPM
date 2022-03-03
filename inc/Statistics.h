#ifndef STATISTICS_H
#define STATISTICS_H

#include <cmath>
#include <vector>
#include <iostream>

#include "SimpleStructures.h"

void calculate_statistical_significance(std::vector<std::pair<Feature, Statistics>>& patterns_statistical_significance,
	double origins_labeld_1_counter, double number_of_origins, double number_of_trails);

Statistics get_statistics(Itemset& itemset, std::vector<std::pair<Feature, Statistics>>& patterns_statistical_significance);

#endif // !STATISTICS_H