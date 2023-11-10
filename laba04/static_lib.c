#include "lib.h"
#include <stdio.h>

int main() {
    int command;

    while (scanf("%d", &command) != EOF) {

        if (command == 1) {

            float point, delta_x;
            printf("Enter 2 numbers (point and delta_x): ");
            scanf("%f %f", &point, &delta_x);
            printf("The result of the first function: %f\n", derivative(point, delta_x));

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

            sort(array, size);

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


    return 0;
}