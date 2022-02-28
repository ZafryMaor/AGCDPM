# Annotated Gene Clusters Discriminative Patterns Miner

Maor M. Zafry
<br>

## Table of contents

* [Introduction](#introduction)
* [Input](#input)
   * [Required](#required-input)
   * [Optional](#optional-input)
* [Output](#output)

## Introduction

This repo contains a stand-alone command line application for Linux systems as well as it's source files, that mines discriminative patterns in a specific type of dataset. This tool was designed to find non-redundant and informative discriminative patterns in transactions datasets, where each transaction corresponds to a gene cluster. The items in the transactions are the genes found in the cluster represented in any string form - can be the annotated product, a "COG" of "Pfam" id etc. Each transaction has a binary label corresponding to a pre-selected class of the cluster, and an "origin" that corresponds to the genome it originated from such that the patterns are considered based on the number of genomes they are found in rather than the number of clusters.<br>

## Input






