/*
 *  Universidade de São Paulo - ICMC
 *  Gabriel Nogueira (Talendar)
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "sorting_algorithms.h"

#define MIN_ARRAY_LEN_EXP 2 //minimum expoent of the size of the generated arrays (minimum k for a size of 10^k)
#define MAX_ARRAY_LEN_EXP 6 //maximum expoent of the size of the generated arrays (maximum k for a size of 10^k)
#define ALMOST_SORTED_ARRAY_K 10 //elements in no more than k positions from their correct sorted position
const bool USE_SCREEN_CLEANER = true; //set it to false in case of incompatibility

/* 
 * Clean the terminal's window.
 */
void clean_screen() {
    if(USE_SCREEN_CLEANER)
        system("@cls||clear");
    else
        printf("\n\n\n\n");
}

/*
 * Prints all the elements an array in a single line.
 */
void print_array(int *a, int size) {
    for(int i = 0; i < size; i++)
        printf("%d ", a[i]);
    printf("\n");
}

/*
 * Write the current date and time to the given .txt file.
 */
void time_now_to_file(FILE *file) {
    time_t raw_time; time(&raw_time);
    struct tm time_info = *localtime(&raw_time);
    fprintf(file, "Date and time of execution: %d/%d/%d %d:%d:%d\n", time_info.tm_year + 1900, time_info.tm_mon + 1, time_info.tm_mday, time_info.tm_hour, time_info.tm_min, time_info.tm_sec);
}

/*
 * Wrapper for the almost_sorted_array function.
 */
int* almost_sorted_array_aux(int size) {
    return almost_sorted_array(size, ALMOST_SORTED_ARRAY_K);
} 

/*
 * Handles the execution and analysis of a single sorting algorithm.
 */
void single_algorithm_menu(SortingInfo* (*sort)(int *array, int size), const char* save_file_path, const char* name)
{
    printf("You have selected %s!\n", name);

    //getting info from the user
    printf("\nEnter the size of the arrays that are going to be generated (max 2*10^9):\n");
    int size; 
    while(scanf(" %d", &size) == 0 || size < 1);

    printf("\nEnter the number of times the sorting should be performed on each array case (1 - 10):\n");
    int num_executions; 
    while(scanf(" %d", &num_executions) == 0 || num_executions < 1 || num_executions > 10);

    printf("\nSorting... this might take some time!\n");

    //running the algorithm
    SortingInfo *rand_info = analyze_sorting_algorithm(sort, &random_array, size, num_executions); //randomly generated array
    SortingInfo *semi_info = analyze_sorting_algorithm(sort, &almost_sorted_array_aux, size, num_executions); //semi-sorted array
    SortingInfo *inv_info = analyze_sorting_algorithm(sort, &inversely_sorted_array, size, num_executions); //inversely sorted array
    SortingInfo *sorted_info = analyze_sorting_algorithm(sort, &sorted_array, size, num_executions); //sorted array

    //printing results
    clean_screen();
    printf("Finished sorting with %s!\n" 
            "Size of the arrays: %d  |  Number of executions per array case: %d\n\n", name, size, num_executions);
    printf("[RANDOM ARRAYS]  Time: %.7lfs  |  Comparisons: %lld  |  Swaps: %lld\n", 
            get_execution_time(rand_info), get_comparisons_count(rand_info), get_swaps_count(rand_info));
    printf("[ALMOST SORTED ARRAYS]  Time: %.7lfs  |  Comparisons: %lld  |  Swaps: %lld\n", 
            get_execution_time(semi_info), get_comparisons_count(semi_info), get_swaps_count(semi_info));
    printf("[INVERSELY SORTED ARRAYS]  Time: %.7lfs  |  Comparisons: %lld  |  Swaps: %lld\n", 
            get_execution_time(inv_info), get_comparisons_count(inv_info), get_swaps_count(inv_info));
    printf("[SORTED ARRAYS]  Time: %.7lfs  |  Comparisons: %lld  |  Swaps: %lld\n", 
            get_execution_time(sorted_info), get_comparisons_count(sorted_info), get_swaps_count(sorted_info));

    //saving results
    FILE *file = fopen(save_file_path, "w");  assert(file != NULL);
    time_now_to_file(file);
    save_sorting_info(rand_info, semi_info, inv_info, sorted_info, size, num_executions, file);
    fclose(file);

    //freeing memory
    free(rand_info);  free(semi_info);  free(inv_info);  free(sorted_info);

    //exit
    char opt;
    printf("\nInformations saved to \"%s\". \nEnter 0 to go back to the main menu.\n", save_file_path);
    do { scanf(" %c", &opt); } while(opt != '0');
}

/*
 * Print to a file (fprintf) the performance of an algorithm on all the array sizes for a given case.
 */
void fprint_case(FILE *file, SortingInfo **info, int min_size, int max_size) 
{
    int num_sizes = max_size - min_size + 1;
    for(int j = 0; j < num_sizes; j++) {
        int k = (j + MIN_ARRAY_LEN_EXP);
        fprintf(file, "\t< SIZE: %d (10^%d)  |  TIME: %.8lfs  |  COMPARISONS: %lld  |  SWAPS: %lld >\n", 
            (int)pow(10, k), k, get_execution_time(info[j]), get_comparisons_count(info[j]), get_swaps_count(info[j]));
    }
}

/*
 * Run all the sorting algorithms on the same arrays on multiple scenarios, so that they can be analyzed and compared.
 */
void analyze_all()
{
    printf("< Analyzing all the sorting algorithms! >\n\n");
    printf("Each sorting algorithm will be executed n times on n different arrays for each of the 4 arrays cases (randomly generated, almost sorted, sorted and inversely sorted) and for each pre-chosen array size. For a given array case and size, all the algorithms will be executed on the same n arrays, so that their performance can be better compared. The size of each array will be 10^k, where k goes from %d to %d.\n", MIN_ARRAY_LEN_EXP, MAX_ARRAY_LEN_EXP);
    
    //GETTING ARRAY CASES
    char cases_opt = '\0';
    bool random_case = false, almost_case = false, inv_case = false, sorted_case = false;
    do {
        if(cases_opt != '\0')
            printf("Invalid option!\n");

        printf("\nSelect the array generation policy.\n"
            "\t1 - ALL CASES\n"
            "\t2 - RANDOM ARRAYS ONLY\n"
            "\t3 - ALMOST SORTED ARRAYS ONLY\n"
            "\t4 - INVERSELY SORTED ARRAYS ONLY\n"
            "\t5 - SORTED ARRAYS ONLY\n"
            "Choose an option: ");
    } while(scanf(" %c", &cases_opt) == 1 && 
            (cases_opt != '1' && cases_opt != '2' && cases_opt != '3' && cases_opt != '4' && cases_opt != '5'));

    switch(cases_opt) {
        case '1':   
            random_case = almost_case = inv_case = sorted_case = true;  break;
        case '2':
            random_case = true;  break;
        case '3':
            almost_case = true;  break;
        case '4':
            inv_case = true;  break;
        case '5':
            sorted_case = true;  break;
    }

    //GETTING NUM OF EXECUTIONS
    printf("\nEnter a value for n (number of executions per array case):  (1 - 10)\n");
    int num_executions; 
    while(scanf(" %d", &num_executions) == 0 || num_executions < 1 || num_executions > 10);

    //SORTING
    clean_screen();
    printf("Sorting... this might take some time!\n\n");
    SortingInfo ***rand_info, ***semi_info, ***inv_info, ***sorted_info;

    if(random_case) {
        printf("-> RANDOM ARRAYS:\n");
        rand_info = analyze_case_on_all(&random_array, MIN_ARRAY_LEN_EXP, MAX_ARRAY_LEN_EXP, num_executions);
    }

    if(almost_case) {
        printf("\n-> ALMOST SORTED ARRAYS (k = %d):\n", ALMOST_SORTED_ARRAY_K);
        semi_info = analyze_case_on_all(&almost_sorted_array_aux, MIN_ARRAY_LEN_EXP, MAX_ARRAY_LEN_EXP, num_executions);
    }

    if(inv_case) {
        printf("\n-> INVERSELY SORTED ARRAYS:\n");
        inv_info = analyze_case_on_all(&inversely_sorted_array, MIN_ARRAY_LEN_EXP, MAX_ARRAY_LEN_EXP, num_executions);
    }

    if(sorted_case) {
        printf("\n-> SORTED ARRAYS:\n");
        sorted_info = analyze_case_on_all(&sorted_array, MIN_ARRAY_LEN_EXP, MAX_ARRAY_LEN_EXP, num_executions);
    }

    //SAVING
    FILE *files[] = {fopen("./results/bubble_sort.txt", "w"), fopen("./results/flagged_bubble_sort.txt", "w"), 
                    fopen("./results/selection_sort.txt", "w"), fopen("./results/insertion_sort.txt", "w"), 
                    fopen("./results/heapsort.txt", "w"), fopen("./results/shellsort.txt", "w"),
                    fopen("./results/quicksort.txt", "w"), fopen("./results/merge_sort.txt", "w")};

    const int num_sizes = MAX_ARRAY_LEN_EXP - MIN_ARRAY_LEN_EXP + 1;

    for(int i = 0; i < 8; i++) {
        FILE *f = files[i];  assert(f != NULL);
        time_now_to_file(f);
        fprintf(f, "Number of executions per array case and size: %d\n\n", num_executions);

        //random arrays
        if(random_case) {
            fprintf(f, "-> RANDOM ARRAYS:\n");
            fprint_case(f, rand_info[i], MIN_ARRAY_LEN_EXP, MAX_ARRAY_LEN_EXP);
        }

        //almost sorted arrays
        if(almost_case) {
            fprintf(f, "\n-> ALMOST SORTED ARRAYS (k = %d):\n", ALMOST_SORTED_ARRAY_K);
            fprint_case(f, semi_info[i], MIN_ARRAY_LEN_EXP, MAX_ARRAY_LEN_EXP);
        }

        //inversely sorted arrays
        if(inv_case) {
            fprintf(f, "\n-> INVERSELY SORTED ARRAYS:\n");
            fprint_case(f, inv_info[i], MIN_ARRAY_LEN_EXP, MAX_ARRAY_LEN_EXP);
        }

        //sorted arrays
        if(sorted_case) {
            fprintf(f, "\n-> SORTED ARRAYS:\n");
            fprint_case(f, sorted_info[i], MIN_ARRAY_LEN_EXP, MAX_ARRAY_LEN_EXP);
        }
    }
    
    //FREEING MEMORY
    for(int i = 0; i < 8; i++) {
        fclose(files[i]);
        for(int j = 0; j < num_sizes; j++) {
            if(random_case) free(rand_info[i][j]);  
            if(almost_case) free(semi_info[i][j]);  
            if(inv_case) free(inv_info[i][j]);  
            if(sorted_case) free(sorted_info[i][j]);
        }
        
        if(random_case) free(rand_info[i]);  
        if(almost_case) free(semi_info[i]);  
        if(inv_case) free(inv_info[i]);  
        if(sorted_case) free(sorted_info[i]);
    }
    
    if(random_case) free(rand_info);  
    if(almost_case) free(semi_info);  
    if(inv_case) free(inv_info);  
    if(sorted_case) free(sorted_info);

    //EXIT
    char opt;
    printf("\nDone! All informations saved to \"./results\". \nEnter 0 to go back to the main menu.\n");
    do { scanf(" %c", &opt); } while(opt != '0');
}

/*
 * Handles the display of the main menu and its operations.
 */
void main_menu()
{
    const char *const MENU_TXT = 
        "< Sorting algorithms analyzer by Talendar (Gabriel Nogueira) >\n\n"
        "  1 - Bubble Sort\n"
        "  2 - Flagged Bubble Sort (Bubble Sort with sentinel)\n"
        "  3 - Selection Sort\n"
        "  4 - Insertion Sort\n"
        "  5 - Heapsort\n"
        "  6 - Shellsort (using Ciura's gap sequence)\n"
        "  7 - Quicksort (partitioning with a random pivot)\n"
        "  8 - Merge Sort\n"
        "  9 - ALL\n"
        "  0 - Exit\n"
        "\nChoose an option: ";

    char opt;
    clean_screen();

    do {
        printf("%s", MENU_TXT);
        scanf(" %c", &opt);

        if(opt != '0')
            clean_screen();

        switch(opt)
        {
            case '1':
                single_algorithm_menu(&bubble_sort, "./results/bubble_sort.txt", "Bubble Sort");
                break;
            case '2':
                single_algorithm_menu(&flagged_bubble_sort, "./results/flagged_bubble_sort.txt", "Flagged Bubble Sort (Bubble Sort with sentinel)");
                break;
            case '3':
                single_algorithm_menu(&selection_sort, "./results/selection_sort.txt", "Selection Sort");
                break;
            case '4':
                single_algorithm_menu(&insertion_sort, "./results/insertion_sort.txt", "Insertion Sort");
                break;
            case '5':
                single_algorithm_menu(&heapsort, "./results/heapsort.txt", "Heapsort");
                break;
            case '6':
                single_algorithm_menu(&shellsort, "./results/shellsort.txt", "Shellsort");
                break;
            case '7':
                single_algorithm_menu(&quicksort, "./results/quicksort.txt", "Quicksort");
                break;
            case '8':
                single_algorithm_menu(&merge_sort, "./results/merge_sort.txt", "Merge Sort");
                break;
            case '9':
                analyze_all();
                break;
            case '0':
                printf("\nLeaving...\n\n");
        }

        if(opt != '0')
            clean_screen();

    } while(opt != '0');
}

/*
 * MAIN
 */
int main() {
    main_menu();
    return 0;
}
