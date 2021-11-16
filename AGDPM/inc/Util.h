#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <queue>
#include <iostream>
#include <algorithm>
#include <map>

using Item = int;
using Itemset = std::vector<Item>;
using Transaction = std::pair<Itemset, int>;
using DS = std::vector<Transaction>;
using Feature = std::pair<std::pair<Itemset, double>, std::pair<int, int>>;
class featureComparator
{
public:
	int operator() (const Feature& f1, const Feature& f2)
	{
		return f1.first.second > f2.first.second;
	}
};
using PatternSet = std::priority_queue<Feature, std::vector<Feature>, featureComparator>;

bool is_pattern_in_pattern(Itemset& pattern1, Itemset& pattern2);

bool is_pattern_in_transaction(Itemset &pattern, Transaction &transaction);

void remove_pattern_from_transaction(Itemset &pattern, Transaction &transaction);

DS construct_conditional_transactions(DS &transactions, Itemset &pattern_to_expand);

void complete(Itemset& pattern, DS& transactions);
#endif // !UTIL_H
