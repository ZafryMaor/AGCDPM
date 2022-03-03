#include "Miner.h"

std::mutex m;

Miner::Miner(DS* dataset, Parameters& params) : dataset(dataset), params(params), remove_patterns_flag(false), pattern_expansion_flag(false), final_set(), number_of_trails(0) {
}

void update_bound_if_better(FinalPatternSet& FS, size_t& k, double& bound) {
	if (FS.size() >= k && bound < FS.top().ig)
		bound = FS.top().ig;
}

void insert_if_better_or_not_full(FinalPatternSet& FS, size_t& k, Feature& feature) {
	if (FS.size() < k || FS.top().ig < feature.ig) {
		if (FS.size() >= k) {
			FS.pop();
		}
		FS.push(feature);
	}
}

void calculate_supports(FPTree& fptree, double& size_cond, double& number_of_labeled_0_in_cond, Item& item) {
	auto node = fptree.header_table[item];
	while (node != nullptr) {
		size_cond = size_cond + node->frequency;
		number_of_labeled_0_in_cond = number_of_labeled_0_in_cond + node->count0;
		node = node->node_link;
	}
}

bool is_root(FPNode* node) {
	return node->parent == nullptr;
}

void get_prefix(FPNode* curr_path_fpnode, Itemset& transformed_prefix_path) {
	while (curr_path_fpnode->parent != nullptr) {
		transformed_prefix_path.push_back(curr_path_fpnode->item);

		curr_path_fpnode = curr_path_fpnode->parent;
	}
}

void construct_conditional_transactions(FPTree& fptree, std::vector<Transaction>& cond_transactions, Item& item) {
	
	auto path_starting_fpnode = fptree.header_table[item];

	while (path_starting_fpnode != nullptr) {
		auto curr_path_fpnode = path_starting_fpnode->parent;
		
		if (!is_root(curr_path_fpnode)) {
			
			Itemset transformed_prefix_path; 
			get_prefix(curr_path_fpnode, transformed_prefix_path);

			for (auto iter = path_starting_fpnode->tid_set.begin(); iter != path_starting_fpnode->tid_set.end(); iter++) {

				Transaction tr = Transaction(iter->first,transformed_prefix_path,iter->second);
				cond_transactions.push_back(tr);

			}

		}
		path_starting_fpnode = path_starting_fpnode->node_link;
	}
}

double Miner::get_initial_bound(FPTree& fptree, std::vector<Feature>& singletons) {
	
	FinalPatternSet FS;

	for (auto it = fptree.header_table.begin(); it != fptree.header_table.end(); ++it) {
		Item item = it->first;
		Itemset beta;
		beta.push_back(item);

		double size_cond = 0;
		double number_of_labeled_0_in_cond = 0;
		calculate_supports(fptree, size_cond, number_of_labeled_0_in_cond, item);
		
		double IG_beta = InfoGain(size_cond / dataset->number_of_origins, dataset->number_of_origins_labeld_1 / dataset->number_of_origins, (size_cond - number_of_labeled_0_in_cond) / size_cond);

		Feature singleton = Feature(beta, IG_beta, number_of_labeled_0_in_cond, (size_cond - number_of_labeled_0_in_cond));
		singletons.push_back(singleton);

		number_of_trails++;

		insert_if_better_or_not_full(FS, params.k, singleton);
		
	}
	return FS.top().ig;
}

void update_max_if_better(Itemset& beta, double& IG_beta, double& size_cond, double& number_of_labeled_0_in_cond, double& max_IG, Feature& best) {
	if (IG_beta > max_IG) {
		max_IG = IG_beta;
		best = Feature(beta, IG_beta, number_of_labeled_0_in_cond, size_cond - number_of_labeled_0_in_cond);
	}
}

void Miner::continue_mining(FPTree& fptree, double& max_IG, Feature& best, double bound, Itemset& alpha) {
	std::vector<Transaction> cond_transactions;
	construct_conditional_transactions(fptree, cond_transactions, alpha[alpha.size() - 1]);

	if (cond_transactions.size() > 0) {
		FPTree cond_fptree = FPTree{ cond_transactions,fptree.min_sup };
		branch_and_bound(cond_fptree, max_IG, best, bound, alpha);
	}
}

void Miner::branch_and_bound(FPTree& fptree, double& max_IG, Feature& best, double bound, Itemset& alpha) {
	if (params.l > 0 && alpha.size() > params.l - 1) {
		return;
	}

	for (auto it = fptree.header_table.begin(); it != fptree.header_table.end(); ++it) {
		Item item = it->first;
		Itemset beta = alpha;
		beta.push_back(item);

		double size_cond = 0;
		double number_of_labeled_0_in_cond = 0;
		calculate_supports(fptree, size_cond, number_of_labeled_0_in_cond, item);

		double IG_beta = InfoGain(size_cond / dataset->number_of_origins, dataset->number_of_origins_labeld_1 / dataset->number_of_origins, (size_cond - number_of_labeled_0_in_cond) / size_cond);

		update_max_if_better(beta, IG_beta, size_cond, number_of_labeled_0_in_cond, max_IG, best);

		double IGub = InfoGainUB(dataset->number_of_origins_labeld_1 / dataset->number_of_origins, size_cond / dataset->number_of_origins);

		number_of_trails++;

		if (IGub > bound && IGub > max_IG) {
			continue_mining(fptree, max_IG, best, bound, beta);
		}
	}
}

void Miner::singleton_iteration(FPTree& fptree, std::vector<Feature>& singletons, double& bound) {

	std::for_each(std::execution::par, std::begin(singletons), std::end(singletons), [&](Feature f) {
		Feature best = f;
		double max_IG = f.ig;
		Itemset alpha = f.itemset;

		double IGub = InfoGainUB((dataset->number_of_origins_labeld_1) / dataset->number_of_origins, (f.sup_0 + f.sup_1) / dataset->number_of_origins);

		if (bound < IGub) {
			continue_mining(fptree, max_IG, best, bound, alpha);
		}

		m.lock();

		insert_if_better_or_not_full(final_set, params.k, best);
		update_bound_if_better(final_set, params.k, bound);
		
		m.unlock();
		});
}

void remove_pattern_items_from_transaction(Itemset& pattern, Transaction& transaction) {
	for (auto iterator = pattern.begin(); iterator != pattern.end(); ++iterator) {
		auto iter = std::find(transaction.itemset.begin(), transaction.itemset.end(), *iterator);
		if (iter != transaction.itemset.end()) {
			transaction.itemset.erase(iter);
		}
	}
}

void remove_patterns(std::vector<Transaction>& transactions, std::vector<Itemset>& patterns_to_remove) {
	for (auto tr = transactions.begin(); tr != transactions.end(); tr++) {
		for(Itemset ptr : patterns_to_remove)
			remove_pattern_items_from_transaction(ptr, *tr);
	}
}

void leave_only_transactions_containing_pattern_to_expand(std::vector<Transaction>& transactions, Itemset pattern_to_expand) {
	auto iter = transactions.begin();
	while (iter != transactions.end()) {
		if (!is_pattern_sub_pattern(pattern_to_expand, iter->itemset))
			iter = transactions.erase(iter);
		else
			++iter;
	}
}

void Miner::mine() {
	uint64_t min_sup = uint64_t((dataset->number_of_origins * params.min_sup) / 100);

	std::vector<Transaction> transactions = dataset->transactions;

	if (remove_patterns_flag)
		remove_patterns(transactions, dataset->patterns_to_remove);

	if (pattern_expansion_flag) {
		leave_only_transactions_containing_pattern_to_expand(transactions, dataset->pattern_to_expand);
	}

	FPTree fptree = FPTree{ transactions,min_sup };

	double inital_IG_bound = 0;
	std::vector<Feature> singletons;
	inital_IG_bound = Miner::get_initial_bound(fptree, singletons);
	
	Itemset alpha;
	singleton_iteration(fptree, singletons, inital_IG_bound);

}