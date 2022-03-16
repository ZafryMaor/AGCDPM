#include "DBAMiner.h"


int main(int argc, char* argv[])
{
	std::cout.flush();

	InputErrorType err = none;

	DS dataset;
	Parameters params;

	std::string transactions_file_path;
	std::string annotaions_file_path;
	bool remove_patterns_flag = false;
	bool pattern_expansion_flag = false;
	bool annotaion_flag = false;

	read_args(argc, argv, transactions_file_path, annotaions_file_path,
		remove_patterns_flag, pattern_expansion_flag, annotaion_flag, err);

	if (err == arg) {
		print_arguments_error();
		return 1;
	}

	std::cout << "Reading parameters file...\n";

	read_paramerters_file(params, err);

	if (err == param) {
		print_parameters_error();
		return 1;
	}

	std::map<std::string, std::string> item_annotaion_by_item_name;
	if (annotaion_flag) {
		std::cout << "Reading annotaions file...\n";
		read_annotaions_file(annotaions_file_path, item_annotaion_by_item_name, err);
	}

	if (err == annofile) {
		print_annotaions_file_error();
		return 1;
	}

	std::cout << "Reading transactions file...\n";

	read_transactions_file(dataset, transactions_file_path, err);

	if (err == ds) {
		print_transactions_file_error();
		return 1;
	}

	if (remove_patterns_flag) {
		std::cout << "Reading patterns to remove file...\n";
		read_patterns_to_remove(dataset, err);
		if (err == ptrfile) {
			print_patterns_to_remove_file_error();
			return 1;
		}
	}

	if (pattern_expansion_flag) {
		std::cout << "Reading pattern to expand file...\n";
		read_pattern_to_expand(dataset, err);
		if (err == ptefile) {
			print_pattern_to_expand_file_error();
			return 1;
		}
	}

	Miner miner = Miner(&dataset, params);
	std::cout << "Mining...\n";
	miner.mine();

	std::cout << "Creating output file...\n";
	std::string input_file_name = transactions_file_path.substr(transactions_file_path.find_last_of('\\') + 1);
	std::string output_file_name = "results for " + input_file_name;
	make_outfile(miner, output_file_name, dataset, annotaion_flag, remove_patterns_flag,
		item_annotaion_by_item_name, params);
}