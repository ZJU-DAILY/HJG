
## Introduction

This repo holds the source code and scripts for “HJG: An Effective Hierarchical Joint Graph for Approximate Nearest Neighbour Search in Multi-Metric Spaces”

## Usage

1. Datasets

The used datasets has the following files:
	*.txt
	*_q.txt	
The first file contains all the data, and the first line for datasets denotes the size, the dimention.
The second line describe the used metrics (taking 0 as an example, it means L-1 Norm Distance) and the third line provide the maximum distance for each metric.
The second file contains the ids  for the quries.

2. Indexes

We implemented all the indexes using C++ with Visual Studio 2019.
This file contains the source code files for RR*-tree, DESIRE, SNSSG, SNSSG* and our HJG 
 In the sub folder, file 'data' contains the dataset, the query file and the execute parameters. 
When using different dataset, modify the file 'par.txt', where the first parameter is the dataset path and the second is the query file path.


## Compared algorithms

| __Algorithm__ | __Paper__ | __Year__ |
|-------------|------------|------------|
RR*-tree   | Indexing Multi-Metric Data | 2016
DESIRE   | DESIRE: An Efficient Dynamic Cluster-based Forest Indexing for Similarity Search in Multi-Metric Spaces | 2022



## Datasets

Each dataset can be obtained from the following links. 

| __Dataset__ | __link__ | __Metrics__ |
|-------------|------------|------------|
| Hornets        |  https://agr.wa.gov/departments/insects-pests-and-weeds/insects/hornets/data  | 4 |
| Rental      | https://www.kaggle.com/c/two-sigma-connect-rental-listing-inquiries | 5 |
| Food         | https://world.openfoodfacts.org/data | 9 |


Two sample datasets are included in the folder 'Dataset'.

