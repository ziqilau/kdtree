/*!
 * Copyright (c) 2016 by Contributors
 * \file point.h
 * \brief define basic point class
 * \author Ziqi Liu
 */

#ifndef POINT_H_
#define POINT_H_

#include "kdtree/logging.h"
#include <vector>

namespace kdtree {

template<typename T>
class Point {
public:
//    template <typename ... Args>
//        Point(const Args& ... args) :
//    coordinates_{args...} {}
    Point() : index_(0) {}
    Point(std::vector<T>& v, size_t ind) : coordinates_(v), index_(ind) {}
    Point(const Point<T>& p) : coordinates_(p.Coordinate()), index_(p.Index()) {}

    inline const std::vector<T>& Coordinate() const {
        return coordinates_;
    }
    inline size_t Index() const {
        return index_;
    }

    inline const size_t Size() const {
        return coordinates_.size();
    }

    inline void Push(const T& e) {
        coordinates_.push_back(e);
    }

    inline void SetIndex(size_t ind) {
        index_ = ind;
    }

    inline void Clear() {
        coordinates_.clear();
    }

    T const& operator[](int const i) const {
        return coordinates_[i];
    }

    friend T SquaredEuclidean(const Point& a, const Point& b) {
        T res=0;
        CHECK_EQ(a.Size(), b.Size()) << "Dimension inconsistent: " << a.Size() << "vs. " << b.Size();
        size_t dim = a.Size();
        for(size_t i=0;i<dim;i++) {
            res+=(a[i]-b[i])*(a[i]-b[i]);
        }
        return res;
    }
    friend T AxisEuclidean(const Point& a, const Point& b, size_t axis) {
        CHECK_EQ(a.Size(), b.Size()) << "Dimension inconsistent";
        CHECK_GT(a.Size(), axis) << "Dimension less equal than axis";
        return a[axis]-b[axis];
    }

private:
    std::vector<T> coordinates_;
    size_t index_;
}; //class Point

} // namespace kdtree

#endif //POINT_H_