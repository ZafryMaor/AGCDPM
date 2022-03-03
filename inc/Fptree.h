#ifndef FPTREE_H
#define FPTREE_H

#include <vector>
#include <map>
#include <cstdint>
#include <algorithm>
#include <set>

#include "SimpleStructures.h"

class tidComparator
{
public:
	int operator() (const std::pair<Origin, Label>& f1, const std::pair<Origin, Label>& f2) const
	{
		return f1.first>f2.first;
	}
};

struct FPNode {
	const Item item;
	double frequency;

	FPNode* node_link;
	std::vector<FPNode*> children;
	FPNode* parent;
	double count0;

	std::set<std::pair<Origin,Label>,tidComparator> tid_set;

	FPNode(const Item& item, FPNode* parent);
};

struct FPTree {
	FPNode* root;
	std::map<Item, FPNode*> header_table;
	uint64_t min_sup;

	FPTree(std::vector<Transaction>& transactions, uint64_t& min_sup);
	~FPTree();
};

#endif // !FPTREE_H