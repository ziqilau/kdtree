/*!
 * Copyright (c) 2016 by Contributors
 * \file criterion.cc
 * \brief implement factory functon
 * \author Ziqi Liu
 */
#include "kdtree/criterion.h"
#include <string>
#include "criterion/cycle_criterion.h"
#include "kdtree/logging.h"

namespace kdtree {

// choose different criterion for axis selection and position
template<typename T>
Criterion<T>* Criterion<T>::Create(const std::string& type) {
    if (type == "variance") {
        LOG(FATAL) << "variance criterion unavailable";
    }
    else {
        return new CycleCriterion<T>();
        LOG(INFO) << "unknown criterion type, cycle selected." << type;
    }
    return NULL;
}

template class Criterion<float>;
template class Criterion<double>;

}
