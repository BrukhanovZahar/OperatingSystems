#include "lib.h"
#include <math.h>

float derivative(float point, float delta_x) {
    return (cosf(point + delta_x) - cosf(point - delta_x)) / (2 * delta_x);
}

void sort(int* array, int size) {
    if (size < 2) return;

    int pivot = array[size / 2];
    int left_pointer, right_pointer;

    for (left_pointer = 0, right_pointer = size - 1;; ++left_pointer, --right_pointer) {

        while (array[left_pointer] < pivot) {
            left_pointer++;
        }

        while (array[right_pointer] > pivot) {
            right_pointer--;
        }

        if (left_pointer >= right_pointer) {
            break;
        }

        int temporary = array[left_pointer];
        array[left_pointer] = array[right_pointer];
        array[right_pointer] = temporary;
    }
    sort(array, left_pointer);
    sort(array + left_pointer, size - left_pointer);
}