#include "InputHandler.h"

void read_args(int& argc, char** argv, std::string& transactions_file_path, std::string& annotaions_file_path,
	bool& remove_patterns_flag, bool& pattern_expansion_flag, bool& annotaion_flag, InputErrorType& err) {
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
		default:
			err = arg;
			break;
		}
	}
}

void read_paramerters_file(Parameters& params, InputErrorType& err) {
	std::string str;
	std::ifstream parameters_file;
	parameters_file.open("parameters.txt");
	if (parameters_file.is_open()) {
		std::getline(parameters_file, str);
		int start_index = str.find_first_of('\"') + 1;
		params.min_sup = std::stod(str.substr(start_index, str.find_last_of('\"') - start_index));

		std::getline(parameters_file, str);
		start_index = str.find_first_of('\"') + 1;
		params.k = (size_t)std::stoi(str.substr(start_index, str.find_last_of('\"') - start_index));

		std::getline(parameters_file, str);
		start_index = str.find_first_of('\"') + 1;
		params.l = (size_t)std::stoi(str.substr(start_index, str.find_last_of('\"') - start_index));
	}
	else {
		err = param;
	}
	parameters_file.close();
	parameters_file.clear();
}

void read_annotaions_file(std::string annotaions_file_path, std::map<std::string, std::string>& item_annotaion_by_item_name,
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

void read_transactions_file(DS& dataset, std::string& transactions_file_path, InputErrorType& err) {
	std::string str;
	int curr_item_id = 0;
	std::set<std::string> origins;

	std::ifstream transactions_file(transactions_file_path);
	if (transactions_file.is_open()) {
		while (std::getline(transactions_file, str)) {

			Label lable = std::stoi(str.substr(str.find_last_of(',') + 1, 1));

			std::string transaction_origin_name = str.substr(0, str.find_first_of(','));

			size_t curr_index = str.find_first_of(',') + 1;
			std::vector<Item> itemset;
			while (curr_index < str.find_last_of(',')) {
				size_t next_index = str.find(',', curr_index);
				std::string item = str.substr(curr_index, next_index - curr_index);
				if (dataset.id_by_item.count(item) == 0) {
					dataset.id_by_item[item] = curr_item_id;
					dataset.item_by_id[curr_item_id] = item;
					curr_item_id++;
				}
				itemset.push_back(dataset.id_by_item[item]);
				curr_index = next_index + 1;
			}
			dataset.transactions.push_back(Transaction(transaction_origin_name, itemset, lable));
			if (origins.find(transaction_origin_name) == origins.end()) {
				origins.insert(transaction_origin_name);
				dataset.number_of_origins++;
				if (lable == 1) dataset.number_of_origins_labeld_1++;
			}
		}

		dataset.number_of_origins_labeld_0 = dataset.number_of_origins - dataset.number_of_origins_labeld_1;
	}
	else {
		err = ds;
	}
	transactions_file.close();
	transactions_file.clear();
}

void read_patterns_to_remove(DS& dataset, InputErrorType& err) {

	std::string str;
	std::ifstream patterns_to_remove_file("patterns_to_remove.txt");
	if (patterns_to_remove_file.is_open()) {
		while (std::getline(patterns_to_remove_file, str)) {
			size_t curr_index = 0;
			std::vector<Item> pattern_to_remove;
			bool non_existing_item = false;
			while (curr_index < str.length() && !non_existing_item) {
				size_t next_index = str.find(',', curr_index);
				if (next_index == std::string::npos)
					next_index = str.length();
				std::string item = str.substr(curr_index, next_index - curr_index);
				if (dataset.id_by_item.count(item) != 0) {
					pattern_to_remove.push_back(dataset.id_by_item[item]);
				}
				else {
					non_existing_item = true;
				}
				curr_index = next_index + 1;
			}
			if (pattern_to_remove.size() > 0 && !non_existing_item) {
				dataset.patterns_to_remove.push_back(pattern_to_remove);
			}
		}
	}
	else {
		err = ptrfile;
	}
	patterns_to_remove_file.close();
	patterns_to_remove_file.clear();
}

void read_pattern_to_expand(DS& dataset, InputErrorType& err) {
	
	bool non_existing_item = false;
	std::string str;
	std::ifstream pattern_to_expand_file("pattern_to_expand.txt");
	if (pattern_to_expand_file.is_open()) {
		std::getline(pattern_to_expand_file, str);

		size_t curr_index = 0;
		while (curr_index < str.length() - 1 && !non_existing_item) {
			size_t next_index = str.find(',', curr_index);
			if (next_index == std::string::npos)
				next_index = str.length();
			std::string item = str.substr(curr_index, next_index - curr_index);
			if (dataset.id_by_item.count(item) != 0) {
				dataset.pattern_to_expand.push_back(dataset.id_by_item[item]);
			}
			else {
				non_existing_item = true;
				err = ptefile;
			}

			curr_index = next_index + 1;
		}

		if (dataset.pattern_to_expand.size() == 0) {
			err = ptefile;
		}

	}
	else {
		err = ptrfile;
	}
	pattern_to_expand_file.close();
	pattern_to_expand_file.clear();
}