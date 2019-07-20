//
// Created by tkhamvilai on 7/16/19.
//

#include "MathHelperFunctions.hpp"

bool MathHelperFunctions_isSubset(std::vector<int> arr1, std::vector<int> arr2)
{
    int i = 0, j = 0;
    int m = arr1.size();
    int n = arr2.size();

    if (m < n)
        return 0;

    sort(arr1.begin(), arr1.end());
    sort(arr2.begin(), arr2.end());

    while (i < n && j < m )
    {
        if( arr1[j] <arr2[i] )
            j++;
        else if( arr1[j] == arr2[i] )
        {
            j++;
            i++;
        }

        else if( arr1[j] > arr2[i] )
            return 0;
    }

    return  (i < n)? false : true;
}

int MathHelperFunctions_norm_of_difference(int i, int j, Eigen::MatrixXi mat) // computes the norm (can be any one) of [ nodes_on_CRs_received(:, i) - nodes_on_CRs_received(:, j) ]
{
    int norm = 0, norm_i = 0, norm_j = 0;
    for(int k = 0 ; k < mat.rows(); k++)
    {
        norm += ( mat(k, i-1) - mat(k, j-1) ) * ( mat(k, i-1) - mat(k, j-1) );
        // here, the result of 'norm' will be the 2-normed squared (ensures integer result and avoid using other libraries)
        norm_i += ( mat(k, i-1) ) * ( mat(k, i-1) );
        norm_j += ( mat(k, j-1) ) * ( mat(k, j-1) );
    }
    if (norm_i == 0 && norm_j == 0)
    {
        return -1;
    }
    else
    {
        return norm;
    }
}

int MathHelperFunctions_Intersection(std::vector<int> arr1, std::vector<int> arr2)
{
    int i = 0, j = 0, count = 0;
    int m = arr1.size();
    int n = arr2.size();

    sort(arr1.begin(), arr1.end());
    sort(arr2.begin(), arr2.end());

    while (i < m && j < n)
    {
        if (arr1[i] < arr2[j])
            i++;
        else if (arr2[j] < arr1[i])
            j++;
        else /* if arr1[i] == arr2[j] */
        {
            count++;
            i++;
            j++;
        }
    }
    return count;
}