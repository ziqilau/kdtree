/*!
 * Copyright (c) 2016 by Contributors
 * \file cycle_criterion.h
 * \brief criterion class cycle the axis and median as position
 * \author Ziqi Liu
 */

#ifndef CYCLE_CRITERION_H_
#define CYCLE_CRITERION_H_

#include "kdtree/criterion.h"
#include <vector>
#include "kdtree/logging.h"
#include "kdtree/kdtree.h"

namespace kdtree {

template<typename T>
class CycleCriterion : public Criterion<T> {
public:
    typedef std::shared_ptr<kdnode<T>> kdnode_ptr;
    CycleCriterion() : dim_(0) {}
    ~CycleCriterion() {}

    const size_t Axis() const override;

    const size_t Position() const override;

    void Set(const std::vector<kdnode_ptr>&, size_t) override;
private:
    // depth
    size_t level_;
    // size of data at current depth
    size_t size_;
    // dimension of the data
    size_t dim_;
};

}

#endif