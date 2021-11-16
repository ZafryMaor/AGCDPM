#include "IOFileHandler.h"

void print_arguments_error() {
	std::cout << "Expected usage: agdpm transactions_file_path" << std::endl;
	std::cout << "Optional flags (can be in any order but keep the file path right after the -a): -e, -r, -v, -a annotations_file_path." << std::endl;
	std::cout << "For more information please visit our github page." << std::endl;
}
void print_parameters_error() {
	std::cout << "Could not read \"parameters.txt\" file." << std::endl;
	std::cout << "For more information please visit our github page." << std::endl;
}
void print_annotaions_file_error() {
	std::cout << "Could not read annotations file provided." << std::endl;
	std::cout << "For more information please visit our github page." << std::endl;
}
void print_transactions_file_error() {
	std::cout << "Could not read transactions dataset file provided." << std::endl;
	std::cout << "For more information please visit our github page." << std::endl;
}
void print_patterns_to_remove_file_error() {
	std::cout << "Could not read \"patterns to remove.txt\" file." << std::endl;
	std::cout << "For more information please visit our github page." << std::endl;
}
void print_pattern_to_expand_file_error() {
	std::cout << "Could not read \"pattern to expand.txt\" file." << std::endl;
	std::cout << "For more information please visit our github page." << std::endl;
}
void print_non_existing_item_in_expand_pattern_error() {
	std::cout << "Found an item that does not exist in transactions dataset in the pattern to expand, \nresulting in an empty conditional dataset." << std::endl;
}

void read_args(int& argc, char** argv, std::string& transactions_file_path, std::string& annotaions_file_path,
	bool& remove_patterns_flag, bool& pattern_expansion_flag, bool& annotaion_flag, bool& verbose_flag, InputErrorType& err) {
	if (argc < 2) {
		err = arg;
		return;
	}
	transactions_file_path = argv[1];
	for (int i = 2; i < argc; i++) {
		switch (argv[i][1])
		{
		case 'a':
			annotaion_flag = true;
			++i;
			annotaions_file_path = argv[i];
			break;
		case 'e':
			pattern_expansion_flag = true;
			break;
		case 'r':
			remove_patterns_flag = true;
			break;
		case 'v':
			verbose_flag = false;
			break;
		default:
			err = arg;
			break;
		}
	}
}

void read_paramerters_file(double& min_sup, int& k, int& l, InputErrorType& err) {
	std::string str;
	std::ifstream parameters_file;
	parameters_file.open("parameters.txt");
	if (parameters_file.is_open()) {
		std::getline(parameters_file, str);
		int start_index = str.find_first_of('\"') + 1;
		min_sup = std::stod(str.substr(start_index, str.find_last_of('\"') - start_index));

		std::getline(parameters_file, str);
		start_index = str.find_first_of('\"') + 1;
		k = std::stoi(str.substr(start_index, str.find_last_of('\"') - start_index));

		std::getline(parameters_file, str);
		start_index = str.find_first_of('\"') + 1;
		l = std::stoi(str.substr(start_index, str.find_last_of('\"') - start_index));
	}
	else {
		err = param;
	}
	parameters_file.close();
	parameters_file.clear();
}

void read_annotaions_file(std::string annotaions_file_path, std::map<std::string, std::string> &item_annotaion_by_item_name,
	InputErrorType& err) {
	std::string str;
	std::ifstream annotations_file;
	annotations_file.open(annotaions_file_path);
	if (annotations_file.is_open()) {
		while (std::getline(annotations_file, str)) {
			int start_index = str.find_first_of('\t') + 1;
			item_annotaion_by_item_name[str.substr(0, start_index - 1)] = str.substr(start_index);
		}
	}
	else {
		err = annofile;
	}
	annotations_file.close();
	annotations_file.clear();
}

DS read_transactions_file(std::string& transactions_file_path, std::map<int, std::string>& item_by_id,
	std::map<std::string, int>& id_by_item, double& transactions_labeld_1_counter,
	double& number_of_transactions, InputErrorType& err) {
	std::string str;
	DS parsed_ds;
	char dim = ',';
	int curr_item_id = 0;
	int transaction_id = 0;
	std::ifstream transactions_file(transactions_file_path);
	if (transactions_file.is_open()) {
		while (std::getline(transactions_file, str)) {
			number_of_transactions++;

			int lable = std::stoi(str.substr(str.length() - 1));
			if (lable == 1) transactions_labeld_1_counter++;

			size_t curr_index = 0;
			std::vector<Item> itemset;
			while (curr_index < str.find_last_of(',')) {
				size_t next_index = str.find(',', curr_index);
				std::string item = str.substr(curr_index, next_index - curr_index);
				if (id_by_item.count(item) == 0) {
					id_by_item[item] = curr_item_id;
					item_by_id[curr_item_id] = item;
					curr_item_id++;
				}
				itemset.push_back(id_by_item[item]);
				curr_index = next_index + 1;
			}
			if (itemset.size() > 0) {
				parsed_ds.push_back(Transaction(itemset, lable));
				transaction_id++;
			}
		}
	}
	else {
		err = dataset;
	}
	transactions_file.close();
	transactions_file.clear();
	return parsed_ds;
}

void remove_patterns(DS& transactions, std::map<std::string, int>& id_by_item, InputErrorType& err) {
	
	std::string str;
	char dim = ',';
	std::ifstream patterns_to_remove_file("patterns_to_remove.txt");
	if (patterns_to_remove_file.is_open()) {
		while (std::getline(patterns_to_remove_file, str)) {
			size_t curr_index = 0;
			std::vector<Item> pattern_to_remove;
			bool non_existing_item = false;
			while (curr_index < str.length()-1 && !non_existing_item) {
				size_t next_index = str.find(',', curr_index);
				std::string item = str.substr(curr_index, next_index - curr_index);
				if (id_by_item.count(item) != 0) {
					pattern_to_remove.push_back(id_by_item[item]);
				}
				else {
					non_existing_item = true;
				}
				curr_index = next_index + 1;
			}

			if (pattern_to_remove.size() > 0 && !non_existing_item) {
				for (auto tr : transactions) {
					if (is_pattern_in_transaction(pattern_to_remove, tr)) {
						remove_pattern_from_transaction(pattern_to_remove, tr);
					}
				}
			}
		}
	}
	else {
		err = ptrfile;
	}
	patterns_to_remove_file.close();
	patterns_to_remove_file.clear();
}

Itemset parse_pattern_to_expand(std::map<std::string, int>& id_by_item, InputErrorType& err) {
	std::vector<Item> pattern_to_expand;
	bool non_existing_item = false;
	std::string str;
	char dim = ',';
	std::ifstream pattern_to_expand_file("pattern_to_expand.txt");
	if (pattern_to_expand_file.is_open()) {
		std::getline(pattern_to_expand_file, str);
		
		size_t curr_index = 0;
		while (curr_index < str.length() - 1 && !non_existing_item) {
			size_t next_index = str.find(',', curr_index);
			std::string item = str.substr(curr_index, next_index - curr_index);
			if (id_by_item.count(item) != 0) {
				pattern_to_expand.push_back(id_by_item[item]);
			}
			else {
				non_existing_item = true;
				err = ptefile;
				print_non_existing_item_in_expand_pattern_error();
			}

			curr_index = next_index + 1;
		}

		if (pattern_to_expand.size() == 0) {
			err = ptefile;
		}
		
	}
	else {
		err = ptrfile;
	}
	pattern_to_expand_file.close();
	pattern_to_expand_file.clear();

	return pattern_to_expand;
}

std::vector<std::pair<Itemset, size_t>> recommend_redundant(std::vector<Feature>& final_set_vector) {
	std::vector<std::pair<Itemset, size_t>> patterns_with_redundancy;

	for (auto feature : final_set_vector) {
		auto pat = feature.first.first;
		patterns_with_redundancy.push_back(std::pair<Itemset, size_t>(pat, 0));
	}
	for (int i = 0; i < patterns_with_redundancy.size(); ++i) {
		for (int j = 0; j < patterns_with_redundancy.size(); +j) {
			if (j == i)
				continue;
			if (is_pattern_in_pattern(patterns_with_redundancy[i].first, patterns_with_redundancy[j].first)) {
				patterns_with_redundancy[i].second++;
			}
		}
	}
	std::sort(patterns_with_redundancy.begin(), patterns_with_redundancy.end(), [](std::pair<Itemset, size_t> a, std::pair<Itemset, size_t> b) -> bool {
		return a.second > b.second;
		});
	return patterns_with_redundancy;

}

void make_outfile(PatternSet& final_set, std::string& output_file_name,
	DS& transactions, std::map<int, std::string>& item_by_id, bool annotaion_flag, bool remove_patterns_flag,
	std::map<std::string, std::string>& item_annotaion_by_item_name, double min_sup, int k, int l,
	double transactions_labeld_1_counter, double transactions_labeld_0_counter) {
	
	std::ofstream outfile(output_file_name);

	outfile << "Parameters used: min_sup = " << min_sup << ", k = " << k;
	if (l > 0) {
		outfile << ", l = " << l;
	}
	outfile << std::endl;

	std::vector<Feature> final_set_vector;
	while (!final_set.empty()) {
		Feature feature = final_set.top();
		final_set_vector.push_back(feature);
		final_set.pop();
	}

	int j = 1;
	for (auto feature = final_set_vector.rbegin(); feature != final_set_vector.rend(); ++feature) {
		
		outfile << "//" << std::endl;
		outfile << "Rank " << j << std::endl;
		
		Itemset pattern = feature->first.first;
		outfile << "Minimal pattern(" << pattern.size() << "): ";
		auto item = pattern.begin();
		outfile << item_by_id[*item];
		item++;
		for (; item != pattern.end(); item++) {
			outfile << "," << item_by_id[*item];
		}
		outfile << std::endl;

		complete(pattern, transactions);
		
		outfile << "Complete pattern: ";
		item = pattern.begin();
		outfile << item_by_id[*item];
		item++;
		for (; item != pattern.end(); item++) {
			outfile << "," << item_by_id[*item];
		}
		outfile << std::endl;

		outfile << "Information Gain: " << feature->first.second << std::endl;
		outfile << "Support in label 0: " << feature->second.first << "/" << transactions_labeld_0_counter << std::endl;
		outfile << "Support in label 1: " << feature->second.second << "/" << transactions_labeld_1_counter << std::endl;
		j++;

		if (annotaion_flag) {
			outfile << std::endl;
			outfile << "Item annotations:" << std::endl;
			for (item = pattern.begin(); item != pattern.end(); item++) {
				std::string item_name = item_by_id[*item];
				outfile << item_name << item_annotaion_by_item_name[item_name] << std::endl;
			}
		}
	}
	outfile.close();
	outfile.clear();

	std::vector<std::pair<Itemset, size_t>> potentially_redundant = recommend_redundant(final_set_vector);
	std::ofstream redundantfile("potentially_redundent_for_"+output_file_name);
	j = 1;
	for (auto pattern : potentially_redundant) {

		redundantfile << "//" << std::endl;
		redundantfile << "Rank " << j << std::endl;
		++j;

		redundantfile << "Pattern(" << pattern.first.size() << "): ";
		auto item = pattern.first.begin();
		redundantfile << item_by_id[*item];
		item++;
		for (; item != pattern.first.end(); item++) {
			redundantfile << "," << item_by_id[*item];
		}
		redundantfile << std::endl;
		redundantfile << "Redundancy (higher value = higher chance of redundancy): " << pattern.second << std::endl;
	}
	redundantfile.close();
	redundantfile.clear();

	std::ofstream patterns_to_remove;

	(remove_patterns_flag) ? patterns_to_remove.open("patterns_to_remove.txt", std::ios::app) : patterns_to_remove.open("patterns_to_remove_for_" + output_file_name);

	for (auto pattern : potentially_redundant) {
		if (pattern.second == 0)
			continue;
		auto item = pattern.first.begin();
		patterns_to_remove << item_by_id[*item];
		item++;
		for (; item != pattern.first.end(); item++) {
			patterns_to_remove << "," << item_by_id[*item];
		}
		patterns_to_remove << std::endl;
	}
	patterns_to_remove.close();
	patterns_to_remove.clear();
}
