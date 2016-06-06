#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <immintrin.h>

#include "pagerank.h"
//Global Variables
int g_npages;
double g_dampener;
double_t* g_set;
double_t* g_set1;
double_t threshold = EPSILON * EPSILON;
double_t g_residual = 0.0;
pthread_mutex_t* g_worker_lock;
pthread_cond_t* g_full; 
int g_nthreads, g_thread_count, g_end; 

typedef struct worker_arg worker_arg;
struct worker_arg 
{
	node* head;
	int length; 
};

void* score_worker(void* arg)
{
	double_t res_sum;
	double_t residual = 0.0;
	double_t* temp;
	node* head = ((worker_arg*)arg)->head;
	int start = head->page->index;
	int length = ((worker_arg*)arg)->length;
	node* current = head;
	node* current1;
	int count = 0;
	node** inlinks = malloc(sizeof(node*) * length);
	double_t* sum = malloc(sizeof(double_t) * length);
	__m128d pta, ptb, pt1a, pt1b, a, b, suma, sumb;
	__m128d dampener = _mm_set_pd(g_dampener,g_dampener);
	double constan = (1.0-g_dampener)/g_npages;
	__m128d constant = _mm_set_pd(constan,constan);
	
	// initialse inlink array and calculate sums
	for(count = 0; count < length; count++)
	{
		inlinks[count] = current->page->inlinks;
		current1 = inlinks[count];
		
		sum[count] = 0.0;
		
		while(current1 != NULL)
		{
			sum[count] += g_set[current1->page->index] / (double_t) current1->page->noutlinks;
			current1 = current1->next;
		}
		
		current = current->next;
	}
	
	if(count == length)
	{
		//calculation of residual using sse
		if(length != 0)
		{
			int i, j;
			for(i = start, j = 0; i < start+length-4; i+=4, j+=4)
			{
				g_set1[start+count] = (1.0-dampener) / npages + dampener * sum[count]
				256 bit attempt
				pt = _mm256_loadu_pd(&g_set[i]);
				ptsum = _mm256_loadu_pd(&sum[j]);
				pt1 = _mm256_mul_pd(dampener, ptsum);
				pt1 = _mm256_add_pd(pt1, constant);
				_mm256_storeu_pd(&g_set1[i], pt1);
				diff = _mm256_sub_pd(pt1,pt);
				diff = _mm256_mul_pd(diff, diff);
				diff = _mm256_hadd_pd(diff,diff);
				_mm256_storeu_pd(&res_sum,diff);
				residual += res_sum;
								
				// pta = _mm_loadu_pd(&g_set[i]);
    //             ptb = _mm_loadu_pd(&g_set[i+2]);
                
    //             suma = _mm_loadu_pd(&sum[j]);
    //             sumb = _mm_loadu_pd(&sum[j+2]);
                
    //             pt1a = _mm_mul_pd(dampener, suma);
    //             pt1b = _mm_mul_pd(dampener, sumb);
                
    //             pt1a = _mm_add_pd(pt1a, constant);
    //             pt1b = _mm_add_pd(pt1b, constant);
                
    //             _mm_storeu_pd(&g_set1[i], pt1a);
    //             _mm_storeu_pd(&g_set1[i+2], pt1b);
                
    //             a = _mm_sub_pd(pt1a, pta);
    //             a = _mm_mul_pd(a, a);
                
    //             b = _mm_sub_pd(pt1b, ptb);
    //             b = _mm_mul_pd(b, b);
                
    //             a = _mm_hadd_pd(a, b);
    //             a = _mm_hadd_pd(a, a);
    //             _mm_store1_pd(&res_sum, a);
    //             residual += res_sum;
			
			}
			// calculate for the remainder
			for(int k = i; k < length + start; k++)
			{
				g_set1[k] = (1.0-g_dampener)/g_npages + g_dampener * sum[k-start];
				residual += pow(g_set1[k] - g_set[k],2);
			}
		}
		
		pthread_mutex_lock(g_worker_lock);
		g_residual += residual;
		
		if(g_thread_count > 0)
		{
			g_thread_count--;
		}
		if (g_thread_count == 0 )
		{
			if(g_residual > threshold)
			{
				//reset for next run
				g_residual = 0.0;
				g_thread_count = g_nthreads;
			}
			else
			{
				g_end = 1;
			}
			// move set1 to set and prepare for next one
			temp = g_set1;
			g_set1 = g_set;
			g_set = temp;
			
			pthread_cond_broadcast(g_full);
		}
		else
		{
			pthread_cond_wait(g_full, g_worker_lock);
		}
		
		pthread_mutex_unlock(g_worker_lock);
		if(g_end)
		{
			return NULL;
		}
		
		
	}
	
	// repeat until done
	
	
	while(1)
	{
		for(count = 0; count < length; count++)
		{
			current1 = inlinks[count];
			
			sum[count] = 0.0;
		
			while(current1 != NULL)
			{
				sum[count] += g_set[current1->page->index] / (double_t) current1->page->noutlinks;
				current1 = current1->next;
			}
		
			current = current->next;
		}
		
		//calculation of residual using sse
		if(length != 0)
		{
			int i, j;
			for(i = start, j = 0; i < start+length-4; i+=4, j+=4)
			{
				//g_set1[start+count] = (1.0-dampener) / npages + dampener * sum[count]
				// pt = _mm256_loadu_pd(&g_set[i]);
				// ptsum = _mm256_loadu_pd(&sum[j]);
				// pt1 = _mm256_mul_pd(dampener, ptsum);
				// pt1 = _mm256_add_pd(pt1, constant);
				// _mm256_storeu_pd(&g_set1[i], pt1);
				// diff = _mm256_sub_pd(pt1,pt);
				// diff = _mm256_mul_pd(diff, diff);
				// diff = _mm256_hadd_pd(diff,diff);
				// _mm256_storeu_pd(&res_sum,diff);
				// residual += res_sum;
				
									
				pta = _mm_loadu_pd(&g_set[i]);
                ptb = _mm_loadu_pd(&g_set[i+2]);
                
                suma = _mm_loadu_pd(&sum[j]);
                sumb = _mm_loadu_pd(&sum[j+2]);
                
                pt1a = _mm_mul_pd(dampener, suma);
                pt1b = _mm_mul_pd(dampener, sumb);
                
                pt1a = _mm_add_pd(pt1a, constant);
                pt1b = _mm_add_pd(pt1b, constant);
                
                _mm_storeu_pd(&g_set1[i], pt1a);
                _mm_storeu_pd(&g_set1[i+2], pt1b);
                
                a = _mm_sub_pd(pt1a, pta);
                a = _mm_mul_pd(a, a);
                
                b = _mm_sub_pd(pt1b, ptb);
                b = _mm_mul_pd(b, b);
                
                a = _mm_hadd_pd(a, b);
                a = _mm_hadd_pd(a, a);
                _mm_store1_pd(&res_sum, a);
                residual += res_sum;
				
			}
			// calculate for the remainder
			for(int k = i; k < length + start; k++)
			{
				g_set1[k] = (1.0-g_dampener)/g_npages + g_dampener * sum[k-start];
				residual += pow(g_set1[k] - g_set[k],2);
			}
		}
		
		pthread_mutex_lock(g_worker_lock);
		g_residual += residual;
		
		if(g_thread_count > 0)
		{
			g_thread_count--;
		}
		if (g_thread_count == 0 )
		{
			if(g_residual > threshold)
			{
				//reset for next run
				g_residual = 0.0;
				g_thread_count = g_nthreads;
			}
			else
			{
				g_end = 1;
			}
			// move set1 to set and prepare for next one
			temp = g_set1;
			g_set1 = g_set;
			g_set = temp;
			
			pthread_cond_broadcast(g_full);
		}
		else
		{
			pthread_cond_wait(g_full, g_worker_lock);
		}
		
		pthread_mutex_unlock(g_worker_lock);
		if(g_end)
		{
			return NULL;
		}
		
		
	}
	
}

void printer(node* list)
{
	node* n = list;
	for(int i = 0; i < g_npages; i++)
	{
		printf("%s %.4f\n", n->page->name, g_set[i]);
	}
}
void pagerank(node* list, size_t npages, size_t nedges, size_t nthreads, double dampener) {

	/*
		TODO

		- implement this function
		- implement any other necessary functions
		- implement any other useful data structures
	*/
	
	double_t initial_score = 1.0 / npages;
	node* current = list;
	worker_arg* worker;
	pthread_t* threads = malloc(sizeof(pthread_t) * nthreads);
	int remain = npages % nthreads;
	
	//initialse globals
	g_npages = npages;
	g_dampener = dampener;
	g_nthreads = nthreads; 
	g_thread_count = nthreads;
	g_set = malloc(sizeof(double_t)*npages);
	g_set1 = malloc(sizeof(double_t)*npages);
	g_worker_lock = malloc(sizeof(pthread_mutex_t));
	g_full = malloc(sizeof(pthread_cond_t));
	// initialise scores 
	for(int i = 0; i < npages; ++i)
	{
	    g_set[i] = initial_score;
	}
	
	//initialise mutex 
	pthread_mutex_init(g_worker_lock, NULL);
	pthread_cond_init(g_full , NULL);
	
	//initialse first worker
	worker = (worker_arg*)malloc(sizeof(worker_arg));
	worker->head = current;
	worker->length = npages/nthreads + (0 < remain--);
	pthread_create(&threads[0], NULL, &score_worker, worker);

	//initialise workers: distribute pages to threads with remainders as well
	for(int i = 1; i < nthreads; i++)
	{
		worker = (worker_arg*)malloc(sizeof(worker_arg));
		
		//shift pointer to start of next block 
		for(size_t i = 0; i < npages/nthreads; i++)
		{
			current = current -> next;
		}
		//if the last worker had a remainder 
		if(-1 < remain)
		{
			current = current -> next;
		}
		
		worker->head = current;
		worker->length = npages/nthreads + (0 < remain--);
		pthread_create(&threads[0], NULL, &score_worker, worker);
		
	
	}
	
	//join threads
	for(size_t i = 0; i < nthreads; i++)
	{
		pthread_join(threads[i], NULL);
	}
	
	printer(list);
	
	free(g_set);
	free(g_set1);
	free(threads);
	free(g_worker_lock);
	free(g_full);
	

}

/*
######################################
### DO NOT MODIFY BELOW THIS POINT ###
######################################
*/

int main(int argc, char** argv) {

	/*
	######################################################
	### DO NOT MODIFY THE MAIN FUNCTION OR HEADER FILE ###
	######################################################
	*/

	config conf;

	init(&conf, argc, argv);

	node* list = conf.list;
	size_t npages = conf.npages;
	size_t nedges = conf.nedges;
	size_t nthreads = conf.nthreads;
	double dampener = conf.dampener;

	pagerank(list, npages, nedges, nthreads, dampener);

	release(list);

	return 0;
}
