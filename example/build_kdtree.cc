/*!
 * Copyright (c) 2016 by Contributors
 * \file build_kdtree.cc
 * \brief application build_kdtree
 * \author Ziqi Liu
 */

#include <fstream>
#include <unordered_map>
#include <vector>
#include "kdtree/criterion.h"
#include "kdtree/logging.h"
#include "example_kdtree.h"

using namespace kdtree;

template<typename T>
using Points = std::vector<Point<T>>;

int main (int argc, char** argv) {
    typedef std::unordered_map<std::string, std::string> KWArgs;
    KWArgs kwargs;
    for(int i=1; i<argc; i++) {
        char name[256], val[256];
        if(sscanf(argv[i], "%[^=]=%[^\n]", name, val) == 2) {
            kwargs[name] = val;
        }
    }
    std::string in, out="kdtree_model", type="cycle";
    if( !kwargs.count("in") ) {
        LOG(FATAL) << "usage: build_kdtree in=sample_csv \
[out]=path_to_model [type]=axis_selection_policy";
    }
    in = kwargs["in"];
    if(kwargs.count("out")) out = kwargs["out"];
    if(kwargs.count("type")) type = kwargs["type"];

    Criterion<double>* crt = NULL;
    crt = Criterion<double>::Create(type);

    KDTreeImpl<double> kdt(crt);

    Points<double> samples;
    read_samples<double>(samples, in);

    build_kdtree<double>(kdt, samples, out);

    return 0;
}