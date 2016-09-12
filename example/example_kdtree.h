/*!
 * Copyright (c) 2016 by Contributors
 * \file example_kdtree.h
 * \brief simple wrap up of basic i/o and build_kdtree, query_kdtree
 * \author Ziqi Liu
 */

#ifndef EXAMPLE_KDTREE_H_
#define EXAMPLE_KDTREE_H_

#include <sstream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>
#include "kdtree/point.h"
#include "kdtree/kdtree.h"
#include "kdtree/kdtreeImpl.h"

using namespace kdtree;

template<typename T>
using Points = std::vector<Point<T>>;

// a simple i/o
template<typename T>
void read_samples(Points<T>& samples, const std::string& name) {
    using namespace std;
    ifstream in(name, std::ifstream::in);
    string line;
    string buf;
    T num;
    samples.clear();
    vector<T> tmp;
    size_t ind = 0;
    while(getline(in, line)) {
        stringstream iss(line);
        tmp.clear();
        while(getline(iss, buf, ',')) {
            stringstream(buf) >> num;
            tmp.push_back(num);
        }
        samples.push_back(Point<T>(tmp, ind++));
    }
}

template<typename T>
void build_kdtree(KDTreeImpl<T>& kdt, const Points<T>& samples,
                  const std::string& out) {
    for(size_t i=0; i<samples.size(); i++) {
        kdt.Add(samples[i]);
    }
    kdt.Build();
    kdt.Save(out);
}


template<typename T>
void query_kdtree(KDTreeImpl<T>& kdt, const Points<T>& queries,
                  const std::string& model, const std::string& name) {
    kdt.Load(model);
    std::ofstream out(name);
    for(size_t i=0; i<queries.size(); i++) {
        const Point<T>* q = kdt.Nearest(queries[i]);
        out << q->Index() << "," << std::sqrt(SquaredEuclidean(*q, queries[i]))
            << "\n";
    }
}

#endif