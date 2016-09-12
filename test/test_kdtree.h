#ifndef TEST_KDTREE_H_
#define TEST_KDTREE_H_

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
    int size = oracle.size();
    for(int i=0;i<size;i++) {
        if(oracle[i] != estimate[i]) {
            return false;
        }
    }
    return true;
}

bool Correctness(const std::string& type,
                 const std::string& model,
                 int dim,
                 int num) {
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
    std::remove(model.c_str());
    //evaluation: accuracy
    bool success = evaluation<float>(oracle, estimate);
    return success;
}

#endif
