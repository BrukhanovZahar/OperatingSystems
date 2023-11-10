#include "lib.h"
#include <math.h>

float derivative(float point, float delta_x) {
    return (cosf(point + delta_x) - cosf(point)) / delta_x;
}

void sort(int* array, int size) {

    for (int first_index = 0; first_index < size - 1; ++first_index) {
        for (int second_index = 0; second_index < size - first_index - 1; ++second_index) {

            if (array[second_index] > array[second_index + 1]) {

                int temporary = array[second_index];
                array[second_index] = array[second_index + 1];
                array[second_index + 1] = temporary;

            }
        }
    }
}