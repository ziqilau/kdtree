/*!
 * Copyright (c) 2016 by Contributors
 * \file kdtreeImpl.h
 * \brief an implementation of interface kdtree
 * \author Ziqi Liu
 */

#ifndef KDTREE_IMPL_H_
#define KDTREE_IMPL_H_

#include "kdtree/kdtree.h"

#include <vector>
#include <stack>
#include <queue>
#include <memory>
#include <algorithm>

#include "recordio/kdtree.pb.h"
#include "kdtree/criterion.h"
#include "kdtree/point.h"
#include "kdtree/logging.h"

namespace kdtree {

template<typename T>
class KDTreeImpl : public KDTree<T> {
public:
    KDTreeImpl(Criterion<T>* ctr) : root_(NULL), dim_(0), criterion_(ctr){}
    KDTreeImpl() : root_(NULL), dim_(0), criterion_(NULL) {}
    virtual ~KDTreeImpl() {}

    typedef std::shared_ptr<kdnode<T>> kdnode_ptr;

    struct best_estimate {
        kdnode_ptr node;
        T distance;
        best_estimate(kdnode_ptr n, T dist) : node(n), distance(dist) {}
    };

    inline const size_t Size() const override {
        return dim_;
    }

    void Add(const Point<T>& p) override {
        kdnode_ptr node = std::make_shared<kdnode<T>>(p);
        nodes_.push_back(node);
        if(dim_ == 0) dim_ = p.Size();
        if(dim_ != 0 && dim_ != p.Size())
        LOG(ERROR) << "Dimension inconsistent";
    }
    void Build() override {
        if (nodes_.empty()) return;
        root_ = _Build(nodes_, 0);
    }
    const Point<T>* Nearest(const Point<T>& query) const override {
        CHECK_EQ(query.Size(), dim_) << "query dimension not equal to \
model dimension: " << query.Size() << " != " << dim_;
        return _Nearest(query);
    }
    void Save(const std::string& name) override {
        FILE* out = fopen(name.c_str(), "wb");
        Serialize(root_, out);
        fclose(out);
    }
    void Load(const std::string& name) override {
        FILE* in = fopen(name.c_str(), "rb");
        root_ = Deserialize(in);
        fclose(in);
        UpdateDim();
    }

private:
    // internal vector for store the points read from external caller
    // by this->Add(const Point<T>& p)
    std::vector<kdnode_ptr> nodes_;
    // the root of k-d tree
    kdnode_ptr root_;
    // dimension of the point
    size_t dim_;
    // criterion used for axis selecton and position
    Criterion<T>* criterion_;

    // equip a protobuf message by kdnode
    inline void Equip(KDTreeRec::KDNodeRec& node, bool isnode, kdnode_ptr root) {
        node.Clear();
        node.set_isnode(isnode);
        if(isnode) {
            node.set_index(root->split->Index());
            node.set_axis(root->axis);
            for(size_t i=0; i<dim_; i++) {
                node.add_point((*root->split)[i]);
            }
        }
    }

    // update the dimension when needed
    inline void UpdateDim() {
        if(root_) {
            dim_ = root_->split->Size();
        }
        else dim_ = 0;
    }

    void Serialize(kdnode_ptr root, FILE* out) {
        KDTreeRec::KDNodeRec node;
        std::string uncompressed_buffer;
        size_t uncompressed_size;
        if(root) {
            Equip(node, true, root);
        }
        else {
            Equip(node, false, root);
        }
        node.SerializeToString(&uncompressed_buffer);
        uncompressed_size = uncompressed_buffer.size();
        fwrite(&uncompressed_size, 1, sizeof(uncompressed_size), out);
        fwrite(uncompressed_buffer.c_str(), 1, uncompressed_size, out);
        if(root) {
            Serialize(root->left, out);
            Serialize(root->right, out);
        }
    }

    kdnode_ptr Deserialize(FILE* in) {
        std::vector<char> uncompressed_buffer;
        size_t uncompressed_size;
        KDTreeRec::KDNodeRec node;
        fread(&uncompressed_size, 1, sizeof(uncompressed_size), in);
        uncompressed_buffer.resize(uncompressed_size);
        fread(uncompressed_buffer.data(), 1, uncompressed_size, in);
        node.ParseFromArray(uncompressed_buffer.data(), uncompressed_size);

        if(!node.isnode()) {
            return nullptr;
        }
        Point<T> point;
        point.SetIndex(node.index());
        for(int64_t i=0; i<node.point_size(); i++)
          point.Push(node.point(i));

        kdnode_ptr root = std::make_shared<kdnode<T>>(point, node.axis());
        root->left = Deserialize(in);
        root->right = Deserialize(in);
        return root;
    }

    kdnode_ptr _Build (std::vector<kdnode_ptr>& nodes, size_t depth) {
        if (nodes.empty()) {
            return kdnode_ptr();
        }

        size_t axis, position;
        CHECK(criterion_);
        criterion_->Set(nodes, depth);
        axis = criterion_->Axis();
        position = criterion_->Position();

        std::nth_element(nodes.begin(), nodes.begin() + position, nodes.end(), [&axis](kdnode_ptr a, kdnode_ptr b) {
                return (*a->split)[axis] < (*b->split)[axis];
            });
        kdnode_ptr node = nodes.at(position);
        node->axis = axis;

        std::vector<kdnode_ptr> left(nodes.begin(), nodes.begin() + position);
        std::vector<kdnode_ptr> right(nodes.begin() + position + 1, nodes.end());
        node->left = _Build(left, depth + 1);
        node->right = _Build(right, depth + 1);

        return node;
    }

    const Point<T>* _Nearest(const Point<T>& query) const {
        if(!root_) return nullptr;
        typedef std::pair<T, kdnode_ptr> DistNodeP;
        auto comp = [](DistNodeP& a, DistNodeP& b){
            return a.first > b.first;
        };
        std::priority_queue<DistNodeP,
            std::vector<DistNodeP>, decltype(comp)> pq(comp);
        best_estimate best(root_, std::numeric_limits<T>::max());
        pq.push(DistNodeP(0,root_));
        while(!pq.empty()) {
            const auto current = pq.top();
            if(current.first >= best.distance) return best.node->split.get();
            pq.pop();
            auto currentNode = current.second;
            T sqDist = SquaredEuclidean(query, *currentNode->split);
            T axisDist = AxisEuclidean(query, *currentNode->split, currentNode->axis);
            if(sqDist < best.distance) {
                best.node = currentNode;
                best.distance = sqDist;
            }
            kdnode_ptr near = (axisDist<=0) ? currentNode->left : currentNode->right;
            kdnode_ptr far = (axisDist<=0) ? currentNode->right : currentNode->left;
            if(near) pq.push(DistNodeP(0, near));
            if(far) pq.push(DistNodeP(axisDist* axisDist, far));
        }
        return best.node->split.get();
    }
}; //class KDTree

} //namespace kdtree

#endif //KDTREE_IMPL_H_
