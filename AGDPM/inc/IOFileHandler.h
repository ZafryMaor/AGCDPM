#ifndef IOFILEHANDLER_H
#define IOFILEHANDLER_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdio.h>
#include <fstream>
#include <queue>
#include "Util.h"

enum InputErrorType { none, arg, param, dataset, annofile, ptrfile, ptefile };

void print_arguments_error();
void print_parameters_error();
void print_annotaions_file_error();
void print_transactions_file_error();
void print_patterns_to_remove_file_error();
void print_pattern_to_expand_file_error();

void read_args(int& argc, char** argv, std::string &transactions_file_path, std::string &annotaions_file_path,
	bool &remove_patterns_flag, bool &pattern_expansion_flag, bool &annotaion_flag, bool &verbose_flag, InputErrorType &err);

void read_paramerters_file(double &min_sup,int &k, int &l, InputErrorType &err);

void read_annotaions_file(std::string annotaions_file_path, std::map<std::string, std::string> &item_annotaion_by_item_name,
	InputErrorType &err);

DS read_transactions_file(std::string &transactions_file_path, std::map<int, std::string> & item_by_id,
	std::map<std::string, int>& id_by_item,	double &transactions_labeld_1_counter,
	double &number_of_transactions, InputErrorType &err);

void remove_patterns(DS &transactions, std::map<std::string, int>& id_by_item, InputErrorType &err);

Itemset parse_pattern_to_expand(std::map<std::string, int>& id_by_item, InputErrorType &err);

void make_outfile(PatternSet &final_set, std::string &output_file_name,
	DS &transactions, std::map<int, std::string>& item_by_id,bool annotaion_flag, bool remove_patterns_flag,
	std::map<std::string, std::string> &item_annotaion_by_item_name, double min_sup, int k, int l,double transactions_labeld_1_counter,
	double transactions_labeld_0_counter);

#endif // !IOFILEHANDLER_H
