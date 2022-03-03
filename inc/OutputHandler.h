#ifndef OUTPUTHANDLER_H
#define OUTPUTHANDLER_H

#include "SimpleStructures.h"
#include "Miner.h"
#include "Statistics.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>

void make_outfile(Miner& miner, std::string& output_file_name, DS& dataset, bool annotaion_flag, bool remove_patterns_flag,
	std::map<std::string, std::string>& item_annotaion_by_item_name, Parameters params);

#endif // !OUTPUTHANDLER_H
