#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <inttypes.h>

#include "emmintrin.h"
#include "immintrin.h"
#include "matrix.h"

#define CHUNK (g_elements/g_nthreads)

static uint32_t g_seed = 0;

static ssize_t g_width = 0;
static ssize_t g_height = 0;
static ssize_t g_elements = 0;

static ssize_t g_nthreads = 1;

//static pthread_mutex_t first_mutex = PTHREAD_MUTEX_INITIALIZER;

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
    
    for (ssize_t i = 0; i < g_elements; ++i) {
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
    
    for (ssize_t y = 0; y < g_height; y++) {
        for (ssize_t x = 0; x < g_width; x++) {
            result[x * g_width + y] = matrix[y * g_width + x];
        }
    }
    
    return result;
}

struct sthread_arg {
    int no;
    int scalar;
    const uint32_t* matrix;
    uint32_t* result;
};

void * add_fn(void *p) {
    struct sthread_arg *ptr = (struct sthread_arg *) p;
    int start = (ptr->no)*CHUNK;
    //set the end of the last thread to the final element.
    int end = 
         ptr->no == g_nthreads -1 ? g_elements : ((ptr->no)+1)*CHUNK;
    for(int i = start; i< end; ++i){
        ptr->result[i] = ptr->matrix[i] + ptr->scalar;
    }
    pthread_exit(NULL);
}

/**
 * Returns new matrix, adding scalar to each element
 */
uint32_t* scalar_add(const uint32_t* matrix, uint32_t scalar) {
    
    uint32_t* result = new_matrix();

    pthread_t threads[g_nthreads];
    struct sthread_arg ta[g_nthreads];
    //initialise and create threads from the last to the first.
    for(ssize_t i = g_nthreads-1; i > -1; --i) {
        ta[i] = (struct sthread_arg) {
            .no = i,
            .scalar = scalar,
            .matrix = matrix,
            .result = result,
        };
        pthread_create(&threads[i],NULL, add_fn, &ta[i]);
    }
    for(ssize_t i = 0; i < g_nthreads; ++i) {
        pthread_join(threads[i], NULL);
    }
    
    return result;
}

void * mult_fn(void *p) {
    struct sthread_arg *ptr = (struct sthread_arg *) p;
    int start = (ptr->no)*CHUNK;
    int end = 
         ptr->no == g_nthreads -1 ? g_elements : ((ptr->no)+1)*CHUNK;
    for(int i = start; i< end; ++i){
        ptr->result[i] = ptr->matrix[i] * ptr->scalar;
    }
    pthread_exit(NULL);
}

/**
 * Returns new matrix, multiplying scalar to each element
 */
uint32_t* scalar_mul(const uint32_t* matrix, uint32_t scalar) {

    uint32_t* result = new_matrix();

    pthread_t threads[g_nthreads];
    struct sthread_arg ta[g_nthreads];

    for(ssize_t i = g_nthreads-1; i > -1; --i) {
        ta[i] = (struct sthread_arg) {
            .no = i,
            .scalar = scalar,
            .matrix = matrix,
            .result = result,
        };
        pthread_create(&threads[i],NULL, mult_fn, &ta[i]);
    }
    for(ssize_t i = 0; i < g_nthreads; ++i) {
        pthread_join(threads[i], NULL);
    }
    
    return result;
}

struct mthread_arg {
    int no;
    const uint32_t* mat1;
    const uint32_t* mat2;
    uint32_t* result;
}; 

void * madd_fn(void *p) {
    struct mthread_arg *ptr = (struct mthread_arg *) p;
    int start = (ptr->no)*CHUNK;
    int end = 
         ptr->no == g_nthreads -1 ? g_elements : ((ptr->no)+1)*CHUNK;
    for (ssize_t index = start; index < end; ++index) {
        ptr->result[index] = ptr->mat1[index] + ptr->mat2[index];
    }
    pthread_exit(NULL);
}

/**
 * Returns new matrix, adding elements with the same index
 */
uint32_t* matrix_add(const uint32_t* matrix_a, const uint32_t* matrix_b) {
    
    uint32_t* result = new_matrix();

    pthread_t threads[g_nthreads];
    struct mthread_arg ta[g_nthreads];
   
    for(ssize_t i = g_nthreads-1; i > -1; --i) {
        ta[i] = (struct mthread_arg) {
            .no = i,
            .mat1 = matrix_a,
            .mat2 = matrix_b,
            .result = result,
        };
        pthread_create(&threads[i],NULL, madd_fn, &ta[i]);
    }
    for(ssize_t i = 0; i < g_nthreads; ++i) {
        pthread_join(threads[i], NULL);
    }
    
    return result;
}

void * mmult_fn(void *p) {
    struct mthread_arg *ptr = (struct mthread_arg *) p;
    ssize_t start = (ptr->no)*CHUNK;
    ssize_t end = 
        ptr->no == g_nthreads -1 ? g_elements : ((ptr->no)+1)*CHUNK;
    ssize_t col_t, row;
    for (ssize_t index = start; index < end; ++index) {
        //use a tmp variable to reduce false sharing.
        uint32_t tmp = 0;
        //store the column and row being multiplied this iteration.
        col_t = (index % g_width) * g_height;
        row = (index/g_height) * g_width;
        for(ssize_t k = 0; k < g_width; ++k) {
            tmp += ptr->mat1[row+k] * ptr->mat2[col_t+k];
        }
        ptr->result[index] = tmp;
    }
    pthread_exit(NULL);
}

/**
 * Returns new matrix, multiplying the two matrices together
 */
uint32_t* matrix_mul(const uint32_t* matrix_a, const uint32_t* matrix_b) {

    uint32_t* result = new_matrix();
    //transpose matrix b to maximise cache hits.
    uint32_t* trans = transposed(matrix_b);

    pthread_t threads[g_nthreads];
    struct mthread_arg *ta = malloc(sizeof(struct mthread_arg) * g_nthreads);
    
    for(ssize_t i = g_nthreads-1; i > -1; --i) {
        ta[i] = (struct mthread_arg) {
            .no = i,
            .mat1 = matrix_a,
            .mat2 = trans,
            .result = result,
        };
        pthread_create(&threads[i],NULL, mmult_fn, &ta[i]);
    }
    for(ssize_t i = 0; i < g_nthreads; ++i) {
        pthread_join(threads[i], NULL);
    }
    free(ta);
    
    return result;
}

/*
This function recursively powers a function to minimise the number
matrix multiply calls.
*/
uint32_t* exp_by_squaring(uint32_t* matrix, uint32_t ex) {
    if(ex == 1) {
        return matrix;
    }
    else if (ex % 2 == 0) {
        uint32_t* new_matrix = matrix_mul(matrix,matrix);
        return exp_by_squaring(new_matrix, ex/2);
    }
    else {
        uint32_t* new_matrix = matrix_mul(matrix,matrix);
        return matrix_mul(matrix, exp_by_squaring(new_matrix,(ex-1)/2));
    }
}

/**
 * Returns new matrix, powering the matrix to the exponent
 */
uint32_t* matrix_pow(const uint32_t* matrix, uint32_t exponent) {

    if(exponent == 0) {
        return identity_matrix();
    }

    uint32_t* result = new_matrix();
    for (ssize_t i = 0; i < g_elements; ++i) {
        result[i] = matrix[i];
    }
    result = exp_by_squaring(result, exponent);
    
    return result;
}

////////////////////////////////
///       COMPUTATIONS       ///
////////////////////////////////

struct thread_arg {
    int no;
    const uint32_t* matrix;
    uint32_t result;
};

void * sum_fn(void *p) {
    struct thread_arg *ptr = (struct thread_arg *) p;
    uint32_t result = 0;
    int start = (ptr->no)*CHUNK/4;
    //use vector addition to add four elements at a time.
    __m128i vsum = _mm_set1_epi32(0);
    const __m128i* mat_ptr = (__m128i*) ptr->matrix;
    for(ssize_t j = 0; j < CHUNK/4; ++j) {
        __m128i v = _mm_load_si128(&mat_ptr[start+j]);
        vsum = _mm_add_epi32(vsum, v);
    }
    //exctract the sums from the final vector.
    result = _mm_extract_epi32(vsum,0);
    result += _mm_extract_epi32(vsum,1);
    result += _mm_extract_epi32(vsum,2);
    result += _mm_extract_epi32(vsum,3);

    ptr->result = result;
    pthread_exit(NULL);
}

/**
 * Returns the sum of all elements
 */
uint32_t get_sum(const uint32_t* matrix) {

    uint32_t result = 0;
    uint32_t unpassed = g_elements % (4*g_nthreads);
    pthread_t threads[g_nthreads];
    struct thread_arg *ta = malloc(sizeof(struct thread_arg) * g_nthreads);

    for(ssize_t i = g_nthreads-1; i > -1; --i) {
        ta[i] = (struct thread_arg) {
            .no = i,
            .matrix = matrix,
            .result = result,
        };
        pthread_create(&threads[i],NULL, sum_fn, &ta[i]);
    }
    for(ssize_t i = 0; i < g_nthreads; ++i) {
        pthread_join(threads[i], NULL);
        result += ta[i].result;
    }
    //add the remaining elements to the sum.
    for(ssize_t i = 0; i < unpassed; ++i) {
        result += matrix[g_elements - i - 1];
    }
    free(ta);
    
    return result;
}

/**
 * Returns the trace of the matrix
 */
uint32_t get_trace(const uint32_t* matrix) {
    
    uint32_t trace = 0;
    for (ssize_t i = 0; i < g_width/2; ++i) {
        //add the first and last elements on the diagonal to the trace
        //in each iteration.
        trace += matrix[i*g_width+i] + matrix[g_elements-i*g_width-i-1];
    }
    //to capture the middle element.
    if(g_width % 2 == 1) {
        trace += matrix[g_width/2];
    }
    
    return trace;
}

struct mmthread_arg {
    int no;
    const uint32_t* matrix;
    uint32_t result;
};

void * min_fn(void *p) {
    struct mmthread_arg *ptr = (struct mmthread_arg *) p;

    uint32_t min = UINT32_MAX;
    int start = (ptr->no)*CHUNK;
    int end = 
        ptr->no == g_nthreads -1 ? g_elements : ((ptr->no)+1)*CHUNK;
    for(int i = start; i< end; ++i){
        if(ptr->matrix[i] < min) {
            min = ptr->matrix[i];
        }
    }
    ptr->result = min;
    pthread_exit(NULL);
}
/**
 * Returns the smallest value in the matrix
 */
uint32_t get_minimum(const uint32_t* matrix) {
    
    uint32_t result = UINT32_MAX;
    pthread_t threads[g_nthreads];
    struct mmthread_arg ta[g_nthreads];

    for(ssize_t i = g_nthreads-1; i > -1; --i) {
        ta[i] = (struct mmthread_arg) {
            .no = i,
            .matrix = matrix,
            .result = result,
        };
        pthread_create(&threads[i],NULL, min_fn, &ta[i]);
    }
    for(ssize_t i = 0; i < g_nthreads; ++i) {
        pthread_join(threads[i], NULL);
        //finding the minimum of the minimums of each thread.
        if(ta[i].result < result) {
            result = ta[i].result;
        }
    }
    
    return result;
}

void * max_fn(void *p) {
    struct mmthread_arg *ptr = (struct mmthread_arg *) p;
    uint32_t max = 0;
    int start = (ptr->no)*CHUNK;
    int end = 
        ptr->no == g_nthreads -1 ? g_elements : ((ptr->no)+1)*CHUNK;
    for(int i = start; i< end; ++i){
        if(ptr->matrix[i] > max) {
            max = ptr->matrix[i];
        }
    }
    ptr->result = max;
    pthread_exit(NULL);
}

/**
 * Returns the largest value in the matrix
 */
uint32_t get_maximum(const uint32_t* matrix) {
    
    uint32_t result = 0;
    pthread_t threads[g_nthreads];
    struct mmthread_arg ta[g_nthreads];

    for(ssize_t i = g_nthreads-1; i > -1; --i) {
        ta[i] = (struct mmthread_arg) {
            .no = i,
            .matrix = matrix,
            .result = result,
        };
        pthread_create(&threads[i],NULL, max_fn, &ta[i]);
    }
    for(ssize_t i = 0; i < g_nthreads; ++i) {
        pthread_join(threads[i], NULL);
        //finding the maximum of the maximums from each thread.
        if(ta[i].result > result) {
            result = ta[i].result;
        }
    }
    
    return result;
}

struct fthread_arg {
    int no;
    uint32_t value;
    const uint32_t* matrix;
    uint32_t result;
};

void * freq_fn(void *p) {
    struct fthread_arg *ptr = (struct fthread_arg *) p;
    //uint32_t freq = 0;
    int start = (ptr->no)*CHUNK;
    int end = 
        ptr->no == g_nthreads -1 ? g_elements : ((ptr->no)+1)*CHUNK;
    for(int i = start; i< end; ++i){
        if(ptr->matrix[i] == ptr->value) {
            ptr->result++;
        }
    }
    // ptr->result = freq;
    pthread_exit(NULL);
}

/**
 * Returns the frequency of the value in the matrix
 */
uint32_t get_frequency(const uint32_t* matrix, uint32_t value) {
    
    uint32_t result = 0;
    pthread_t threads[g_nthreads];
    struct fthread_arg ta[g_nthreads];

    for(ssize_t i = g_nthreads -1; i > -1; --i) {
        ta[i] = (struct fthread_arg) {
            .no = i,
            .value = value,
            .matrix = matrix,
            .result = result,
        };
        pthread_create(&threads[i],NULL, freq_fn, &ta[i]);
    }
    for(ssize_t i = 0; i < g_nthreads; ++i) {
        pthread_join(threads[i], NULL);
        result += ta[i].result;
    }
    
    return result;
}
