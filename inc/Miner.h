#ifndef MINER_H
#define MINER_H

#include <queue>
#include <mutex>
#include <execution>
#include <atomic>

#include "InfoGain.h"
#include "Fptree.h"
#include "SimpleStructures.h"

class Miner {
public:
	DS* dataset;
	Parameters params;
	bool remove_patterns_flag;
	bool pattern_expansion_flag;
	FinalPatternSet final_set;
	std::atomic<long> number_of_trails;

	Miner(DS* dataset, Parameters& params);
	void mine();

private:
	double get_initial_bound(FPTree& fptree, std::vector<Feature>& singletons);
	void singleton_iteration(FPTree& fptree, std::vector<Feature>& singletons, double& bound);
	void branch_and_bound(FPTree& fptree, double& max_IG, Feature& best, double bound, Itemset& alpha);
	void continue_mining(FPTree& fptree, double& max_IG, Feature& best, double bound, Itemset& alpha);
};

#endif // !MINER_H
