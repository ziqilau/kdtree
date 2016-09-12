/*!
 * Copyright (c) 2016 by Contributors
 * \file test_kdtree.h
 * \brief test the correctness of kdtree compared with the results of brutal force
 * \author Ziqi Liu
 */
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <omp.h>

#include "kdtree/point.h"
#include "kdtree/kdtree.h"
#include "kdtree/criterion.h"
#include "kdtree/kdtreeImpl.h"
#include "kdtree/logging.h"

#include "example_kdtree.h"

using namespace kdtree;

template<typename T>
using Points = std::vector<Point<T>>;

template<typename T>
void generate_samples(Points<T>& samples, int num, int dim) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_real_distribution<T> distribution(0.0,1.0);
    for(int i=0;i<num;i++) {
        Point<T> point;
        for(int j=0;j<dim;j++) {
            T r = distribution(generator);
            point.Push(r);
        }
        point.SetIndex((size_t)i);
        samples.push_back(point);
    }
}

template<typename T>
void brutal_force_nearest(const Points<T>& samples,
                          const Points<T>& queries,
                          std::vector<size_t>& oracle) {
    size_t size = queries.size();
    oracle.resize(size);
#pragma omp parallel for
    for(uint32_t i=0;i<queries.size();i++) {
        T best_dist = std::numeric_limits<T>::max();
        size_t best_ind=0;
        for(size_t j=0;j<samples.size();j++) {
            T dist = std::sqrt(SquaredEuclidean(queries[i], samples[j]));
            if(dist < best_dist) {
                best_ind = j;
                best_dist = dist;
            }
        }
        oracle[i] = best_ind;
    }
}
template<typename T>
void kdtree_nearest(const Points<T>& queries,
                    std::vector<size_t>& estimate,
                    KDTreeImpl<T>& kdt) {
    size_t size = queries.size();
    estimate.resize(size);
#pragma omp parallel for
    for(size_t i=0;i<queries.size();i++) {
        const Point<T>* p = kdt.Nearest(queries[i]);
        estimate[i] = p->Index();
    }
}

template<typename T>
bool evaluation(const std::vector<size_t>& oracle, const std::vector<size_t>& estimate) {
    std::cout << "# : oracle : kd-tree\n";
    int size = oracle.size();
    for(int i=0;i<size;i++) {
        std::cout << i << " : " << oracle[i] << " : " << estimate[i] << "\n";
        if(oracle[i] != estimate[i]) {
            return false;
        }
    }
    return true;
}

int main (int argc, char** argv) {
    typedef std::unordered_map<std::string, std::string> KWArgs;
    KWArgs kwargs;
    for(int i=1; i<argc; i++) {
        char name[256], val[256];
        if(sscanf(argv[i], "%[^=]=%[^\n]", name, val) == 2) {
            kwargs[name] = val;
        }
    }
    std::string type, model="kdtree_model";
    int dim = 10, num = 100;
    if(kwargs.count("type")) type = kwargs["type"];
    if(kwargs.count("dim")) dim = std::stoi(kwargs["dim"]);
    if(kwargs.count("num")) num = std::stoi(kwargs["num"]);

    Criterion<float>* crt = NULL;
    crt = Criterion<float>::Create(type);
    KDTreeImpl<float> kdt(crt);

    //randomly generate samples
    Points<float> samples;
    generate_samples<float>(samples, num, dim);
    //randomly generate queries
    Points<float> queries;
    generate_samples<float>(queries, num, dim);
    //brutal force results
    std::vector<size_t> oracle;
    brutal_force_nearest<float>(samples, queries, oracle);
    //k-d tree results
    std::vector<size_t> estimate;
    build_kdtree<float>(kdt, samples, model);
    kdt.Load(model);
    kdtree_nearest<float>(queries, estimate, kdt);
    //evaluation: accuracy
    bool success = evaluation<float>(oracle, estimate);

    if(success) std::cout << "test passed!\n";
    else std::cout << "test failed!\n";

    return 0;
}