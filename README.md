# Annotated Gene Clusters Discriminative Patterns Miner

Maor M. Zafry
<br>

## Table of contents

* [Introduction](#introduction)
* [Input](#input)
   * [Required](#required)
   * [Optional](#optional)
* [Output](#output)

## Introduction

This repo contains a stand-alone command line application for Linux systems as well as it's source files, that mines discriminative patterns in a specific type of dataset. This tool was designed to find non-redundant and informative discriminative patterns in transactions datasets, where each transaction corresponds to a gene cluster. The items in the transactions are the genes found in the cluster represented in any string form - can be the annotated product, a "COG" of "Pfam" id etc. Each transaction has a binary label corresponding to a pre-selected class of the cluster, and an "origin" that corresponds to the genome it originated from such that the patterns are considered based on the number of genomes they are found in rather than the number of clusters.<br>

## Input
The general command for running the application is:<br>
./agcdpm [path_to_transaction_dataset_file] [-r] [-e] [-a path_to_item_annotations_file]

### Required

There are two required files for the use of our application. The first file required for the application is a "parameters.txt" file containing the values of the parameters, and must be found in the same directory as the application (an example for the file can be found in the application directory). The application requires 3 parameters:
  1) Minimum support (min_sup): the minimum percent of genomes required for a pattern to be found in to be concidered - float value 0-100%.
  2) k: the number of patterns to be given in the output - integer value greater than 0.
  3) l: the maximum length of patterns to be considered - integer value greater than 0 or 0 for unlimited length (not recommended).<br><br>


The second file required is the dataset file containing the transactions dataset, and must be given in the run command of the application. Each transaction should be in a new line (without blank line between transactions). The begining of a transaction until the first ',' is the "origin" of the transaction. After the first ',' the itemset of the transaction should be found where each item is separated by a ',' and the items name should not include a ',' character. after the last ',' a binary (0 or 1) label should be found whith no other charecters after it. Simple example of a transaction:<br>
origin1,i1,i2,i3,1

### Optional

There are three optional methods of the application that may be used by adding a flag to the command. 
  1) The -r flag allows the user to remove from the dataset the items found in patterns given to the application in a file with the name "patterns_to_remove.txt" located in the application directory. This file should contain itemsets in separate lines where each item is separated by a ','.<br> 
  2) The -e flag allows the user to "expand" a single pattern, considering only discriminative pattens that include this pattern. In order use this method a file named "pattern_to_expand.txt" located in the application's directory, containing a single itemset (a single line with item names separated by ',').<br>
  3) The -a flag allows the user to add item annotations (in case the item names are ids like "COG1337" for example) to be presented with each pattern in the output. The path to the annotations file must be given right after the -a flag. The annotations file should include a line for each item id where the line starts with the id followed by a tab and then the annotation in a string form. The annotation must not contain new line characters.<br>

Examples for optional files can be found in the optional files directory.<br>

## Output
The application outputs three files to the application's directory:<br>
  1) Results: the file containing the discriminative patterns found by the application named "results for [transaction dataset file name]". Each result presented includes the        minimal and completed form (includes all items found with the pattern in every instance) of the pattern, the Information Gain score of the pattern, the support of the            pattern in each label, the p-value and q'-value of the pattern and optionally the annotaion for each of the items in the pattern (in case the -a flag was used).
  2) Redundant patterns: a file containing the patterns found ordered in decreasing order by a redundancy score given to each pattern (higher redundancy score is more                redundant) named "potentially_redundent_for_[transaction dataset file name]".
  3) Patterns to remove: if the -r flag was not used a file containing all patterns found with a redundancy score greater than 1 named                                                "patterns_to_remove_for_[transaction dataset file name]" is created. If the -r flag was used the patterns found with a redundancy score greater than 1 are automaticlly           added to the end of the given "patterns_to_remove.txt" file.



