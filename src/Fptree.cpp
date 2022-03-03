#include "Fptree.h"

FPNode::FPNode(const Item& item, FPNode* parent) :
	item(item), frequency(1), node_link(nullptr), parent(parent), children(), count0(0)
{
}

void create_new_item_origins_set_if_needed(const Item& item, std::map<Item, std::set<Origin>>& origins_by_item) {
	if (origins_by_item.count(item) == 0) {
		origins_by_item[item] = std::set<Origin>();
	}
}

void inc_frequency(std::map<Item, uint64_t>& frequency_by_item, const Item& item) {
	if (frequency_by_item.count(item) == 0)
		frequency_by_item[item] = 1;
	else
		++frequency_by_item[item];
}

void count_frequencies_itemset(Itemset& itemset, Origin& origin, std::map<Item, std::set<Origin>>& origins_by_item,
	std::map<Item, uint64_t>& origin_frequency_by_item) {

	for (const Item& item : itemset) {
		create_new_item_origins_set_if_needed(item, origins_by_item);

		std::set<Origin>* item_origins_set = &origins_by_item[item];
		if (item_origins_set->find(origin) == item_origins_set->end()) {
			item_origins_set->insert(origin);
			inc_frequency(origin_frequency_by_item, item);
		}
	}
}

void count_frequencies_transactions(std::vector<Transaction>& transactions, uint64_t& number_of_origins, std::map<Item, std::set<Origin>>& origins_by_item,
	std::map<Item, uint64_t>& origin_frequency_by_item) {
	
	std::set<Origin> origins;

	for (auto it = transactions.begin(); it != transactions.end(); it++) {
		Origin origin = it->origin;
		origins.insert(origin);

		count_frequencies_itemset(it->itemset, origin, origins_by_item, origin_frequency_by_item);
	}

	number_of_origins = origins.size();
}

void impose_min_sup(std::vector<Transaction>& transactions, uint64_t& min_sup, uint64_t& number_of_origins, std::map<Item, uint64_t>& origin_frequency_by_item) {
	for (auto it = transactions.begin(); it != transactions.end(); it++) {
		it->itemset.erase(
			std::remove_if(it->itemset.begin(), it->itemset.end(),
			[&origin_frequency_by_item, min_sup, number_of_origins](const Item & x) {
				uint64_t freq_x = origin_frequency_by_item[x];
				return (freq_x < min_sup || freq_x == number_of_origins);
			}
		), it->itemset.end());
	}
}

void order_by_decreasing_frequency(std::vector<Transaction>& transactions, std::map<Item, uint64_t>& frequency_by_item) {
	for (auto it = transactions.begin(); it != transactions.end(); it++) {
		std::sort(it->itemset.begin(), it->itemset.end(),
			[&frequency_by_item](const Item & a, const Item & b) -> bool
			{
				uint64_t l = frequency_by_item[a];
				uint64_t r = frequency_by_item[b];
				if (l == r) {
					return a > b;
				}
				return l > r;
			});
	}
}

const std::vector<FPNode*>::const_iterator find_child_if_child_with_item_exists(FPNode* curr_fpnode, const Item& item) {
	return std::find_if(
		curr_fpnode->children.cbegin(), curr_fpnode->children.cend(), [item](FPNode * fpnode) {
			return fpnode->item == item;
		});
}

void update_horizontal_pointers(std::map<Item, FPNode*>& header_table, FPNode*& curr_fpnode_new_child, std::map<Item, FPNode*>& last_node_by_item) {
	if (header_table.count(curr_fpnode_new_child->item)) {
		last_node_by_item[curr_fpnode_new_child->item]->node_link = curr_fpnode_new_child;
		last_node_by_item[curr_fpnode_new_child->item] = curr_fpnode_new_child;
	}
	else {
		header_table[curr_fpnode_new_child->item] = curr_fpnode_new_child;
		last_node_by_item[curr_fpnode_new_child->item] = curr_fpnode_new_child;
	}
}

bool is_first_count_for_item_from_origin(std::set<Origin>* item_origins_set, Origin& origin) {
	return item_origins_set->find(origin) != item_origins_set->end();
}

void choose_new_fpnode_frequency(Origin& origin, Label& label, const Item& item, FPNode*& curr_fpnode_new_child, std::map<Item, std::set<Origin>>& origins_by_item) {
	
	if (label == 0) {
		(curr_fpnode_new_child->count0)++;
	}

	std::set<Origin>* item_origins_set = &origins_by_item[item];
	if (is_first_count_for_item_from_origin(item_origins_set,origin)) {
		origins_by_item[item].erase(origin);
	}
	else {
		curr_fpnode_new_child->frequency = 0;
		curr_fpnode_new_child->count0 = 0;
	}
}

void create_new_fpnode_in_path(Origin& origin, Label& label, FPNode*& curr_fpnode, std::map<Item, FPNode*>& header_table, const Item& item,
	std::map<Item, FPNode*>& last_node_by_item, std::map<Item, std::set<Origin>>& origins_by_item) {

	auto curr_fpnode_new_child = new FPNode(item, curr_fpnode);

	choose_new_fpnode_frequency(origin, label, item, curr_fpnode_new_child, origins_by_item);

	curr_fpnode_new_child->tid_set.insert(std::pair<Origin,Label>(origin,label));

	curr_fpnode->children.push_back(curr_fpnode_new_child);

	update_horizontal_pointers(header_table, curr_fpnode_new_child, last_node_by_item);

	curr_fpnode = curr_fpnode_new_child;
}

void handle_existing_fpnode_frequency(FPNode*& curr_fpnode_child, Origin& origin, Label& label, const Item& item, std::map<Item, std::set<Origin>>& origins_by_item) {

	std::set<Origin>* item_origins_set = &origins_by_item[item];
	if (item_origins_set->find(origin) != item_origins_set->end()) {
		origins_by_item[item].erase(origin);
		curr_fpnode_child->frequency++;
		if (label == 0) { (curr_fpnode_child->count0)++; }
	}
}

bool is_first_visit_of_fpnode_from_origin(std::pair<Origin, Label>& tid, FPNode*& curr_fpnode_child){
	return curr_fpnode_child->tid_set.find(tid) == curr_fpnode_child->tid_set.end();
}

void construct_new_path_from_root(Origin& origin, Label& label, FPNode*& curr_fpnode, std::map<Item, FPNode*>& header_table, Itemset& itemset,
	std::map<Item, FPNode*>& last_node_by_item, std::map<Item, std::set<Origin>>& origins_by_item) {
	
	for (const Item item : itemset) {

		const std::vector<FPNode*>::const_iterator child_iterator = find_child_if_child_with_item_exists(curr_fpnode, item);

		if (child_iterator == curr_fpnode->children.cend())
		{
			create_new_fpnode_in_path(origin, label, curr_fpnode, header_table, item, last_node_by_item, origins_by_item);
			
		}
		else
		{
			auto curr_fpnode_child = *child_iterator;

			std::pair<Origin, Label> tid = std::pair<Origin, Label>(origin, label);
			if (is_first_visit_of_fpnode_from_origin(tid,curr_fpnode_child)) {

				curr_fpnode_child->tid_set.insert(tid);
				handle_existing_fpnode_frequency(curr_fpnode_child, origin, label, item, origins_by_item);

			}
			
			curr_fpnode = curr_fpnode_child;
		}
	}
}

void construct_tree(FPNode* root, std::map<Item, FPNode*>& header_table, std::vector<Transaction>& transactions, std::map<Item, std::set<Origin>>& origins_by_item) {
	std::map<Item, FPNode*> last_node_by_item;

	for (auto it = transactions.begin(); it != transactions.end(); ++it) {
		FPNode * curr_fpnode = root;
		Origin origin = it->origin;
		Label label = it->label;
		construct_new_path_from_root(origin, label, curr_fpnode, header_table, it->itemset, last_node_by_item, origins_by_item);
	}
}

FPTree::FPTree(std::vector<Transaction>& transactions, uint64_t& min_sup) :
	root(new FPNode(Item{}, nullptr)), header_table(),
	min_sup(min_sup)
{
	uint64_t number_of_origins = 0;
	std::map<Item, std::set<Origin>> origins_by_item;
	std::map<Item, uint64_t> origin_frequency_by_item;

	count_frequencies_transactions(transactions, number_of_origins, origins_by_item, origin_frequency_by_item);

	impose_min_sup(transactions, min_sup, number_of_origins, origin_frequency_by_item);

	order_by_decreasing_frequency(transactions, origin_frequency_by_item);

	construct_tree(root, header_table, transactions, origins_by_item);
	
	
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
