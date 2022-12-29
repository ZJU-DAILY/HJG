//
// Copyright (c) 2017 ZJULearning. All rights reserved.
//
// This source code is licensed under the MIT license.
//
#include <efanna2e/index.h>
namespace efanna2e {
Index::Index(const size_t dimension, const size_t n, Metric metric)
  : dimension_ (dimension), nd_(n), has_built(false) {
    switch (metric) {
      case L2:distance_ = new DistanceL2();
        break;
      case INNER_PRODUCT:distance_ = new DistanceInnerProduct();
        break;
      case FAST_L2:distance_ = new DistanceFastL2();
        break;
      case L1:distance_ = new DistanceL1();
        break;
      case LDistance:distance_ = new DistanceLDistance();
        break;
      case CosDis:distance_ = new DistanceCosDis();
        break;
      default:distance_ = new DistanceL2();
        break;
    }
}
Index::~Index() {}
}
