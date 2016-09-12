/*!
 * Copyright (c) 2016 by Contributors
 * \file criterion.h
 * \brief interface class for axis selection and position used in kdtree
 * \author Ziqi Liu
 */

#ifndef CRITERION_H_
#define CRITERION_H_

#include <string>
#include <memory>
#include "kdtree.h"

namespace kdtree {

template<typename T>
class Criterion {
public:
    static Criterion* Create(const std::string& type);

    typedef std::shared_ptr<kdnode<T>> kdnode_ptr;

    virtual ~Criterion() {}

    virtual const size_t Axis() const = 0;

    virtual const size_t Position() const = 0;

    // set the input data for axis selection and position algorithm
    // used before Axis() and Position()
    virtual void Set(const std::vector<kdnode_ptr>&, size_t) = 0;
};

}

#endif
