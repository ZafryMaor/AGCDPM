#include "FPTree.h"

FPNode::FPNode(const Item& item, FPNode* parent) :
	item(item), frequency(1), node_link(nullptr), parent(parent), children(), count0(0)
{
}

FPTree::FPTree(DS& transactions, uint64_t& min_sup) :
	root(new FPNode(Item{}, nullptr)), header_table(),
	min_sup(min_sup)
{
	// scan the transactions counting the frequency of each item
	std::map<Item, uint64_t> frequency_by_item;
	for (auto it = transactions.begin(); it != transactions.end(); it++) {
		for (const Item& item : it->first) {
			++frequency_by_item[item];
		}
	}

	//impose min_sup on frequency_by_item and transactions
	for (auto it = transactions.begin(); it != transactions.end(); it++) {
		it->first.erase(std::remove_if(it->first.begin(), it->first.end(),
			[&frequency_by_item, min_sup](const Item & x) {
				return (frequency_by_item[x] < min_sup);
			}
		), it->first.end());
	}

	auto fbi_rm_iter = frequency_by_item.begin();
	while (fbi_rm_iter != frequency_by_item.end()) {
		if (fbi_rm_iter->second < min_sup) { fbi_rm_iter = frequency_by_item.erase(fbi_rm_iter); }
		else { ++fbi_rm_iter; }
	}

	// order each transaction by decreasing frequencey 
	for (auto it = transactions.begin(); it != transactions.end(); it++) {
		std::sort(it->first.begin(), it->first.end(), [&frequency_by_item](const Item & a, const Item & b) -> bool
			{
				uint64_t l = frequency_by_item[a];
				uint64_t r = frequency_by_item[b];
				if (l == r) {
					return a > b;
				}
				return l > r;
			});
	}

	std::map<Item, FPNode*> last_node_by_item;

	// start tree construction
	for (auto it1 = transactions.begin(); it1 != transactions.end(); ++it1) {
		auto curr_fpnode = root;

		for (const Item item : it1->first) {

			// insert item in the tree
			const auto it = std::find_if(
				curr_fpnode->children.cbegin(), curr_fpnode->children.cend(), [item](FPNode * fpnode) {
					return fpnode->item == item;
				});
			if (it == curr_fpnode->children.cend())
			{
				// the child doesn't exist, create a new node
				auto curr_fpnode_new_child = new FPNode(item, curr_fpnode);
				if (it1->second == 0) { (curr_fpnode_new_child->count0)++; }
				// add the new node to the tree
				curr_fpnode->children.push_back(curr_fpnode_new_child);

				// update the node-link structure
				if (header_table.count(curr_fpnode_new_child->item)) {
					last_node_by_item[curr_fpnode_new_child->item]->node_link = curr_fpnode_new_child;
					last_node_by_item[curr_fpnode_new_child->item] = curr_fpnode_new_child;
				}
				else {
					header_table[curr_fpnode_new_child->item] = curr_fpnode_new_child;
					last_node_by_item[curr_fpnode_new_child->item] = curr_fpnode_new_child;
				}

				// advance to the next node of the current transaction
				curr_fpnode = curr_fpnode_new_child;
			}
			else
			{
				// the child exist, increment its frequency
				auto curr_fpnode_child = *it;
				curr_fpnode_child->frequency++;
				if (it1->second == 0) { (curr_fpnode_child->count0)++; }
				// advance to the next node of the current transaction
				curr_fpnode = curr_fpnode_child;
			}
		}
	}
}

FPTree::~FPTree() {
	delete root;
	for (auto pair : header_table) {
		auto curr = pair.second;
		while (curr != nullptr) {
			auto prev = curr;
			curr = curr->node_link;
			delete prev;
		}
	}
}
