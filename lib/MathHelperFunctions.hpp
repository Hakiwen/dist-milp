//
// Created by tkhamvilai on 7/16/19.
//

#ifndef DIST_MILP_MATHHELPERFUNCTIONS_HPP
#define DIST_MILP_MATHHELPERFUNCTIONS_HPP

#include <vector>
#include <algorithm>
#include <Eigen/Dense>

bool MathHelperFunctions_isSubset(std::vector<int> arr1, std::vector<int> arr2);
int MathHelperFunctions_norm_of_difference(int i, int j, Eigen::MatrixXi mat);
int MathHelperFunctions_Intersection(std::vector<int> arr1, std::vector<int> arr2);

#endif //DIST_MILP_MATHHELPERFUNCTIONS_HPP
