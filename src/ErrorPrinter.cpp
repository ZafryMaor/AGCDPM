#include "ErrorPrinter.h"

void print_arguments_error() {
	std::cout << "Expected usage: agdpm transactions_file_path" << std::endl;
	std::cout << "Optional flags (can be in any order but keep the file path right after the -a): -e, -r, -a annotations_file_path." << std::endl;
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
	std::cout << "Could not read \"pattern to expand.txt\" file or the pattern contains an item not existing in the transactions file." << std::endl;
	std::cout << "For more information please visit our github page." << std::endl;
}
