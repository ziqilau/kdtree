/*!
 * Copyright (c) 2016 by Contributors
 * \file kdtree.h
 * \brief interface of KDTree
 * \author Ziqi Liu
 */

#ifndef KDTREE_H_
#define KDTREE_H_

#include <memory>
#include "point.h"

namespace kdtree {

// node in k-d tree
template<typename T>
struct kdnode {
    typedef std::shared_ptr<kdnode<T>> kdnode_ptr;
    typedef std::shared_ptr<const Point<T>> point_ptr;
    kdnode_ptr left;
    kdnode_ptr right;
    size_t axis;
    point_ptr split;
    kdnode(const Point<T>& p) : left(NULL), right(NULL),
        axis(0), split(std::make_shared<const Point<T>>(p)) {}
    kdnode(const Point<T>& p, size_t x) : left(NULL), right(NULL),
        axis(x), split(std::make_shared<const Point<T>>(p)) {}
};

template<typename T>
class KDTree {
public:
    virtual ~KDTree() {}

    // Copy the points into k-d tree from external caller
    virtual void Add(const Point<T>& p) = 0;

    // Build the tree structure
    virtual void Build() = 0;

    virtual const Point<T>* Nearest(const Point<T>& query) const = 0;

    // save model
    virtual void Save(const std::string& name) = 0;

    // load model
    virtual void Load(const std::string& name) = 0;

    // return the dimension of the dataset
    virtual inline const size_t Size() const = 0;
}; //class KDTree

} //namespace kdtree

#endif //KDTREE_H_