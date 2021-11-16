#include "Util.h"

bool is_pattern_in_pattern(Itemset& pattern1, Itemset& pattern2) {
	bool found_pattern = true;
	for (auto iterator = pattern1.begin(); iterator != pattern1.end() && found_pattern; ++iterator) {
		auto iter = std::find(pattern2.begin(), pattern2.end(), *iterator);
		if (iter == pattern2.end()) {
			found_pattern = false;
		}
	}

	return found_pattern;
}

bool is_pattern_in_transaction(Itemset &pattern, Transaction &transaction) {
	bool found_pattern = true;
	for (auto iterator = pattern.begin(); iterator != pattern.end() && found_pattern; ++iterator) {
		auto iter = std::find(transaction.first.begin(), transaction.first.end(), *iterator);
		if (iter == transaction.first.end()) {
			found_pattern = false;
		}
	}

	return found_pattern;
}

void remove_pattern_from_transaction(Itemset& pattern, Transaction& transaction) {
	for (auto iterator = pattern.begin(); iterator != pattern.end(); ++iterator) {
		auto iter = std::find(transaction.first.begin(), transaction.first.end(), *iterator);
		if (iter != transaction.first.end()) {
			transaction.first.erase(iter);
		}
	}
}

DS construct_conditional_transactions(DS& transactions, Itemset& pattern_to_expand) {
	DS conditional_dataset;
	for (auto transaction : transactions) {
		if (is_pattern_in_transaction(pattern_to_expand, transaction)) {
			Transaction conditional_transaction;
			conditional_transaction.second = transaction.second;
			for (auto i : transaction.first) {
				if (std::find(pattern_to_expand.begin(), pattern_to_expand.end(), i) == pattern_to_expand.end()) {
					conditional_transaction.first.push_back(i);
				}
			}
			conditional_dataset.push_back(conditional_transaction);
		}
	}
	return conditional_dataset;
}

void complete(Itemset& pattern, DS& transactions) {
	DS conditional_dataset = construct_conditional_transactions(transactions, pattern);
	int sizeds = conditional_dataset.size();

	std::map<Item, uint64_t> frequency_by_item;
	for (auto it = conditional_dataset.begin(); it != conditional_dataset.end(); it++) {
		for (const Item& item : it->first) {
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