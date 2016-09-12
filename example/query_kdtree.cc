/*!
 * Copyright (c) 2016 by Contributors
 * \file query_kdtree.cc
 * \brief application query_kdtree
 * \author Ziqi Liu
 */

#include <sstream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include "example_kdtree.h"
#include "kdtree/criterion.h"
#include "kdtree/logging.h"

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
    std::string query, model="kdtree_model", type="cycle", out="result.csv";
    if( !kwargs.count("query") || !kwargs.count("model") ) {
        LOG(FATAL) << "usage: query_kdtree query=query_csv \
model=path_to_model [out]=output [type]=axis_selection_policy";
    }
    query = kwargs["query"];
    model = kwargs["model"];
    if(kwargs.count("type")) type = kwargs["type"];
    if(kwargs.count("out")) out = kwargs["out"];

    Criterion<double>* crt = NULL;
    crt = Criterion<double>::Create(type);

    KDTreeImpl<double> kdt(crt);

    Points<double> queries;
    read_samples<double>(queries, query);

    query_kdtree<double>(kdt, queries, model, out);

    return 0;
}