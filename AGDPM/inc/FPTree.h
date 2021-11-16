#ifndef FPTREE_H
#define FPTREE_H

#include <vector>
#include <map>
#include <cstdint>
#include <algorithm>
#include "Util.h"

struct FPNode {
	const Item item;
	uint64_t frequency;
	FPNode* node_link;
	std::vector<FPNode*> children;
	FPNode* parent;
	double count0;

	FPNode(const Item& item, FPNode* parent);
};

struct FPTree {
	FPNode* root;
	std::map<Item, FPNode*> header_table;
	uint64_t min_sup;

	FPTree(DS& transactions, uint64_t& min_sup);
	~FPTree();
};

#endif // !FPTREE_H
