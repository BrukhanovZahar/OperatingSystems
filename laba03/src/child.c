#include "../include/general.h"

int main() {
    int file_descriptor_for_exchange = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, ACCESS_MODE);
    error_processing(file_descriptor_for_exchange == -1, "Shm_open error");

    sem_t* semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, ACCESS_MODE, 0);
    error_processing(semaphore == SEM_FAILED, "Sem_open error");

    void* result_data_for_exchange = mmap(NULL, SIZE_SHARED_MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED,
                                          file_descriptor_for_exchange, 0);
    error_processing(result_data_for_exchange == MAP_FAILED, "Mmap error");

    float division_result = 0;
    char character;
    bool is_this_first_number = true;

    while ((read(STDIN_FILENO, &character, 1)) > 0) {
        char* buffer = malloc(sizeof(char) * MAX_SIZE_OF_STRING);
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
                strcpy(result_data_for_exchange, "-1");
                sem_post(semaphore);
                sem_close(semaphore);
                exit(-1);
            }

            division_result /= (float) number;

            if (character == '\n' || character == EOF) {
                sprintf(result_data_for_exchange, "%f", division_result);
                is_this_first_number = true;
                sem_post(semaphore);
                sem_wait(semaphore);
            }
        }
    }
    strcpy(result_data_for_exchange, "\0");
    sem_post(semaphore);
    sem_close(semaphore);
    return 0;
}