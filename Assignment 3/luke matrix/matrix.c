#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <inttypes.h>

#include "matrix.h"

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
    
    for (ssize_t y = 0; y < g_height; y++) {
        for (ssize_t x = 0; x < g_width; x++) {
            printf("%" PRIu32 " ", matrix[y * g_width + x]);
        }
        
        printf("\n");
    }
}

/**
 * Displays given matrix row
 */
void display_row(const uint32_t* matrix, ssize_t row) {
    
    for (ssize_t i = 0; i < g_width; i++) {
        printf("%" PRIu32 " ", matrix[row * g_width + i]);
    }
    
    printf("\n");
}

/**
 * Displays given matrix column
 */
void display_column(const uint32_t* matrix, ssize_t column) {
    
    for (ssize_t i = 0; i < g_height; i++) {
        printf("%" PRIu32 "\n", matrix[i * g_width + column]);
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
    
    for (ssize_t i = 0; i < g_width; i++) {
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
    
    for (ssize_t i = 0; i < g_elements; i++) {
        matrix[i] = fast_rand();
    }
    
    return matrix;
}

/**
 * Returns new matrix with all elements set to given value
 */
uint32_t* uniform_matrix(uint32_t value) {
    
    uint32_t* matrix = new_matrix();
    
    for (ssize_t i = 0; i < g_elements; i++) {
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
    
    for (ssize_t i = 0; i < g_elements; i++) {
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
    
    for (ssize_t i = 0; i < g_elements; i++) {
        result[i] = matrix[i];
    }
    
    return result;
}

/**
 * Returns new matrix with elements in ascending order
 */
uint32_t* sorted(const uint32_t* matrix) {
    
    uint32_t* result = cloned(matrix);
    
    /*
     to do
     
     1 0    0 0
     0 1 => 1 1
     
     4 3    1 2
     2 1 => 3 4
     */
    
    return result;
}

/**
 * Returns new matrix with elements rotated 90 degrees clockwise
 */
uint32_t* rotated(const uint32_t* matrix) {
    
    uint32_t* result = new_matrix();
    
    for (ssize_t y = 0; y < g_height; y++) {
        for (ssize_t x = 0; x < g_width; x++) {
            result[x * g_height + (g_height - y - 1)] = matrix[y * g_width + x];
        }
    }
    
    return result;
}

/**
 * Returns new matrix with elements ordered in reverse
 */
uint32_t* reversed(const uint32_t* matrix) {
    
    uint32_t* result = new_matrix();
    
    for (ssize_t i = 0; i < g_elements; i++) {
        result[i] = matrix[g_elements - 1 - i];
    }
    
    return result;
}

/**
 * Returns new transposed matrix
 */
uint32_t* transposed(const uint32_t* matrix) {
    
    uint32_t* result = new_matrix();
    
    for (ssize_t y = 0; y < g_height; y++) {
        for (ssize_t x = 0; x < g_width; x++) {
            result[x * g_width + y] = matrix[y * g_width + x];
        }
    }
    
    return result;
}

/**
 * Returns new matrix, adding scalar to each element
 */
uint32_t* scalar_add(const uint32_t* matrix, uint32_t scalar) {
    
    uint32_t* result = new_matrix();
    
    for(ssize_t i = 0; i < g_elements; ++i) {
        result[i] = matrix[i] + scalar;
    }
    /*
     to do
     
     1 0        2 1
     0 1 + 1 => 1 2
     
     1 2        5 6
     3 4 + 4 => 7 8
     */
    
    return result;
}

/**
 * Returns new matrix, multiplying scalar to each element
 */
uint32_t* scalar_mul(const uint32_t* matrix, uint32_t scalar) {
    
    uint32_t* result = new_matrix();
    
    for(ssize_t i = 0; i < g_elements; ++i) {
        result[i] = matrix[i] * scalar;
    }
    /*
     to do
     
     1 0        2 0
     0 1 x 2 => 0 2
     
     1 2        2 4
     3 4 x 2 => 6 8
     */
    
    return result;
}

/**
 * Returns new matrix, adding elements with the same index
 */
uint32_t* matrix_add(const uint32_t* matrix_a, const uint32_t* matrix_b) {
    
    uint32_t* result = new_matrix();
    
    for(ssize_t i = 0; i < g_elements; ++i) {
        result[i] = matrix_a[i] + matrix_b[i];
    }
    /*
     to do
     
     1 0   0 1    1 1
     0 1 + 1 0 => 1 1
     
     1 2   4 4    5 6
     3 4 + 4 4 => 7 8
     */
    
    return result;
}

/**
 * Returns new matrix, multiplying the two matrices together
 */
uint32_t* matrix_mul(const uint32_t* matrix_a, const uint32_t* matrix_b) {
    
    uint32_t* result = new_matrix();
    
    for(ssize_t i = 0; i < g_height; ++i) {
        for(ssize_t j = 0; j < g_width; ++j) {
            for(ssize_t k = 0; k < g_width; ++k) {
                result[i*g_height+j] += matrix_a[i*g_height+k] * matrix_b[j+g_height*k];
            }
        }
    }
    /*
     to do
     
     1 2   1 0    1 2
     3 4 x 0 1 => 3 4
     
     1 2   5 6    19 22
     3 4 x 7 8 => 43 50
     */
    
    return result;
}

/**
 * Returns new matrix, powering the matrix to the exponent
 */
uint32_t* matrix_pow(const uint32_t* matrix, uint32_t exponent) {
    
    uint32_t* result = new_matrix();
    uint32_t* interim = new_matrix();
    if(exponent == 0) {
        return identity_matrix();
    }
    for (ssize_t i = 0; i < g_elements; ++i) {
        interim[i] = matrix[i];
    }
    if(exponent == 1) {
        return interim;
    }
    for (ssize_t e = 0; e < exponent - 1; ++e) {
        for(ssize_t i = 0; i < g_elements; ++i) {
            result[i] = 0;
        }
        for(ssize_t i = 0; i < g_height; ++i) {
            for(ssize_t j = 0; j < g_width; ++j) {
                for(ssize_t k = 0; k < g_width; ++k) {
                    result[i*g_height+j] += interim[i*g_height+k] * matrix[j+g_height*k];
                }
            }
        }
        for(ssize_t i = 0; i < g_elements; ++i) {
            interim[i] = result[i];
        }
    }
    /*
     to do
     
     1 2        1 0
     3 4 ^ 0 => 0 1
     
     1 2        199 290
     3 4 ^ 4 => 435 634
     */
    
    return result;
}

////////////////////////////////
///       COMPUTATIONS       ///
////////////////////////////////

/**
 * Returns the sum of all elements
 */
uint32_t get_sum(const uint32_t* matrix) {
    
    uint32_t sum = 0;
    for(ssize_t i = 0; i < g_elements; ++i) {
        sum += matrix[i];
    }
    /*
     to do
     
     1 2
     2 1 => 6
     
     1 1
     1 1 => 4
     */
    
    return sum;
}

/**
 * Returns the most frequently occuring value
 * or UINT32_MAX when there is a tie
 */
uint32_t get_mode(const uint32_t* matrix) {
    
    /*
     to do
     
     1 2
     2 2 => 2
     
     1 2
     3 4 => UINT32_MAX
     */
    
    return 0;
}

/**
 * Returns the trace of the matrix
 */
uint32_t get_trace(const uint32_t* matrix) {
    
    uint32_t trace = 0;
    for (ssize_t i = 0; i < g_width; ++i) {
        trace += matrix[i*g_width+i];
    }
    /*
     to do
     
     1 0
     0 1 => 2
     
     2 1
     1 2 => 4
     */
    
    return trace;
}

/**
 * Returns the upper median
 */
uint32_t get_median(const uint32_t* matrix) {
    
    /*
     to do
     
     1 0
     0 1 => 1
     
     1 1
     2 2 => 2
     */
    
    return 0;
}

/**
 * Returns the smallest value in the matrix
 */
uint32_t get_minimum(const uint32_t* matrix) {
    
    uint32_t min = UINT32_MAX;
    for(ssize_t i = 0; i < g_elements; ++i) {
        if (matrix[i] < min) {
            min = matrix[i];
        }
    }
    /*
     to do
     
     1 2
     3 4 => 1
     
     4 3
     2 1 => 1
     */
    
    return min;
}

/**
 * Returns the largest value in the matrix
 */
uint32_t get_maximum(const uint32_t* matrix) {
    
    uint32_t max = 0;
    for(ssize_t i = 0; i < g_elements; ++i) {
        if (matrix[i] > max) {
            max = matrix[i];
        }
    }
    /*
     to do
     
     1 2
     3 4 => 4
     
     4 3
     2 1 => 4
     */
    
    return max;
}

/**
 * Returns the frequency of the value in the matrix
 */
uint32_t get_frequency(const uint32_t* matrix, uint32_t value) {
    
    uint32_t freq = 0;
    for(ssize_t i = 0; i < g_elements; ++i) {
        if(matrix[i] == value) {
            freq++;
        }
    }
    /*
     to do
     
     1 1
     1 1 :: 1 => 4
     
     1 0
     0 1 :: 2 => 0
     */
    
    return freq;
}
