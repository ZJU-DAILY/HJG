//
// Copyright (c) 2017 ZJULearning. All rights reserved.
//
// This source code is licensed under the MIT license.
//

#include <efanna2e/index_random.h>


namespace efanna2e {

IndexRandom::IndexRandom(const size_t dimension, const size_t n, Metric m):Index(dimension, n, m){
  has_built = true;
}
IndexRandom::~IndexRandom() {}
void IndexRandom::Build(size_t n, const float *data, const Parameters &parameters) {
  data_ = data;
  nd_ = n;

  // Do Nothing

  has_built = true;
}
void IndexRandom::Build(size_t n, vector<string> data, const Parameters &parameters) {
  data_2 = data;
  nd_ = n;

  // Do Nothing

  has_built = true;
}
void IndexRandom::Search(const float *query, const float *x, size_t k, const Parameters &parameters, unsigned *indices) {

    GenRandom(rng, indices, k, nd_);
}

void IndexRandom::Search(string query, vector<string >x, size_t k, const Parameters &parameters, unsigned *indices) {

    GenRandom(rng, indices, k, nd_);
}

}
