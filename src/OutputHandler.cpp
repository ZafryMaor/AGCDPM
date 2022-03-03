#include "OutputHandler.h"

void write_rank(std::ofstream& outfile, int j) {
	outfile << "//" << std::endl;
	outfile << "Rank " << j << std::endl;
}

void write_pattern(std::ofstream& outfile, DS& dataset, Itemset& pattern, std::string pat_type) {

	outfile << pat_type << "(" << pattern.size() << "): ";
	auto item = pattern.begin();
	outfile << dataset.item_by_id[*item];
	item++;
	for (; item != pattern.end(); item++) {
		outfile << "," << dataset.item_by_id[*item];
	}
	outfile << std::endl;
}

void write_parameters(std::ofstream& outfile, Parameters& params) {
	outfile << "Parameters used: min_sup = " << params.min_sup << ", k = " << params.k;
	if (params.l > 0) {
		outfile << ", l = " << params.l;
	}
	outfile << std::endl;
}

void write_item_annotations(std::ofstream& outfile, DS& dataset, std::map<std::string, std::string>& item_annotaion_by_item_name, Itemset& pattern) {
	outfile << std::endl;
	outfile << "Item annotations:" << std::endl;
	for (auto item = pattern.begin(); item != pattern.end(); item++) {
		std::string item_name = dataset.item_by_id[*item];
		outfile << item_name << "\t" << item_annotaion_by_item_name[item_name] << std::endl;
	}
}

void vectorize(FinalPatternSet& FS, std::vector<Feature>& final_set_vector) {
	while (!FS.empty()) {
		Feature feature = FS.top();
		final_set_vector.push_back(feature);
		FS.pop();
	}
}

void copy_to_statistics_vector(std::vector<Feature>& final_set_vector, std::vector<std::pair<Feature, Statistics>>& final_set_statistics_vector) {
	for (auto f : final_set_vector) {
		final_set_statistics_vector.push_back(std::pair<Feature, Statistics>(f, Statistics(0, 0)));
	}
}

void sort_final_set_vector_by_IG(std::vector<Feature>& final_set_vector) {
	std::sort(final_set_vector.begin(), final_set_vector.end(), [](const Feature & f1, const Feature & f2) {
		return f1.ig > f2.ig; });
}

std::vector<Transaction> construct_conditional_transactions(std::vector<Transaction>& transactions, Itemset& pattern) {
	std::vector<Transaction> conditional_dataset;
	for (auto transaction : transactions) {
		if (is_pattern_sub_pattern(pattern, transaction.itemset)) {
			Itemset tr_is;
			for (auto i : transaction.itemset) {
				if (std::find(pattern.begin(), pattern.end(), i) == pattern.end()) {
					tr_is.push_back(i);
				}
			}
			Transaction conditional_transaction = Transaction(transaction.origin,tr_is,transaction.label);
			conditional_dataset.push_back(conditional_transaction);
		}
	}
	return conditional_dataset;
}

void complete(Itemset& pattern, std::vector<Transaction>& transactions) {
	std::vector<Transaction> conditional_dataset = construct_conditional_transactions(transactions, pattern);
	int sizeds = conditional_dataset.size();

	std::map<Item, uint64_t> frequency_by_item;
	for (auto it = conditional_dataset.begin(); it != conditional_dataset.end(); it++) {
		for (const Item& item : it->itemset) {
			++frequency_by_item[item];
		}
	}

	auto fbi_iter = frequency_by_item.begin();
	while (fbi_iter != frequency_by_item.end()) {
		if (fbi_iter->second == sizeds) {
			pattern.push_back(fbi_iter->first);
		}
		fbi_iter++;
	}

}

void recommend_redundant(std::vector<std::pair<Itemset, size_t>>& potentially_redundant, std::vector<Feature>& final_set_vector, std::vector<Itemset>& completed_patterns) {

	for (auto pattern = completed_patterns.begin(); pattern != completed_patterns.end(); ++pattern) {
		auto pat = *pattern;
		potentially_redundant.push_back(std::pair<Itemset, size_t>(pat, 0));
	}

	std::set<int> completed_to_delete;

	for (size_t i = 0; i < completed_patterns.size() - 1; ++i) {
		for (size_t j = i + 1; j < completed_patterns.size(); ++j) {
			if (is_pattern_sub_pattern(potentially_redundant[i].first, completed_patterns[j])) {
				if (potentially_redundant[i].first.size() == completed_patterns[j].size()) {
					completed_to_delete.insert(j);
				}
				++potentially_redundant[i].second;
			}
		}
	}
	int j = 0;
	for (int i : completed_to_delete) {
		potentially_redundant.erase(potentially_redundant.begin() + (i - j));
		++j;
	}
	std::sort(potentially_redundant.begin(), potentially_redundant.end(), [](std::pair<Itemset, size_t> a, std::pair<Itemset, size_t> b) -> bool {
		return a.second > b.second;
		});

}

void make_outfile(Miner& miner, std::string& output_file_name, DS& dataset, bool annotaion_flag, bool remove_patterns_flag,
	std::map<std::string, std::string>& item_annotaion_by_item_name, Parameters params) {

	std::ofstream outfile(output_file_name);

	write_parameters(outfile, params);

	std::vector<Feature> final_set_vector;
	vectorize(miner.final_set, final_set_vector);

	std::vector<std::pair<Feature, Statistics>> final_set_statistics_vector;
	copy_to_statistics_vector(final_set_vector, final_set_statistics_vector);
	
	calculate_statistical_significance(final_set_statistics_vector, dataset.number_of_origins_labeld_1, dataset.number_of_origins, (double)miner.number_of_trails);

	sort_final_set_vector_by_IG(final_set_vector);
	
	std::vector<Itemset> completed_patterns;

	int j = 1;
	for (auto feature = final_set_vector.begin(); feature != final_set_vector.end(); ++feature) {

		write_rank(outfile,j);
		
		Itemset pattern = feature->itemset;

		write_pattern(outfile, dataset, pattern, "Minimal pattern");
		
		complete(pattern, dataset.transactions);
		completed_patterns.push_back(pattern);
		write_pattern(outfile, dataset, pattern, "Complete pattern");

		Statistics statistics = get_statistics(feature->itemset, final_set_statistics_vector);

		outfile << "Information Gain: " << feature->ig << " ; p-value: " << statistics.pval << " ; q-value: " << statistics.qval << std::endl;
		outfile << "Support (by genome not transaction) in label 0: " << feature->sup_0 << "/" << dataset.number_of_origins_labeld_0 << std::endl;
		outfile << "Support (by genome not transaction) in label 1: " << feature->sup_1 << "/" << dataset.number_of_origins_labeld_1 << std::endl;
		
		if (annotaion_flag) {
			write_item_annotations(outfile, dataset, item_annotaion_by_item_name, pattern);
		}

		j++;
	}
	outfile.close();
	outfile.clear();

	std::vector<std::pair<Itemset, size_t>> potentially_redundant;
	recommend_redundant(potentially_redundant, final_set_vector, completed_patterns);

	std::ofstream redundantfile("potentially_redundent_for_" + output_file_name);
	j = 1;
	for (auto pattern : potentially_redundant) {

		write_rank(redundantfile, j);
		++j;

		write_pattern(redundantfile, dataset, pattern.first, "Pattern");

		redundantfile << "Redundancy (higher value = higher chance of being redundant): " << pattern.second << std::endl;
	}
	redundantfile.close();
	redundantfile.clear();

	std::ofstream patterns_to_remove;

	(remove_patterns_flag) ? patterns_to_remove.open("patterns_to_remove.txt", std::ios::app) : patterns_to_remove.open("patterns_to_remove_for_" + output_file_name);

	for (auto pattern : potentially_redundant) {
		if (pattern.second == 0)
			continue;
		auto item = pattern.first.begin();
		patterns_to_remove << dataset.item_by_id[*item];
		item++;
		for (; item != pattern.first.end(); item++) {
			patterns_to_remove << "," << dataset.item_by_id[*item];
		}
		patterns_to_remove << std::endl;
	}
	patterns_to_remove.close();
	patterns_to_remove.clear();
}