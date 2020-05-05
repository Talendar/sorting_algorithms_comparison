/*
 *  Universidade de São Paulo - ICMC
 *  Gabriel de Oliveira Guedes Nogueira (Talendar)
 */

#include "sorting_algorithms.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <string.h>

bool seed_set = false; //set to true when the seed for the random int generator has been chosen

/*
 * Struct to hold the information about the execution of a sorting algorithm.
 */
struct SortingInfo {
    double time;
    long long int comparisons_count, swaps_count;
};

// getters
double get_execution_time(SortingInfo *info) {return info->time;}
long long int get_comparisons_count(SortingInfo *info) {return info->comparisons_count;}
long long int get_swaps_count(SortingInfo *info) {return info->swaps_count;}

/*
 * Add the data from b to a and free b;
 */
static void sum_info(SortingInfo *a, SortingInfo *b) {
    a->time += b->time;
    a->comparisons_count += b->comparisons_count;
    a->swaps_count += b->swaps_count;
    free(b);
}

/*
 * Swaps the positions of the two given elements.
 */
void swap(int *a, int *b) 
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

/*
 * Returns true if the array is sorted, false otherwise.
 */
bool is_sorted(int *array, int size) {
    for(int i = 0; i < size - 1; i++) {
        if(array[i] > array[i+1])
            return false;
    }
    return true;
}

/*
 * Generates a random int array with the specified size. Each element will be within the range [0, size - 1].
 */
int* random_array(int size) 
{
    if(!seed_set) {
        srand(time(NULL));
        seed_set = true;
    }

    int *a = malloc(size * sizeof(int));
    for(int i = 0; i < size; i++) {
        a[i] = rand() % size;
    }

    return a;
}

/*
 * Returns an almost sorted array where each of the "size" elements may be misplaced by no more than "k" positions from the correct sorted order.
 */
int* almost_sorted_array(int size, int k)
{
    if(!seed_set) {
        srand(time(NULL));
        seed_set = true;
    }

    int *a = malloc(size * sizeof(int));
    for(int i = 0; i < size; i++) {
        int entropy = rand() % (k + 1);
        a[i] = i + entropy;
    }

    return a;
}

/*
 * Returns a sorted int array with elements within the range [0, size - 1].
 */
int* sorted_array(int size)
{
    int *a = malloc(size * sizeof(int));
    for(int i = 0; i < size; i++) {
        a[i] = i;
    }

    return a;
}

/*
 * Returns a reversely sorted int array with elements within the range [0, size - 1].
 */
int* inversely_sorted_array(int size)
{
    int *a = malloc(size * sizeof(int));
    for(int i = 0; i < size; i++) {
        a[i] = size - 1 - i;
    }

    return a;
}

/*
 * Run the given sorting algorithm "num_execution" times on arrays with the given "size" generated by the function "generate_array".
 * Returns a pointer to a SortingInfo "object" containing informations about the algorithm's performance.
 */
SortingInfo* analyze_sorting_algorithm(SortingInfo* (*sort)(int *array, int size), int* (*generate_array)(int size), int size, int num_executions)
{
    SortingInfo *info = malloc(sizeof(SortingInfo));
    info->time = info->comparisons_count = info->swaps_count = 0;

    for(int i = 0; i < num_executions; i++) {
        sum_info(info, sort(generate_array(size), size));
    }

    info->time /= num_executions;  info->comparisons_count /= num_executions;  info->swaps_count /= num_executions;
    return info;
}

/*
 * Helper for the analyze_case_on_all function.
 */
static void help_analyze_all(SortingInfo ***info, int alg_index, int size_index, char *name, SortingInfo* (*sort)(int *a, int n), int *array, int *temp_array, int size) 
{
    printf("\t\t\t-> %s...", name); 
    fflush(stdout); 

    memcpy(temp_array, array, size * sizeof(int));  
    SortingInfo *temp_info = sort(temp_array, size);
    
    if(!is_sorted(temp_array, size)) { //check if the sorting was sucessful
        printf("\n\nERROR: %s couldn't sort the given array. The array isn't sorted correctly. Aborting...\n", name);
        exit(1);
    }

    printf("OK! (%.6lfs)\n", temp_info->time);
    sum_info(info[alg_index][size_index], temp_info);
}

/* 
 *
 * Algorithms indexes: bubble_sort[0], flagged_bubble_sort[1], selection_sort[2], insertion_sort[3], heapsort[4], shellsort[5], quicksort[6], merge_sort[7].
 */
SortingInfo*** analyze_case_on_all(int* (*generate_array)(int size), int min_size_exp, int max_size_exp, int num_executions)
{
    int num_sizes = max_size_exp - min_size_exp + 1; //number of different array sizes

    //alocating holders for the sorting info
    SortingInfo ***info = malloc(8 * sizeof(SortingInfo**));  assert(info != NULL);
    for(int i = 0; i < 8; i++) {
        info[i] = malloc(num_sizes * sizeof(SortingInfo*));  assert(info[i] != NULL);
        for(int j = 0; j < num_sizes; j++) {
            info[i][j] = malloc(sizeof(SortingInfo));  assert(info[i][j] != NULL);
            info[i][j]->time = info[i][j]->comparisons_count = info[i][j]->swaps_count = 0;
        }
    }

    //sorting
    for(int k = min_size_exp; k <= max_size_exp; k++) {
        int size = pow(10, k);
        int *temp_array = malloc(size * sizeof(int));
        printf("\t-> SIZE: 10^%d\n", k);

        for(int n = 0; n < num_executions; n++) {
            printf("\t\t-> Execution %d/%d\n", n+1, num_executions);
            int *array = generate_array(size);

            int size_index = k - min_size_exp;
            help_analyze_all(info, 0, size_index, "Bubble Sort", &bubble_sort, array, temp_array, size);
            help_analyze_all(info, 1, size_index, "Flagged Bubble Sort", &flagged_bubble_sort, array, temp_array, size);
            help_analyze_all(info, 2, size_index, "Selection Sort", &selection_sort, array, temp_array, size);
            help_analyze_all(info, 3, size_index, "Insertion Sort", &insertion_sort, array, temp_array, size);
            help_analyze_all(info, 4, size_index, "Heapsort", &heapsort, array, temp_array, size);
            help_analyze_all(info, 5, size_index, "Shellsort", &shellsort, array, temp_array, size);
            help_analyze_all(info, 6, size_index, "Quicksort", &quicksort, array, temp_array, size);
            help_analyze_all(info, 7, size_index, "Merge Sort", &merge_sort, array, temp_array, size);
            
            free(array);
        }
        free(temp_array);

        //mean of the performance for the current array size
        for(int i = 0; i < 8; i++) {
            SortingInfo *s = info[i][k - min_size_exp];

            s->time /= num_executions;
            s->comparisons_count /= num_executions;
            s->swaps_count /= num_executions;
        }
    }

    return info;
}

/*
 * Save the given sorting informations in the given .txt file.
 */
void save_sorting_info(SortingInfo *rand_info, SortingInfo* semi_info, SortingInfo* inv_info, SortingInfo* sorted_info, int arrays_size, int num_executions, FILE *file)
{
    fprintf(file, "\t< ARRAYS SIZE: %d (%.0e)  |  NUMBER OF EXECUTIONS: %d >\n", arrays_size, (double) arrays_size, num_executions);
    fprintf(file, "\t\t[RANDOM ARRAYS]  Time: %.8lfs  |  Comparisons: %lld  |  Swaps: %lld\n", 
            get_execution_time(rand_info), get_comparisons_count(rand_info), get_swaps_count(rand_info));
    fprintf(file, "\t\t[SEMI SORTED ARRAYS]  Time: %.8lfs  |  Comparisons: %lld  |  Swaps: %lld\n", 
            get_execution_time(semi_info), get_comparisons_count(semi_info), get_swaps_count(semi_info));
    fprintf(file, "\t\t[INVERSELY SORTED ARRAYS]  Time: %.8lfs  |  Comparisons: %lld  |  Swaps: %lld\n", 
            get_execution_time(inv_info), get_comparisons_count(inv_info), get_swaps_count(inv_info));
    fprintf(file, "\t\t[SORTED ARRAYS]  Time: %.8lfs  |  Comparisons: %lld  |  Swaps: %lld\n\n", 
            get_execution_time(sorted_info), get_comparisons_count(sorted_info), get_swaps_count(sorted_info));
}

/*
 * Non-optimized version of the bubble sort algorithm.
 */
SortingInfo* bubble_sort(int *array, int size)
{
    SortingInfo *info = malloc(sizeof(SortingInfo));
    info->comparisons_count = info->swaps_count = 0;
    clock_t starting_clock = clock();

    for(int i = size-1; i >= 1; i--) {
        for(int j = 0; j < i; j++) {
            info->comparisons_count++;
            if(array[j] > array[j+1]) {
                swap(&array[j], &array[j+1]);
                info->swaps_count++;
            }
        }
    }
    
    info->time = ( (double) (clock() - starting_clock) ) / CLOCKS_PER_SEC;
    return info;
}

/*
 * Optimized (flagged or with sentinel) version of the bubble sort algorithm.
 */
SortingInfo* flagged_bubble_sort(int *array, int size)
{
    SortingInfo *info = malloc(sizeof(SortingInfo));
    info->comparisons_count = info->swaps_count = 0;
    clock_t starting_clock = clock();

    bool sorted; //this flag/sentinel will be set to true when no swaps have been made during an iteration
    int i = size - 1;

    do {
        sorted = true;
        for(int j = 0; j < i; j++) {
            info->comparisons_count++;
            if(array[j] > array[j+1]) {
                swap(&array[j], &array[j+1]);
                info->swaps_count++;
                sorted = false;
            }
        }
        i--;
    } while(!sorted);
    
    info->time = ( (double) (clock() - starting_clock) ) / CLOCKS_PER_SEC;
    return info;
}

/*
 * Selection sort algorithm.
 */
SortingInfo* selection_sort(int *array, int size)
{
    SortingInfo *info = malloc(sizeof(SortingInfo));
    info->comparisons_count = info->swaps_count = 0;
    clock_t starting_clock = clock();

    for(int i = 0; i < size - 1; i++) {
        int index_min = i;
        for(int j = i+1; j < size; j++) {
            info->comparisons_count++;
            if(array[j] < array[index_min])
                index_min = j;
        }

        if(index_min != i) {
            swap(&array[i], &array[index_min]);
            info->swaps_count++;
        }
    }

    info->time = ( (double) (clock() - starting_clock) ) / CLOCKS_PER_SEC;
    return info;
}

/*
 * Gapped insertion sort algorithm. Return a tuple containing, respectively, the number of comparisons and the number of swaps made.
 */
static long long int* gapped_insertion_sort(int *array, int size, int gap)
{
    long long int *info = calloc(2, sizeof(long long int));
    for(int i = gap; i < size; i++) {
        for(int j = i; (j - gap) >= 0; j -= gap) {
            info[0]++;
            if(array[j] < array[j-gap]) {
                swap(&array[j], &array[j-gap]);
                info[1]++;
            }
            else
                break;
        }
    }

    return info;
}

/*
 * Insertion sort algorithm.
 */
SortingInfo* insertion_sort(int *array, int size)
{
    SortingInfo *info = malloc(sizeof(SortingInfo));
    clock_t starting_clock = clock();

    long long int *temp_info = gapped_insertion_sort(array, size, 1); //gapped insertion sort with a gap of 1 (equivalent to the regular insertion sort)
    info->comparisons_count = temp_info[0];
    info->swaps_count = temp_info[1];
    free(temp_info);

    info->time = ( (double) (clock() - starting_clock) ) / CLOCKS_PER_SEC;
    return info;

    //REGULAR INSERTION SORT
    /*for(int i = 1; i < size; i++) {
        for(int j = i; j > 0; j--) {
            info->comparisons_count++;
            if(array[j] < array[j-1]) {
                swap(&array[j], &array[j-1]);
                info->swaps_count++;
            }
            else
                break;
        }
    }*/
}

/*
 * Returns true if the given array is a max heap. Used only for debugging.
 */
static bool is_max_heap(int *array, int size) 
{
    for(int i = 0; i < (size - 1)/2; i++) {
        if(array[i] < array[2*i + 1] || ((2*i + 2) < size && array[i] < array[2*i + 2]))
            return false;
    }

    return true;
}

/*
 * Assuming that the given array is almost a heap, except for maybe the item in index i (meaning it might have a lower value than its childs), this function will turn the array into a heap (if necessary).
 * Returns a 2d tuple containing, respectively, the number of comparisons and the number of swaps made.
 */
static long long int* max_heapify(int *array, int size, int i)
{
    long long int *info = calloc(2, sizeof(long long int));
    while(i < size) {
        int temp_index = i, 
            left_child = 2*i + 1, 
            right_child = 2*i + 2;

        //checks whether the element at i is greater than its left child
        if(left_child < size && array[temp_index] < array[left_child]) 
            temp_index = left_child;

        //checks whether the element at i is greater than its right child
        if(right_child < size && array[temp_index] < array[right_child]) 
            temp_index = right_child;

        info[0] += 3; //3 comparions were made
        if(temp_index == i)
            //break if the element at i is greater than its childs, that is, if the heap property was reestablished
            break;

        swap(&array[temp_index], &array[i]); //swap the element at i with its left or right child (whichever one has the greatest value)
        info[1]++;
        i = temp_index;
    }

    return info;
}

/*
 * Builds (in-place) a max heap out of the given array.
 * Returns a 2d tuple containing, respectively, the number of comparisons and the number of swaps made.
 */
static long long int* build_max_heap(int *array, int size)
{
    long long int *info = calloc(2, sizeof(long long int));
    for(int i = size/2 - 1; i >= 0; i--) {
        long long int *temp_info = max_heapify(array, size, i);
        info[0] += temp_info[0];
        info[1] += temp_info[1];
        free(temp_info);
    }

    return info;
}

/*
 * Heapsort algorithm.
 */
SortingInfo* heapsort(int *array, int size) 
{
    SortingInfo *info = malloc(sizeof(SortingInfo));
    info->comparisons_count = info->swaps_count = 0;
    clock_t starting_clock = clock();

    build_max_heap(array, size);
    for(int i = size - 1; i > 0; i--) {
        swap(&array[0], &array[i]); //the greatest element of the heap is in its first position!
        long long int *temp_info = max_heapify(array, i, 0);

        info->comparisons_count += temp_info[0];
        info->swaps_count += temp_info[1];
        free(temp_info);
    }

    info->time = ( (double) (clock() - starting_clock) ) / CLOCKS_PER_SEC;
    return info;
}

/*
 * Shellsort algorithm using the Ciura's gap sequence. All the elements after 701 have been approximated with the recursive formula Hk = 2.25*Hk-1 (obtained from the Tokuda's sequence). This approximations seem to improve the algorithm's performance for very big arrays.
 */
SortingInfo* shellsort(int *array, int size) 
{
    SortingInfo *info = malloc(sizeof(SortingInfo));
    info->comparisons_count = info->swaps_count = 0;
    clock_t starting_clock = clock();

    int const GAPS[] = {1, 4, 10, 23, 57, 132, 301, 701, 1577, 3549, 7995, 17965, 40423}; //Ciura's gap sequence (approximation)
    for(int k = 13; k >= 0; k--) {
        long long int *temp_info = gapped_insertion_sort(array, size, GAPS[k]);
        info->comparisons_count += temp_info[0];
        info->swaps_count += temp_info[1];
        free(temp_info);
    }

    info->time = ( (double) (clock() - starting_clock) ) / CLOCKS_PER_SEC;
    return info;
}

/*
 * Partition the given array from start_index to end_index by choosing a random pivot and placing all elements smaller or equal to the pivot to its left and all elements greater than the pivot to its right. Hoare's partitioning method is used here.
 * Return the index of the pivot in the new array.
 */
static int partition(int *array, int start_index, int end_index, long long int *comparisons_counter, long long int *swaps_counter) 
{ 
    //check whether the seed for the rand() function has been set
    if(!seed_set) {
        srand(time(NULL));
        seed_set = true;
    }

    //select a random pivot and place it into the end of the sub-array
    int random_index = start_index + (rand() % (end_index - start_index + 1));
    swap(&array[end_index], &array[random_index]);
    (*swaps_counter)++;

    //Hoare's partitioning
    int pivot = array[end_index];
    int l = start_index - 1, r = end_index;

    while(true) {
        do { l++; /**/ (*comparisons_counter)++; } while(array[l] <= pivot); //incrementing the left pointer until a "wrong" element is found
        do { r--; /**/ (*comparisons_counter)++; } while(array[r] > pivot); //decrementing the right pointer until a "wrong" element is found

        //stop if the pointers have met
        if(l >= r)
            break;

        swap(&array[l], &array[r]); //swap a wrong element in the left with a wrong element in the right, putting them in their right positions
        (*swaps_counter)++;
    }

    swap(&array[end_index], &array[r + 1]); //swap the pivot with the left-most element of the "right" sub-array (sub-array with elements greater than the pivot)
    (*swaps_counter)++;
    
    return r + 1;

    //Lomuto's partitioning (deprecated)
    /*int pivot = array[end_index];
    int i = (start_index - 1);

    for (int j = start_index; j <= end_index - 1; j++) {  
        (*comparisons_counter)++;
        if (array[j] < pivot) { 
            i++;
            swap(&array[i], &array[j]); 
            (*swaps_counter)++;
        } 
    } 

    swap(&array[i + 1], &array[end_index]); 
    (*swaps_counter)++;
    return (i + 1); */
} 

/*
 * Quicksort algorithm used internally.
 */
static void quicksort_aux(int *array, int start_index, int end_index, long long int *comparisons_counter, long long int *swaps_counter) 
{
    (*comparisons_counter)++;
    if (start_index < end_index) { 
        int p = partition(array, start_index, end_index, comparisons_counter, swaps_counter); //pivot

        quicksort_aux(array, start_index, (p - 1), comparisons_counter, swaps_counter); //sort the array to the left of the pivot
        quicksort_aux(array, (p + 1), end_index, comparisons_counter, swaps_counter); //sort the array to the right of the pivot
    } 
}

/*
 * Wrapper for the quicksort_aux function.
 */
SortingInfo* quicksort(int *array, int size) 
{
    SortingInfo *info = malloc(sizeof(SortingInfo));
    clock_t starting_clock = clock();

    long long int comparisons_counter, swaps_counter;
    comparisons_counter = swaps_counter = 0;
    
    quicksort_aux(array, 0, size-1, &comparisons_counter, &swaps_counter);

    info->comparisons_count = comparisons_counter;
    info->swaps_count = swaps_counter;

    info->time = ( (double) (clock() - starting_clock) ) / CLOCKS_PER_SEC;
    return info;
}

/*
 * Merge array[l...m] and array[m+1...r] assuming that they are both sorted. 
 */
static void merge(int *array, int l, int m, int r, long long int *comparisons_counter, long long int *swaps_counter) 
{
    int size_left = m - l + 1, 
        size_right = r - m;

    //alocating memory for the temporary arrays and copying the elements into them
    int *left_array = malloc(size_left * sizeof(int)),
        *right_array = malloc(size_right * sizeof(int));

    for(int i = 0; i < size_left; i++) { left_array[i] = array[l + i]; } 
    for(int i = 0; i < size_right; i++) { right_array[i] = array[m + 1 + i]; }

    //merging
    int i = 0, //current index of the left array
        j = 0, //current index of the right array
        k = l; //current index of the merged array

    while(i < size_left || j < size_right) { //repeat until both sub-arrays have been fully read
        //both sub-arrays havent been fully read
        if(i < size_left && j < size_right) {
            array[k] = (left_array[i] < right_array[j]) ? left_array[i++] : right_array[j++];
            (*comparisons_counter)++;
        }
        //one of the sub-arrays have been fully read
        else {
            array[k] = (i < size_left) ? left_array[i++] : right_array[j++];
        }

        k++;
        (*swaps_counter)++;
    }

    free(left_array);
    free(right_array);
}

/*
 * Merge sort algorithm used internally.
 */
static void merge_sort_aux(int *array, int start_index, int end_index, long long int *comparisons_counter, long long int *swaps_counter)
{
    if(start_index < end_index) {
        int m = (end_index + start_index)/2;

        merge_sort_aux(array, start_index, m, comparisons_counter, swaps_counter);
        merge_sort_aux(array, m + 1, end_index, comparisons_counter, swaps_counter);

        merge(array, start_index, m, end_index, comparisons_counter, swaps_counter);
    }
}

/*
 * Wrapper for the merge_sort_aux function.
 */
SortingInfo* merge_sort(int *array, int size)
{
    SortingInfo *info = malloc(sizeof(SortingInfo));
    clock_t starting_clock = clock();

    long long int comparisons_counter, swaps_counter;
    comparisons_counter = swaps_counter = 0;
    
    merge_sort_aux(array, 0, size-1, &comparisons_counter, &swaps_counter);

    info->comparisons_count = comparisons_counter;
    info->swaps_count = swaps_counter;

    info->time = ( (double) (clock() - starting_clock) ) / CLOCKS_PER_SEC;
    return info;
}