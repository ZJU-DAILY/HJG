//
// Created by 付聪 on 2017/6/21.
//

#include <efanna2e/index_graph.h>
#include <efanna2e/index_random.h>
#include <efanna2e/util.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include<vector>
#include <cstdlib> // Header file needed to use srand and rand
#include <ctime> // Header file needed to use time

using namespace std;

struct DataN {
	unsigned id;
  float* data;
  float dis;
  DataN() = default;
  
  inline bool operator<(const DataN &other) const {
        return dis < other.dis;
  }
};

struct DataS {
	unsigned id;
  string data;
  float dis;
  DataS() = default;
  
  inline bool operator<(const DataS &other) const {
        return dis < other.dis;
  }
};

std::vector<DataN> dataN;
struct DataN dn;
std::vector<DataS> dataS;
struct DataS ds;

void split(const string &str,vector<string> &res,const char pattern)
{
	istringstream is(str);
  string temp;
  while(getline(is,temp,pattern))
  	res.push_back(temp);//stoi(temp)转整型
  return;
}

void load_data(char* filename, std::vector<DataN> &dataN, unsigned& num,unsigned& dim){
  std::ifstream in(filename);
  if(!in.is_open()){std::cout<<"open file error"<<std::endl;exit(-1);}
	
	string line;
	int i = 0;
	int j = 0;
	vector<string> res;
	
	while(getline(in, line)) {//将in文件中的每一行字符读入到string line中
    if(i == 1){
    	split(line,res,' ');
    	for(auto r:res){
				stringstream ss(r);
				float number;
				ss >> number;
    		if(j == 0) num = number;
    		if(j == 1) dim = number;
    		j++;
    	}
		}else if(i > 1){
			dataN.push_back(dn);
			dataN[i-2].data = new float[dim * sizeof(float)];
			dataN[i-2].id = i - 2;
			split(line,res,' ');
    	for(auto r:res){
    		stringstream ss(r);
				float number;
				ss >> number;
				*(dataN[i-2].data + j) = number;
				j++;				
    	}
		}
		
		res.clear();
		j = 0;
		i++;
    
	}
  
  /*for(size_t i = 0; i < num; i++) {	//输出数据
  	for(int j = 0; j < dim; j++){
  		std::cout << *(dataN[i].data + j) << " ";
  	}
  	std::cout << std::endl;
  }*/
  in.close();
}

void load_data(char* filename, std::vector<DataS> &dataS, unsigned& num,unsigned& dim){
	std::ifstream in(filename);
  if(!in.is_open()){std::cout<<"open file error"<<std::endl;exit(-1);}
	
	string line;
	int i = 0;
	vector<string> res;
	
	while(getline(in, line)) {//将in文件中的每一行字符读入到string line中
		if(i == 1){
			split(line,res,' ');
			stringstream nums(res[0]);
			nums >> num;
			stringstream dims(res[1]);
			dims >> dim;
		}else if(i > 1){
			//data.push_back(line);
			dataS.push_back(ds);
			dataS[i-2].data = line;
			dataS[i-2].id = i - 2;
		}
		
		i++;
  }
  
  /*for(size_t i = 0; i < num * dim; i++) {	//输出数据
    	std::cout << dataS[i].data << endl;
  }*/
  
  in.close();
}

float getDis(const float *a, const float *b, unsigned size, unsigned metric){
	if(metric == 0){
		efanna2e::Distance* d = new efanna2e::DistanceL2();
		float dist = d->compare(a, b, size);
		return dist;
	}else if(metric == 1){
		efanna2e::Distance* d = new efanna2e::DistanceL1();
		float dist = d->compare(a, b, size);
		return dist;
	}else{
		efanna2e::Distance* d = new efanna2e::DistanceCosDis();
		float dist = d->compare(a, b, size);
		return dist;
	}
}

float getDis(const string a, const string b, unsigned size, unsigned metric){
	efanna2e::Distance* d = new efanna2e::DistanceLDistance();
	float dist = d->compare(a, b, size);
	return dist;
}	

int main(int argc, char** argv){
  
  std::ifstream in(argv[2]);
  if(!in.is_open()){std::cout<<"open file error"<<std::endl;exit(-1);}
  
  unsigned points_num, dim;
  unsigned metric;
  string line;
  
  unsigned l;
  stringstream ss(argv[1]);
	ss >> l;
  const unsigned level = l;
  
  while(getline(in, line)){
  	stringstream ss(line);
		ss >> metric;
  	break;
  }
  
  if(metric == 0){
  	load_data(argv[2], dataN, points_num, dim);
  	
  	unsigned K = (unsigned)atoi(argv[3 + level]);
		unsigned L = (unsigned)atoi(argv[4 + level]);
		unsigned iter = (unsigned)atoi(argv[5 + level]);
		unsigned S = (unsigned)atoi(argv[6 + level]);
		unsigned R = (unsigned)atoi(argv[7 + level]);
		
    // unsigned pivot = 7167;
    unsigned pivot = 64;
    
    /*unsigned seed;  // Random generator seed
    // Use the time function to get a "seed” value for srand
    seed = time(0);
    srand(seed);
    // Now generate and print three random numbers
    unsigned random = rand() % points_num;
    float max_dis = 0;
    unsigned pivot;
    for(unsigned i = 0; i < points_num; i++){
    	float dis = getDis(dataN[i].data, dataN[random].data, dim, metric);
    	if(dis > max_dis){
    		pivot = i;
    		max_dis = dis;
    	}
    }
    cout << pivot << endl;*/
    
    for(int i = 0; i < points_num; i++){
    	float dis = getDis(dataN[i].data, dataN[pivot].data, dim, metric);
    	dataN[i].dis = dis;
    }
    sort(dataN.begin(), dataN.begin() + points_num);
    
    float* data_load[level];
    unsigned level_num[level];
    int pos = 0;
    
    for(unsigned i = 0; i < level; i++){
    	vector<unsigned> mapToId;
    	
    	if(i < level - 1) level_num[i] = points_num / level;
    	if(i == level - 1) level_num[i] = points_num - (level - 1) * points_num / level;
    	data_load[i] = new float[level_num[i] * dim * sizeof(float)];
    	
    	for(unsigned j = 0; j < level_num[i]; j++){
    		mapToId.push_back(dataN[pos + j].id);
    		for(unsigned k = 0; k < dim; k++){
		  		*(data_load[i] + j * dim + k) = *(dataN[pos + j].data + k);
		  		k++;
    		}
    	}
    	pos += level_num[i];
    	char* graph_filename = argv[3 + i];
			
			efanna2e::IndexRandom init_index(dim, level_num[i], efanna2e::L2);
			efanna2e::IndexGraph index(dim, level_num[i], efanna2e::L2, (efanna2e::Index*)(&init_index));
			
			std::cout << "dim: " << dim << std::endl;
  		std::cout << "level_num[i]: " << level_num[i] << std::endl;

			efanna2e::Parameters paras;
			paras.Set<unsigned>("K", K);
			paras.Set<unsigned>("L", L);
			paras.Set<unsigned>("iter", iter);
			paras.Set<unsigned>("S", S);
			paras.Set<unsigned>("R", R);

			auto s = std::chrono::high_resolution_clock::now();
			index.Build(level_num[i], data_load[i], paras);
			index.setPivot(pivot);
			auto e = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> diff = e-s;
			std::cout <<"Time cost: "<< diff.count() << "s" << "\n";
			
			FILE* time = fopen("time.txt", "a");
			fprintf(time, "%f", diff.count());
			fprintf(time, "\n");
			fflush(time);

			index.SaveGraph(graph_filename, mapToId);
    }

  }else if(metric == 1){
  	load_data(argv[2], dataN, points_num, dim);
  	
  	unsigned K = (unsigned)atoi(argv[3 + level]);
		unsigned L = (unsigned)atoi(argv[4 + level]);
		unsigned iter = (unsigned)atoi(argv[5 + level]);
		unsigned S = (unsigned)atoi(argv[6 + level]);
		unsigned R = (unsigned)atoi(argv[7 + level]);
  	
  	// unsigned pivot = 7167;
  	unsigned pivot = 64;
  	
  	/*unsigned seed;  // Random generator seed
    // Use the time function to get a "seed” value for srand
    seed = time(0);
    srand(seed);
    // Now generate and print three random numbers
    unsigned random = rand() % points_num;
    float max_dis = 0;
    unsigned pivot;
    for(unsigned i = 0; i < points_num; i++){
    	float dis = getDis(dataN[i].data, dataN[random].data, dim, metric);
    	if(dis > max_dis){
    		pivot = i;
    		max_dis = dis;
    	}
    }
    cout << pivot << endl;*/
    
    for(int i = 0; i < points_num; i++){
    	float dis = getDis(dataN[i].data, dataN[pivot].data, dim, metric);
    	dataN[i].dis = dis;
    }
    sort(dataN.begin(), dataN.begin() + points_num);
    
    float* data_load[level];
    unsigned level_num[level];
    int pos = 0;
    
    for(unsigned i = 0; i < level; i++){
    	vector<unsigned> mapToId;
    	
    	if(i < level - 1) level_num[i] = points_num / level;
    	if(i == level - 1) level_num[i] = points_num - (level - 1) * points_num / level;
    	data_load[i] = new float[level_num[i] * dim * sizeof(float)];
    	
    	for(unsigned j = 0; j < level_num[i]; j++){
    		mapToId.push_back(dataN[pos + j].id);
    		for(unsigned k = 0; k < dim; k++){
		  		*(data_load[i] + j * dim + k) = *(dataN[pos + j].data + k);
		  		k++;
    		}
    	}
    	pos += level_num[i];
    	char* graph_filename = argv[3 + i];
			
			efanna2e::IndexRandom init_index(dim, level_num[i], efanna2e::L1);
			efanna2e::IndexGraph index(dim, level_num[i], efanna2e::L1, (efanna2e::Index*)(&init_index));
			
			std::cout << "dim: " << dim << std::endl;
  		std::cout << "level_num[i]: " << level_num[i] << std::endl;

			efanna2e::Parameters paras;
			paras.Set<unsigned>("K", K);
			paras.Set<unsigned>("L", L);
			paras.Set<unsigned>("iter", iter);
			paras.Set<unsigned>("S", S);
			paras.Set<unsigned>("R", R);

			auto s = std::chrono::high_resolution_clock::now();
			index.Build(level_num[i], data_load[i], paras);
			index.setPivot(pivot);
			auto e = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> diff = e-s;
			std::cout <<"Time cost: "<< diff.count() << "s" << "\n";
			
			FILE* time = fopen("time.txt", "a");
			fprintf(time, "%f", diff.count());
			fprintf(time, "\n");
			fflush(time);

			index.SaveGraph(graph_filename, mapToId);
    }
  }else if(metric == 2){
  	load_data(argv[2], dataN, points_num, dim);
  	
  	unsigned K = (unsigned)atoi(argv[3 + level]);
		unsigned L = (unsigned)atoi(argv[4 + level]);
		unsigned iter = (unsigned)atoi(argv[5 + level]);
		unsigned S = (unsigned)atoi(argv[6 + level]);
		unsigned R = (unsigned)atoi(argv[7 + level]);
  	
  	// unsigned pivot = 7167;
  	unsigned pivot = 64;
    
    for(int i = 0; i < points_num; i++){
    	float dis = getDis(dataN[i].data, dataN[pivot].data, dim, metric);
    	dataN[i].dis = dis;
    }
    sort(dataN.begin(), dataN.begin() + points_num);
    
    float* data_load[level];
    unsigned level_num[level];
    int pos = 0;
    
    for(unsigned i = 0; i < level; i++){
    	vector<unsigned> mapToId;
    	
    	if(i < level - 1) level_num[i] = points_num / level;
    	if(i == level - 1) level_num[i] = points_num - (level - 1) * points_num / level;
    	data_load[i] = new float[level_num[i] * dim * sizeof(float)];
    	
    	for(unsigned j = 0; j < level_num[i]; j++){
    		mapToId.push_back(dataN[pos + j].id);
    		for(unsigned k = 0; k < dim; k++){
		  		*(data_load[i] + j * dim + k) = *(dataN[pos + j].data + k);
		  		k++;
    		}
    	}
    	pos += level_num[i];
    	char* graph_filename = argv[3 + i];
			
			efanna2e::IndexRandom init_index(dim, level_num[i], efanna2e::CosDis);
			efanna2e::IndexGraph index(dim, level_num[i], efanna2e::CosDis, (efanna2e::Index*)(&init_index));
			
			std::cout << "dim: " << dim << std::endl;
  		std::cout << "level_num[i]: " << level_num[i] << std::endl;

			efanna2e::Parameters paras;
			paras.Set<unsigned>("K", K);
			paras.Set<unsigned>("L", L);
			paras.Set<unsigned>("iter", iter);
			paras.Set<unsigned>("S", S);
			paras.Set<unsigned>("R", R);

			auto s = std::chrono::high_resolution_clock::now();
			index.Build(level_num[i], data_load[i], paras);
			index.setPivot(pivot);
			auto e = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> diff = e-s;
			std::cout <<"Time cost: "<< diff.count() << "s" << "\n";
			
			FILE* time = fopen("time.txt", "a");
			fprintf(time, "%f", diff.count());
			fprintf(time, "\n");
			fflush(time);

			index.SaveGraph(graph_filename, mapToId);
    }
  }else{
  	load_data(argv[2], dataS, points_num, dim);
  	
  	unsigned K = (unsigned)atoi(argv[3 + level]);
		unsigned L = (unsigned)atoi(argv[4 + level]);
		unsigned iter = (unsigned)atoi(argv[5 + level]);
		unsigned S = (unsigned)atoi(argv[6 + level]);
		unsigned R = (unsigned)atoi(argv[7 + level]);
  	
  	// unsigned pivot = 7167;
  	unsigned pivot = 64;
    
    for(int i = 0; i < points_num; i++){
    	float dis = getDis(dataS[i].data, dataS[pivot].data, dim, metric);
    	dataS[i].dis = dis;
    }
    sort(dataS.begin(), dataS.begin() + points_num);
    
    vector<vector<string>> data_load(level);
    unsigned level_num[level];
    int pos = 0;
    
    for(unsigned i = 0; i < level; i++){
    	vector<unsigned> mapToId;
    	
    	if(i < level - 1) level_num[i] = points_num / level;
    	if(i == level - 1) level_num[i] = points_num - (level - 1) * points_num / level;
    	
    	for(unsigned j = 0; j < level_num[i]; j++){
    		mapToId.push_back(dataS[pos + j].id);
    		data_load[i].push_back(dataS[pos + j].data);
    	}
    	pos += level_num[i];
    	char* graph_filename = argv[3 + i];
			
			efanna2e::IndexRandom init_index(dim, level_num[i], efanna2e::LDistance);
			efanna2e::IndexGraph index(dim, level_num[i], efanna2e::LDistance, (efanna2e::Index*)(&init_index));
			
			std::cout << "dim: " << dim << std::endl;
  		std::cout << "level_num[i]: " << level_num[i] << std::endl;

			efanna2e::Parameters paras;
			paras.Set<unsigned>("K", K);
			paras.Set<unsigned>("L", L);
			paras.Set<unsigned>("iter", iter);
			paras.Set<unsigned>("S", S);
			paras.Set<unsigned>("R", R);

			auto s = std::chrono::high_resolution_clock::now();
			index.Build(level_num[i], data_load[i], paras);
			index.setPivot(pivot);
			auto e = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> diff = e-s;
			std::cout <<"Time cost: "<< diff.count() << "s" << "\n";
			
			FILE* time = fopen("time.txt", "a");
			fprintf(time, "%f", diff.count());
			fprintf(time, "\n");
			fflush(time);

			index.SaveGraph(graph_filename, mapToId);
  	}
 	}
  return 0;
}
