/*!
 * Copyright (c) 2016 by Contributors
 * \file test_kdtree.h
 * \brief test the correctness of kdtree compared with the results of brutal force
 * \author Ziqi Liu
 */
#include "test_kdtree.h"
#include <gtest/gtest.h>


TEST(KDTreeTest, Correctness) {
    EXPECT_EQ(true, Correctness("cycle", "kdtree_model", 1000, 10));
    EXPECT_EQ(true, Correctness("", "kdtree_model", 1000, 1000));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
