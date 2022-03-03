#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include "SimpleStructures.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>

void read_args(int& argc, char** argv, std::string& transactions_file_path, std::string& annotaions_file_path,
	bool& remove_patterns_flag, bool& pattern_expansion_flag, bool& annotaion_flag, InputErrorType& err);

void read_paramerters_file(Parameters& param, InputErrorType& err);

void read_annotaions_file(std::string annotaions_file_path, std::map<std::string, std::string>& item_annotaion_by_item_name,
	InputErrorType& err);

void read_transactions_file(DS& dataset, std::string& transactions_file_path, InputErrorType& err);

void read_patterns_to_remove(DS& dataset, InputErrorType& err);

void read_pattern_to_expand(DS& dataset, InputErrorType& err);

#endif // !INPUTHANDLER_H
