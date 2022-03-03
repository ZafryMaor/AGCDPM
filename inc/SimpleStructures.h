#ifndef SIMPLESTRUCTURES_H
#define SIMPLESTRUCTURES_H

#include <vector>
#include <queue>
#include <set>
#include <iostream>
#include <map>
#include <algorithm>

enum InputErrorType { none, arg, param, ds, annofile, ptrfile, ptefile };

struct Parameters {
	double min_sup;
	size_t k;
	size_t l;

	Parameters();
};

using Item = uint64_t;
using Itemset = std::vector<Item>;
using Origin = std::string;
using Label = int;

struct Transaction {
	Origin origin;
	Itemset itemset;
	Label label;
	
	Transaction(Origin origin, Itemset& itemset, Label label);
};

struct DS {
	std::vector<Transaction> transactions;
	double number_of_origins;
	double number_of_origins_labeld_1;
	double number_of_origins_labeld_0;
	std::map<uint64_t, std::string> item_by_id;
	std::map<std::string, uint64_t> id_by_item;
	std::vector<Itemset> patterns_to_remove;
	Itemset pattern_to_expand;

	DS();
};

struct Feature {
	Itemset itemset;
	double ig;
	double sup_0;
	double sup_1;

	Feature(Itemset& itemset, double ig, double sup_0, double sup_1);
};

class featureComparator
{
public:
	int operator() (const Feature& f1, const Feature& f2)
	{
		return f1.ig > f2.ig;
	}
};

using FinalPatternSet = std::priority_queue<Feature, std::vector<Feature>, featureComparator>;

struct Statistics {
	double pval;
	double qval;

	Statistics(double pval, double qval);
};

bool is_pattern_sub_pattern(Itemset& pattern1, Itemset& pattern2);

bool are_patterns_equal(Itemset& pattern1, Itemset& pattern2);

#endif // !SIMPLESTRUCTURES_H
