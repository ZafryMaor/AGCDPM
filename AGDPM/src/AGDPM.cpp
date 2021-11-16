#include "AGDPM.h"

int main(int argc, char* argv[])
{
	std::cout.flush();

	InputErrorType err = none;

	std::string transactions_file_path;
	std::string annotaions_file_path;
	bool remove_patterns_flag = false;
	bool pattern_expansion_flag = false;
	bool annotaion_flag = false;
	bool verbose_flag = true;

	read_args(argc, argv, transactions_file_path, annotaions_file_path,
		remove_patterns_flag, pattern_expansion_flag, annotaion_flag, verbose_flag, err);

	if (err == arg) {
		print_arguments_error();
		return 1;
	}

	if(verbose_flag)
		std::cout << "Reading parameters file...\n";
	double min_sup = 1;
	int k = 10;
	int l = 0;
	read_paramerters_file(min_sup, k, l, err);
	
	if (err == param) {
		print_parameters_error();
		return 1;
	}

	std::map<std::string, std::string> item_annotaion_by_item_name;
	if (annotaion_flag) {
		if (verbose_flag)
			std::cout << "Reading annotaions file...\n";
		read_annotaions_file(annotaions_file_path, item_annotaion_by_item_name, err);
	}

	if (err == annofile) {
		print_annotaions_file_error();
		return 1;
	}

	if (verbose_flag)
		std::cout << "Reading transactions file...\n";
	std::map<int, std::string> item_name_by_id;
	std::map<std::string, int> item_id_by_item_name;
	double transactions_labeld_1_counter = 0;
	double number_of_transactions = 0;
	DS transactions = read_transactions_file(transactions_file_path, item_name_by_id, item_id_by_item_name,
		transactions_labeld_1_counter, number_of_transactions, err);

	if (err == dataset) {
		print_transactions_file_error();
		return 1;
	}

	if (remove_patterns_flag) {
		if (verbose_flag)
			std::cout << "Removing patterns by user specification...\n";
		remove_patterns(transactions, item_id_by_item_name, err);
		if (err == ptrfile) {
			print_patterns_to_remove_file_error();
			return 1;
		}
	}

	PatternSet final_set;
	if (pattern_expansion_flag) {
		if (verbose_flag)
			std::cout << "Constructing conditional database of the pattern targeted for expansion...\n";
		Itemset pattern_to_expand = parse_pattern_to_expand(item_id_by_item_name, err);
		if (err == ptefile) {
			print_pattern_to_expand_file_error();
			return 1;
		}
		DS conditional_transactions_of_pattern_to_expand = construct_conditional_transactions(transactions, pattern_to_expand);
		
		if (verbose_flag)
			std::cout << "Mining...\n";
		final_set = mine(conditional_transactions_of_pattern_to_expand, min_sup, k, l,
			transactions_labeld_1_counter, number_of_transactions);
	}
	else {
		if (verbose_flag)
			std::cout << "Mining...\n";
		final_set = mine(transactions, min_sup, k, l,
			transactions_labeld_1_counter, number_of_transactions);
	}
	
	if (verbose_flag)
		std::cout << "Creating output file...\n";
	std::string input_file_name = transactions_file_path.substr(transactions_file_path.find_last_of('\\') + 1);
	std::string output_file_name = "results for " + input_file_name;
	make_outfile(final_set, output_file_name, transactions, item_name_by_id, annotaion_flag, remove_patterns_flag, item_annotaion_by_item_name,
		min_sup, k, l, transactions_labeld_1_counter, number_of_transactions - transactions_labeld_1_counter);
}

