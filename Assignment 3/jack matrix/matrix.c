#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <inttypes.h>

#include "matrix.h"

#define CHUNK ((g_elements)/g_nthreads)

static uint32_t g_seed = 0;

static ssize_t g_width = 0;
static ssize_t g_height = 0;
static ssize_t g_elements = 0;

static ssize_t g_nthreads = 1;

////////////////////////////////
///     UTILITY FUNCTIONS    ///
////////////////////////////////

/**
 * Returns pseudorandom number determined by the seed
 */
uint32_t fast_rand(void) {

    g_seed = (214013 * g_seed + 2531011);
    return (g_seed >> 16) & 0x7FFF;
}

/**
 * Sets the seed used when generating pseudorandom numbers
 */
void set_seed(uint32_t seed) {

    g_seed = seed;
}

/**
 * Sets the number of threads available
 */
void set_nthreads(ssize_t count) {

    g_nthreads = count;
}

/**
 * Sets the dimensions of the matrix
 */
void set_dimensions(ssize_t order) {

    g_width = order;
    g_height = order;

    g_elements = g_width * g_height;
}

/**
 * Displays given matrix
 */
void display(const uint32_t* matrix) {

    for (ssize_t y = 0; y < g_height; ++y) {
        for (ssize_t x = 0; x < g_width; ++x) {
            if (x > 0) printf(" ");
            printf("%" PRIu32, matrix[y * g_width + x]);
        }

        printf("\n");
    }
}

/**
 * Displays given matrix row
 */
void display_row(const uint32_t* matrix, ssize_t row) {

    for (ssize_t x = 0; x < g_width; ++x) {
        if (x > 0) printf(" ");
        printf("%" PRIu32, matrix[row * g_width + x]);
    }

    printf("\n");
}

/**
 * Displays given matrix column
 */
void display_column(const uint32_t* matrix, ssize_t column) {

    for (ssize_t y = 0; y < g_height; ++y) {
        printf("%" PRIu32 "\n", matrix[y * g_width + column]);
    }
}

/**
 * Displays the value stored at the given element index
 */
void display_element(const uint32_t* matrix, ssize_t row, ssize_t column) {

    printf("%" PRIu32 "\n", matrix[row * g_width + column]);
}

////////////////////////////////
///   MATRIX INITALISATIONS  ///
////////////////////////////////

/**
 * Returns new matrix with all elements set to zero
 */
uint32_t* new_matrix(void) {

    return calloc(g_elements, sizeof(uint32_t));
}

/**
 * Returns new identity matrix
 */
uint32_t* identity_matrix(void) {

    uint32_t* matrix = new_matrix();

    for (ssize_t i = 0; i < g_width; ++i) {
        matrix[i * g_width + i] = 1;
    }

    return matrix;
}

/**
 * Returns new matrix with elements generated at random using given seed
 */
uint32_t* random_matrix(uint32_t seed) {

    uint32_t* matrix = new_matrix();

    set_seed(seed);

    for (ssize_t i = 0; i < g_elements; ++i) {
        matrix[i] = fast_rand();
    }

    return matrix;
}

/**
 * Returns new matrix with all elements set to given value
 */
uint32_t* uniform_matrix(uint32_t value) {

    uint32_t* matrix = new_matrix();

    for (ssize_t i = 0; i < g_elements; ++i) {
        matrix[i] = value;
    }

    return matrix;
}

/**
 * Returns new matrix with elements in sequence from given start and step
 */
uint32_t* sequence_matrix(uint32_t start, uint32_t step) {

    uint32_t* matrix = new_matrix();
    uint32_t current = start;

    for (ssize_t i = 0; i < g_elements; ++i) {
        matrix[i] = current;
        current += step;
    }

    return matrix;
}

////////////////////////////////
///     MATRIX OPERATIONS    ///
////////////////////////////////

/**
 * Returns new matrix with elements cloned from given matrix
 */

uint32_t* cloned(const uint32_t* matrix) {

    uint32_t* result = new_matrix();
	for (uint32_t i = 0; i < g_elements; ++i) {
		result[i] = matrix[i];
	}
	
    return result;
}

/**
 * Returns new matrix with elements ordered in reverse
 */
uint32_t* reversed(const uint32_t* matrix) {

    uint32_t* result = new_matrix();

    for (ssize_t i = 0; i < g_elements; ++i) {
        result[i] = matrix[g_elements - 1 - i];
    }

    return result;
}

/**
 * Returns new transposed matrix
 */

uint32_t* transposed(const uint32_t* matrix) {

    uint32_t* result = new_matrix();

	for (uint32_t i = 0; i < g_elements; ++i) {
		result[i] = matrix[(i%g_width)*g_width + ((i - i%g_width) / g_width)];
	}

    return result;
}

struct sca_add_argument {
	const uint32_t *matrix;
	uint32_t *new_matrix;
	size_t thread_id;
	uint32_t value;
};

static void *sca_add_worker(void *arg) {
	struct sca_add_argument *argument = (struct sca_add_argument *)arg;

	uint32_t start = argument->thread_id * CHUNK;
	uint32_t end = argument->thread_id == g_nthreads - 1 ? g_elements : (argument->thread_id + 1) * CHUNK;

	for (uint32_t i = start; i < end; ++i) {
		argument->new_matrix[i] = argument->matrix[i]+argument->value;
	}

	return NULL;
}


/**
 * Returns new matrix with scalar added to each element
 */
uint32_t* scalar_add(const uint32_t* matrix, uint32_t scalar) {

    uint32_t* result = new_matrix();

    /*
        to do

        1 0        2 1
        0 1 + 1 => 1 2

        1 2        5 6
        3 4 + 4 => 7 8
    */

//	for (ssize_t i = 0; i < g_width*g_height; ++i) {
//		result[i] = matrix[i]+scalar;
//	}

//    return result;
	struct sca_add_argument args[g_nthreads];// = malloc(sizeof(struct sca_add_argument)*g_nthreads);

	for (uint32_t i = 0; i < g_nthreads; ++i) {
		args[i] = (struct sca_add_argument) {
			.matrix = matrix,
				.new_matrix = result,
				.thread_id = i,
				.value = scalar
		};
	}

	pthread_t thread_ids[g_nthreads];

	//initialize threads
	for (uint32_t i = 0; i < g_nthreads; ++i) {
		pthread_create(thread_ids + i, NULL, sca_add_worker, args + i);
	}

	// join all the threads
	for (uint32_t i = 0; i < g_nthreads; ++i) {
		pthread_join(thread_ids[i], NULL);
	}

	

	//free(args);
	return result;

}

/**
 * Returns new matrix with scalar multiplied to each element
 */

struct sca_mult_argument {
	const uint32_t *matrix; //8
	uint32_t *new_matrix; //8
	size_t thread_id; //8
	uint32_t value; //4
	char padding[36];
};

static void *sca_mult_worker(void *arg) {
	struct sca_mult_argument *argument = (struct sca_mult_argument *)arg;

	uint32_t start = argument->thread_id * CHUNK;
	uint32_t end = argument->thread_id == g_nthreads - 1 ? g_elements : (argument->thread_id + 1) * CHUNK;

	for (uint32_t i = start; i < end; ++i) {
		argument->new_matrix[i] = argument->matrix[i] * argument->value;
	}

	return NULL;
}


uint32_t* scalar_mul(const uint32_t* matrix, uint32_t scalar) {

    uint32_t* result = new_matrix();

    /*
        to do

        1 0        2 0
        0 1 x 2 => 0 2

        1 2        2 4
        3 4 x 2 => 6 8
    */
//	for (ssize_t i = 0; i < g_width*g_height; ++i) {
//		result[i] = matrix[i]*scalar;
//	}

//    return result;
	struct sca_mult_argument args[g_nthreads];// = malloc(sizeof(struct sca_mult_argument)*g_nthreads);

	for (uint32_t i = 0; i < g_nthreads; ++i) {
		args[i] = (struct sca_mult_argument) {
			.matrix = matrix,
				.new_matrix = result,
				.thread_id = i,
				.value = scalar
		};
	}

	pthread_t thread_ids[g_nthreads];

	//initialize threads
	for (uint32_t i = 0; i < g_nthreads; ++i) {
		pthread_create(thread_ids + i, NULL, sca_mult_worker, args + i);
	}

	// join all the threads
	for (uint32_t i = 0; i < g_nthreads; ++i) {
		pthread_join(thread_ids[i], NULL);
	}



	//free(args);
	return result;

}

/**
 * Returns new matrix with elements added at the same index
 */
uint32_t* matrix_add(const uint32_t* matrix_a, const uint32_t* matrix_b) {

    uint32_t* result = new_matrix();

    /*
        to do

        1 0   0 1    1 1
        0 1 + 1 0 => 1 1

        1 2   4 4    5 6
        3 4 + 4 4 => 7 8
    */

	for (ssize_t i = 0; i < g_width*g_height; ++i) {
		result[i] = matrix_a[i] + matrix_b[i];
	}

    return result;
}

/**
 * Returns new matrix, multiplying the two matrices together
 */

struct mult_argument {
	const uint32_t *matrix_a;
	const uint32_t *matrix_b;
	uint32_t *new_matrix;
	size_t thread_id;
	char pad[32];
};

static void *mult_worker(void *arg) {
	struct mult_argument *argument = (struct mult_argument *)arg;

	size_t start = argument->thread_id * CHUNK;
	size_t end = argument->thread_id == g_nthreads - 1 ? g_elements : (argument->thread_id + 1) * CHUNK;

	uint32_t col,row,sum_rc;
	for (size_t i = start; i < end; ++i) {
		sum_rc = 0;
		col = i%g_width*g_width;
		row = i - i%g_width;
		for (ssize_t j = 0; j < g_width; ++j){
			sum_rc += argument->matrix_a[row + j] * argument->matrix_b[col+j];
		}

		argument->new_matrix[i] = sum_rc;
	}

	return NULL;
}


uint32_t* matrix_mul(const uint32_t* matrix_a, const uint32_t* matrix_b) {

    uint32_t* result = new_matrix();
	uint32_t* trans_b = transposed(matrix_b);

    /*
        to do

        1 2   1 0    1 2
        3 4 x 0 1 => 3 4

        1 2   5 6    19 22
        3 4 x 7 8 => 43 50
    */

	//for (ssize_t i = 0; i < g_height; ++i){
	//	for (ssize_t j = 0; j < g_width; ++j){
//			for (ssize_t k = 0; k < g_width; ++k){
//				result[i*g_height + j] += matrix_a[i*g_height + k] * trans_b[j*g_height + k];
//			}
//
//		}
//	}

//    return result;

	struct mult_argument args[g_nthreads];// = malloc(sizeof(struct mult_argument)*g_nthreads);

	for (size_t i = 0; i < g_nthreads; ++i) {
		args[i] = (struct mult_argument) {
			.matrix_a = matrix_a,
				.matrix_b = trans_b,
				.new_matrix = result,
				.thread_id = i
		};
	}

	pthread_t thread_ids[g_nthreads];

	//initialize threads
	for (ssize_t i = 0; i < g_nthreads; ++i) {
		pthread_create(thread_ids + i, NULL, mult_worker, args + i);
	}

	// join all the threads
	for (size_t i = 0; i < g_nthreads; ++i) {
		pthread_join(thread_ids[i], NULL);
	}



	//free(args);
	return result;
}

/**
 * Returns new matrix, powering the matrix to the exponent
 */

uint32_t* matrix_pow(const uint32_t* matrix, uint32_t exponent) {

    uint32_t* result = new_matrix();

    /*
        to do

        1 2        1 0
        3 4 ^ 0 => 0 1

        1 2        1 2
        3 4 ^ 1 => 3 4

        1 2        199 290
        3 4 ^ 4 => 435 634
    */
	if (exponent == 0){
		return identity_matrix();
	}
	result = cloned(matrix);

	while (exponent > 1){
		if (exponent % 2 == 0){
			result = matrix_mul(result, result);
			exponent /= 2;
		}
		else{
			result = matrix_mul(result, matrix_mul(result, result));
			exponent = (exponent - 1) / 2;
		}
	}
	
	//result = square_mult(result, exponent);

    return result;
}

////////////////////////////////
///       COMPUTATIONS       ///
////////////////////////////////

/**
 * Returns the sum of all elements
 */

struct sum_argument {
	const uint32_t *matrix;
	size_t thread_id;
	uint32_t results;
};

static void *sum_worker(void *arg) {
	struct sum_argument *argument = (struct sum_argument *)arg;

	size_t start = argument->thread_id * CHUNK;
	size_t end = argument->thread_id == g_nthreads - 1 ? g_elements : (argument->thread_id + 1) * CHUNK;

	uint32_t sum = 0;
	for (size_t i = start; i < end; ++i) {
		sum += argument->matrix[i];
	}
	argument->results = sum;

	return NULL;
}

uint32_t get_sum(const uint32_t* matrix) {

    /*
        to do

        1 2
        2 1 => 6

        1 1
        1 1 => 4
    */
//	uint32_t sum = 0;
//	for (int i = 0; i < g_elements; ++i){
//		sum = sum + matrix[i];
//	}
 //   return sum;
	struct sum_argument args[g_nthreads];// = malloc(sizeof(struct sum_argument)*g_nthreads);

	uint32_t final_result = 0;

	for (size_t i = 0; i < g_nthreads; ++i) {
		args[i] = (struct sum_argument) {
			.matrix = matrix,
				.thread_id = i,
				.results = 0
		};
	}

	pthread_t thread_ids[g_nthreads];

	//initialize threads
	for (ssize_t i = 0; i < g_nthreads; ++i) {
		pthread_create(thread_ids + i, NULL, sum_worker, args + i);
	}

	// join all the threads
	for (size_t i = 0; i < g_nthreads; ++i) {
		pthread_join(thread_ids[i], NULL);
	}

	// add all the results
	for (size_t i = 0; i < g_nthreads; ++i) {
		final_result += args[i].results;
	}

	//free(args);
	return final_result;
}



/**
 * Returns the trace of the matrix
 */
uint32_t get_trace(const uint32_t* matrix) {//each thread will have a calculable amount of the trace
	
    /*
        to do

        1 0
        0 1 => 2

        2 1
        1 2 => 4
    */
	uint32_t sum = 0;
	for(uint32_t i = 0; i < (g_width/2); ++i){
		sum += matrix[i*g_width + i] + matrix[(g_width - i)*(g_height)-i-1];
	}
	if (g_height % 2 == 1){
		sum += matrix[(g_height / 2 + 1)*(g_width / 2 + 1)];
	}

    return sum;

	
}

/**
 * Returns the smallest value in the matrix
 */


struct min_argument {
	const uint32_t *matrix;
	size_t thread_id;
	uint32_t min;
	char padding[44];
};

static void *min_worker(void *arg) {
	struct min_argument *argument = (struct min_argument *)arg;

	size_t start = argument->thread_id * CHUNK;
	size_t end = argument->thread_id == g_nthreads - 1 ? g_elements : (argument->thread_id + 1) * CHUNK;

	for (size_t i = start; i < end; ++i) {
		if (argument->matrix[i] < argument->min){
			argument->min = argument->matrix[i];
		}
	}

	return NULL;
}


uint32_t get_minimum(const uint32_t* matrix) {

	struct min_argument args[g_nthreads];// = malloc(sizeof(struct min_argument)*g_nthreads);

	

	for (size_t i = 0; i < g_nthreads; ++i) {
		args[i] = (struct min_argument) {
			.matrix = matrix,
				.thread_id = i,
				.min = UINT32_MAX
		};
	}

	pthread_t thread_ids[g_nthreads];

	//initialize threads
	for (ssize_t i = 0; i < g_nthreads; ++i) {
		pthread_create(thread_ids + i, NULL, min_worker, args + i);
	}

	// join all the threads
	for (size_t i = 0; i < g_nthreads; ++i) {
		pthread_join(thread_ids[i], NULL);
	}

	uint32_t final_min = UINT32_MAX;

	// add all the results
	for (size_t i = 0; i < g_nthreads; ++i) {

		final_min = args[i].min<final_min ? args[i].min : final_min;
	}

	//free(args);
	return final_min;
}

/**
 * Returns the largest value in the matrix
 */

struct max_argument {
	const uint32_t *matrix;
	size_t thread_id;
	uint32_t max;
	char padding[44];
};

static void *max_worker(void *arg) {
	struct max_argument *argument = (struct max_argument *)arg;

	size_t start = argument->thread_id * CHUNK;
	size_t end = argument->thread_id == g_nthreads - 1 ? g_elements : (argument->thread_id + 1) * CHUNK;
	
	for (size_t i = start; i < end; ++i) {
		if (argument->matrix[i] > argument->max){
			argument->max = argument->matrix[i];
		}
	}

	return NULL;
}


uint32_t get_maximum(const uint32_t* matrix) {

	struct max_argument args[g_nthreads];// = malloc(sizeof(struct max_argument)*g_nthreads);

	uint32_t final_max = 0;

	for (size_t i = 0; i < g_nthreads; ++i) {
		args[i] = (struct max_argument) {
			.matrix = matrix,
				.thread_id = i,
				.max = 0
		};
	}

	pthread_t thread_ids[g_nthreads];

	//initialize threads
	for (ssize_t i = 0; i < g_nthreads; ++i) {
		pthread_create(thread_ids + i, NULL, max_worker, args + i);
	}

	// join all the threads
	for (size_t i = 0; i < g_nthreads; ++i) {
		pthread_join(thread_ids[i], NULL);
	}

	// add all the results
	for (size_t i = 0; i < g_nthreads; ++i) {

		final_max = args[i].max>final_max ? args[i].max : final_max;
	}

	//free(args);
	return final_max;

}

/**
 * Returns the frequency of the value in the matrix
 */

struct freq_argument {
	const uint32_t *matrix;
	size_t thread_id;
	uint32_t value;
	uint32_t results;
};

static void *freq_worker(void *arg) {
	struct freq_argument *argument = (struct freq_argument *)arg;

	size_t start = argument->thread_id * CHUNK;
	size_t end = argument->thread_id == g_nthreads - 1 ? g_elements : (argument->thread_id + 1) * CHUNK;
	
	argument->results = 0;
	for (size_t i = start; i < end; ++i) {
		if (argument->matrix[i] == argument->value){
			argument->results++;
		}
	}

	return NULL;
}

uint32_t get_frequency(const uint32_t* matrix, uint32_t value) {

	/*
		to do

		1 1
		1 1 :: 1 => 4

		1 0
		0 1 :: 2 => 0
		*/

	struct freq_argument args[g_nthreads];// = malloc(sizeof(struct freq_argument)*g_nthreads);

	uint32_t final_result = 0;

	for (size_t i = 0; i < g_nthreads; ++i) {
		args[i] = (struct freq_argument) {
			.matrix = matrix,
				.thread_id = i,
				.value=value,
				.results = 0
		};
	}

	pthread_t thread_ids[g_nthreads];

	//initialize threads
	for (ssize_t i = 0; i < g_nthreads; ++i) {
		pthread_create(thread_ids + i, NULL, freq_worker, args + i);
	}

	// join all the threads
	for (size_t i = 0; i < g_nthreads; ++i) {
		pthread_join(thread_ids[i], NULL);
	}

	// add all the results
	for (size_t i = 0; i < g_nthreads; ++i) {
		final_result += args[i].results;
	}

	//free(args);
	return final_result;

}
