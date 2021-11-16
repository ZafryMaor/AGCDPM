#include "Miner.h"

std::mutex m;

void construct_conditional_ds(FPTree& fptree, DS& transactions, Item& item) {
	auto path_starting_fpnode = fptree.header_table[item];
	while (path_starting_fpnode != nullptr) {

		auto curr_path_fpnode = path_starting_fpnode->parent;
		// check if curr_path_fpnode is already the root of the fptree
		if (curr_path_fpnode->parent != nullptr) {
			// the path has at least one node (excluding the starting node and the root)
			Itemset transformed_prefix_path;
			while (curr_path_fpnode->parent != nullptr) {
				transformed_prefix_path.push_back(curr_path_fpnode->item);

				// advance to the next node in the path
				curr_path_fpnode = curr_path_fpnode->parent;
			}
			Transaction tr0;
			tr0.first = "";
			tr0.second.first = transformed_prefix_path;
			tr0.second.second = 0;
			Transaction tr1;
			tr1.first = "";
			tr1.second.first = transformed_prefix_path;
			tr1.second.second = 1;
			for (int i = 0; i < path_starting_fpnode->count0; i++) {
				transactions.push_back(tr0);
			}
			for (int i = 0; i < path_starting_fpnode->frequency - path_starting_fpnode->count0; i++) {
				transactions.push_back(tr1);
			}

		}
		path_starting_fpnode = path_starting_fpnode->node_link;
	}
}

void branch_and_bound(FPTree& fptree, double& max_IG, Feature& best, double bound, Itemset& alpha, double& number_of_transactions, double& transactions_labeld_1_counter, int k, int l) {
	if (fptree.header_table.size() == 0) {
		return;
	}
	if (l>0 && alpha.size() > l-1) {
		return;
	}
	for (auto it = fptree.header_table.begin(); it != fptree.header_table.end(); ++it) {
		Item item = it->first;
		Itemset beta = alpha;
		beta.push_back(item);

		//Calculate labeled supports of pattern beta
		double size_cond = 0;
		double count0group1 = 0;
		auto node = fptree.header_table[item];
		while (node != nullptr) {
			size_cond = size_cond + node->frequency;
			count0group1 = count0group1 + node->count0;
			node = node->node_link;
		}

		//Calculate IG of beta
		double IG_beta = InfoGain(size_cond / number_of_transactions, (transactions_labeld_1_counter) / number_of_transactions, (size_cond - count0group1) / size_cond);

		if (IG_beta > max_IG) {
			max_IG = IG_beta;
			best = Feature(std::pair<Itemset, double>(beta, IG_beta), std::pair<int, int>((int)count0group1, (int)(size_cond - count0group1)));
		}

		double IGub = InfoGainUB((transactions_labeld_1_counter) / number_of_transactions, size_cond / number_of_transactions);

		if (IGub > bound && IGub > max_IG) {
			DS cond_transactions;
			construct_conditional_ds(fptree, cond_transactions, item);

			if (cond_transactions.size() > 0) {
				FPTree cond_fptree = FPTree{ cond_transactions,fptree.min_sup };
				branch_and_bound(cond_fptree, max_IG, best, bound, beta, number_of_transactions, transactions_labeld_1_counter, k, l);
			}
		}
	}
}

double get_initial_bound(FPTree & fptree, double number_of_origins, double origins_labeld_1_counter, int k, std::vector<Feature> & singletons) {
	PatternSet FS;
	if (fptree.header_table.size() == 0) {
		return 0;
	}
	for (auto it = fptree.header_table.begin(); it != fptree.header_table.end(); ++it) {
		Item item = it->first;
		Itemset beta;
		beta.push_back(item);

		//Calculate labeled supports of pattern beta
		double size_cond = 0;
		double count0group1 = 0;
		auto node = fptree.header_table[item];
		while (node != nullptr) {
			size_cond = size_cond + node->frequency;
			count0group1 = count0group1 + node->count0;
			node = node->node_link;
		}

		//Calculate IG of beta
		double IG_beta = InfoGain(size_cond / number_of_origins, origins_labeld_1_counter / number_of_origins, (size_cond - count0group1) / size_cond);

		Feature singleton = Feature(std::pair<Itemset, double>(beta, IG_beta), std::pair<int, int>((int)count0group1, (int)(size_cond - count0group1)));
		singletons.push_back(singleton);

		if (FS.size() < k || FS.top().first.second < IG_beta) {
			if (FS.size() >= k) {
				FS.pop();
			}
			FS.push(singleton);
		}
	}
	return FS.top().first.second;
}

void singleton_iteration(FPTree & fptree, PatternSet & FS, double& number_of_origins, double& origins_labeld_1_counter, int k,
	std::vector<Feature> & singletons, double bound, int l, bool verbose_flag) {

	std::for_each(std::execution::par, std::begin(singletons), std::end(singletons), [&](Feature f) {
		Feature best = f;
		double max_IG = f.first.second;
		Itemset alpha = f.first.first;

		double IGub = InfoGainUB((origins_labeld_1_counter) / number_of_origins, ((double)(f.second.first) + (double)f.second.second) / number_of_origins);
		
		m.lock();
		if (FS.size() >= k && FS.top().first.second > bound) {
			bound = FS.top().first.second;
		}
		m.unlock();
		
		if (bound < IGub) {
			DS cond_transactions;
			construct_conditional_ds(fptree, cond_transactions, alpha[0]);

			if (cond_transactions.size() > 0) {
				FPTree cond_fptree = FPTree{ cond_transactions,fptree.min_sup };
				branch_and_bound(cond_fptree, max_IG, best, bound, alpha, number_of_origins, origins_labeld_1_counter, k, l);
			}
		}

		//Insert best pat of item to queue if not full or if full and IG is better than current lowest IG in queue
		m.lock();
		if (FS.size() < k || best.first.second > FS.top().first.second) {
			if (FS.size() >= k) {
				FS.pop();
			}
			FS.push(best);
		}
		if (FS.size() >= k && bound < FS.top().first.second)
			bound = FS.top().first.second;
		m.unlock();
	});		
}

PatternSet mine(DS transactions, double min_sup_percent, int k, int l,
	double origins_labeld_1_counter, double number_of_origins, bool verbose_flag) {
	uint64_t min_sup = (number_of_origins * min_sup_percent) / 100;
	PatternSet FS;
	Itemset best_pat;

	FPTree fptree = FPTree{ transactions,min_sup };
	double inital_IG_bound = 0;
	std::vector<Feature> singletons;
	inital_IG_bound = get_initial_bound(fptree, number_of_origins, origins_labeld_1_counter, k, singletons);
	Itemset alpha;
	singleton_iteration(fptree, FS, number_of_origins, origins_labeld_1_counter, k, singletons, inital_IG_bound, l, verbose_flag);

	return FS;
}
