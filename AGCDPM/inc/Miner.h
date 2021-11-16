#ifndef MINER_H
#define MINER_H

#include <queue>
#include <mutex>
#include <execution>

#include "InfoGain.h"
#include "FPTree.h"
#include "Util.h"

PatternSet mine(DS transactions, double min_sup_percent, int k, int l,
	double origins_labeld_1_counter, double number_of_origins, bool verbose_flag);


#endif // !MINER_H
