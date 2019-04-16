//#include "tmr.h"
#include <stdlib.h>
#include <stdio.h>

const int TOLERANCE = 5;
const int ARR_LEN = 3;

// Careful for the types, depending of the output of the controller

////////////////////////////////////////////////////////////////////////

/**
 * Detects faults among the three copies, outputs the number (1, 2 or 3)
 * of the faulty element if the 2 other match, 0 if all of them match, 6
 * if none matches.
 * Also used to compute voter_mean.
 */
int error_detector(int* array){
	int v1 = array[0];
	int v2 = array[1];
	int v3 = array[2];
	//*Returns the number corresponding to the wrong value among three ones, 0 if they all match and 6 if they all mismatch
	/*get the values from the three redundant applications
	*/

	int mismatch12 = ( abs(v1 - v2) > TOLERANCE );
	int mismatch13 = ( abs(v1 - v3) > TOLERANCE );
	int mismatch23 = ( abs(v2 - v3) > TOLERANCE );
	
	return 1*mismatch12*mismatch13 + 2*mismatch12*mismatch23 + 3*mismatch13*mismatch23;
}


/**
 * Mean of the matching values, 0 if no value match. 
 * May genereate a float output from integer inputs.
 */
double voter_mean(int* array, int err_detector_result){
	if (err_detector_result == 6){
		return 0;
	}
	else {
		double sum = 0;
		for(int i = 0 ; i < ARR_LEN ; i++){
			sum = sum + ((double) array[i])*(err_detector_result != i+1) / ( 3*(err_detector_result == 0) + 2*(err_detector_result != 0) );
		}
		return sum;
	}
}

////////////////////////////////////////////////////////////////////////

int compare (const void * a, const void * b){
	return ( *(int*)a - *(int*)b );
}

/**
 * Computes the median of the three results.
 * Garanties an integer output
 */
int voter_median(int* array){
	int copy[] = {array[0], array[1], array[2]};
	qsort(copy, ARR_LEN, sizeof(int), compare);
	return copy[1];
}

////////////////////////////////////////////////////////////////////////

int main(){
	int array[] = {2, 1, 111};
	
	printf("[ %d, %d, %d ]\n", array[0], array[1], array[2]);
	double vote = voter_mean(array, error_detector(array));
	printf("Error detector: %d\n", error_detector(array));
	printf("Mean voter: %f\n", vote);
	printf("Median voter: %d\n", voter_median(array));
	return 0;
}
