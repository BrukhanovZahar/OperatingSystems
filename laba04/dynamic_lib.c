#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dlfcn.h>

const char* FIRST_LIBRARY_PATH = "trash/liblib_first.so";
const char* SECOND_LIBRARY_PATH = "trash/liblib_second.so";
void* library_descriptor = NULL;
int current_library = 1;

float (* derivative_func)(float point, float delta_x) = NULL;

void (* sort_func)(int* array, int size) = NULL;

void error_processing(bool exception, char* bug_report) {
    if (exception) {
        fprintf(stderr, "%s", bug_report);
        exit(-1);
    }
}

void open_library(const char* path_to_library) {
    if (library_descriptor != NULL) {
        dlclose(library_descriptor);
    }

    library_descriptor = dlopen(path_to_library, RTLD_LAZY);
    error_processing(library_descriptor == NULL, "Library opening error.\n");

    derivative_func = dlsym(library_descriptor, "derivative");
    error_processing(derivative_func == NULL, "Error in finding the method derivative.\n");

    sort_func = dlsym(library_descriptor, "sort");
    error_processing(sort_func == NULL, "Error in finding the method sort.\n");
}

void swap_library() {
    if (current_library == 1) {

        open_library(SECOND_LIBRARY_PATH);
        current_library = 2;

    } else {

        open_library(FIRST_LIBRARY_PATH);
        current_library = 1;

    }

    printf("Current library - %d\n", current_library);
}

int main() {
    open_library(FIRST_LIBRARY_PATH);
    int command;

    while (scanf("%d", &command) != EOF) {
        if (command == 0) {

            swap_library();

        } else if (command == 1) {

            float point, delta_x;
            printf("Enter 2 numbers (point and delta_x): ");
            scanf("%f %f", &point, &delta_x);
            printf("The result of the first function: %f\n", (*derivative_func)(point, delta_x));

        } else if (command == 2) {

            int size;
            printf("Enter the size of the array: ");
            scanf("%d", &size);

            int array[size];
            printf("Enter the array elements:\n");
            for (int index_array = 0; index_array < size; ++index_array) {
                printf("Element %d: ", index_array + 1);
                scanf("%d", &array[index_array]);
            }

            (*sort_func)(array, size);

            printf("The result of the second function:\n");
            for (int index_array = 0; index_array < size; ++index_array) {
                printf("%d ", array[index_array]);
            }
            printf("\n");

        } else if (command == -1) {

            printf("The program is completed.\n");
            break;

        } else {

            printf("You entered the wrong command.\n");

        }

    }

    dlclose(library_descriptor);
    return 0;
}