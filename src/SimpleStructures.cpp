#include "SimpleStructures.h"

Transaction::Transaction(Origin origin, Itemset& itemset, Label label) : origin(origin), itemset(itemset), label(label) {
}

Feature::Feature(Itemset& itemset, double ig, double sup_0, double sup_1) : itemset(itemset), ig(ig), sup_0(sup_0), sup_1(sup_1) {
}

Parameters::Parameters() : min_sup(1), k(10), l(6) {
}

DS::DS() : transactions(), number_of_origins(0), number_of_origins_labeld_0(0), number_of_origins_labeld_1(0), patterns_to_remove(),
pattern_to_expand(), item_by_id(), id_by_item(){
}

Statistics::Statistics(double pval, double qval) : pval(pval), qval(qval) {
}

bool is_pattern_sub_pattern(Itemset& pattern1, Itemset& pattern2) {
	if (pattern1.size() > pattern2.size()) {
		return false;
	}
	bool found_pattern = true;
	for (auto iterator = pattern1.begin(); iterator != pattern1.end() && found_pattern; ++iterator) {
		auto iter = std::find(pattern2.begin(), pattern2.end(), *iterator);
		if (iter == pattern2.end()) {
			found_pattern = false;
		}
	}

	return found_pattern;
}

bool are_patterns_equal(Itemset& pattern1, Itemset& pattern2) {
	if (pattern1.size() != pattern2.size()) {
		return false;
	}

	bool found_pattern = is_pattern_sub_pattern(pattern1,pattern2);

	return found_pattern;
}
