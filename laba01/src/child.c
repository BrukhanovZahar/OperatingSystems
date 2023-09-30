#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc __attribute__((unused)), char* argv[]) {
    const int max_size_of_string = 50;
    int out_descriptor = atoi(argv[0]);
    float division_result = 0;
    char character;
    bool is_this_first_number = true;
    while ((read(STDIN_FILENO, &character, 1)) > 0) {
        char* buffer = malloc(max_size_of_string * sizeof(char));
        int index_of_buffer = 0;

        while (character != ' ' && character != '\n' && character != '\0') {
            buffer[index_of_buffer++] = character;

            if (read(STDIN_FILENO, &character, 1) <= 0) {
                character = EOF;
                break;
            }
        }

        if (is_this_first_number) {
            division_result = strtof(buffer, NULL);
            is_this_first_number = false;
        } else {
            int number = atoi(buffer);

            if (number == 0) {
                float error = -1;
                write(out_descriptor, &error, sizeof(float));
                exit(-1);
            }

            division_result /= (float) number;

            if (character == '\n' || character == EOF) {
                write(out_descriptor, &division_result, sizeof(float));
                is_this_first_number = true;
            }
        }
    }
    close(out_descriptor);
    close(STDIN_FILENO);
    return 0;
}