/*
 *  Universidade de São Paulo - ICMC
 *  Gabriel Nogueira (Talendar)
 */

#ifndef SORTING_ALGORITHMS_H
    #define SORTING_ALGORITHMS_H
    #include <stdbool.h>
    #include <stdio.h>

    typedef struct SortingInfo SortingInfo;
    double get_execution_time(SortingInfo *info);
    long long int get_comparisons_count(SortingInfo *info); 
    long long int get_swaps_count(SortingInfo *info);

    void swap(int *a, int *b);
    bool is_sorted(int *array, int size);

    int* random_array(int size);
    int* sorted_array(int size);
    int* almost_sorted_array(int size, int k);
    int* inversely_sorted_array(int size);
    
    SortingInfo* analyze_sorting_algorithm(SortingInfo* (*sort)(int *array, int size), int* (*generate_array)(int size), int size, int num_executions);
    SortingInfo*** analyze_case_on_all(int* (*generate_array)(int size), int min_size_exp, int max_size_exp, int num_executions);
    void save_sorting_info(SortingInfo *rand_info, SortingInfo* semi_info, SortingInfo* inv_info, SortingInfo* sorted_info, int arrays_size, int num_executions, FILE *file);

    SortingInfo* bubble_sort(int *array, int size);
    SortingInfo* flagged_bubble_sort(int *array, int size);
    SortingInfo* selection_sort(int *array, int size);
    SortingInfo* insertion_sort(int *array, int size);

    SortingInfo* shellsort(int *array, int size);
    SortingInfo* heapsort(int *array, int size);
    SortingInfo* quicksort(int *array, int size);
    SortingInfo* merge_sort(int *array, int size);
#endif
