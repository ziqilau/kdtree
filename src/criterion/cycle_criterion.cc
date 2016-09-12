/*!
 * Copyright (c) 2016 by Contributors
 * \file cycle_criterion.cc
 * \brief Implementation of cycle_criterion class
 * \author Ziqi Liu
 */

#include "criterion/cycle_criterion.h"
#include <memory>
#include <vector>
#include "kdtree/logging.h"

namespace kdtree {

    template<typename T>
    using kdnode_ptr = std::shared_ptr<kdnode<T>>;

    template<typename T>
    const size_t CycleCriterion<T>::Axis() const {
        CHECK_GT(dim_, 0) << "Dimension zero, Init dimension first";
        return level_% dim_;
    }

    template<typename T>
    const size_t CycleCriterion<T>::Position() const {
        return size_/2;
    }

    template<typename T>
    void CycleCriterion<T>::Set(const std::vector<kdnode_ptr>& nodes, size_t level) {
        level_ = level;
        size_ = nodes.size();
        if(nodes.size()) dim_ = nodes.front()->split->Size();
    }

    template class CycleCriterion<float>;
    template class CycleCriterion<double>;
}
