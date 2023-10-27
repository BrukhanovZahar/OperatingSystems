#include "../include/general.h"

void string_correction(char* file_name) {
    for (unsigned long symbol_index = 0; symbol_index < strlen(file_name); ++symbol_index) {
        if (file_name[symbol_index] == '\n') {
            file_name[symbol_index] = '\0';
            return;
        }
    }
}

int main() {
    shm_unlink(SHARED_MEMORY_NAME);
    sem_unlink(SEMAPHORE_NAME);

    int file_descriptor_for_exchange = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, ACCESS_MODE);
    error_processing(file_descriptor_for_exchange == -1, "Shm_open error");
    error_processing(ftruncate(file_descriptor_for_exchange, SIZE_SHARED_MEMORY) == -1, "Truncate error");

    void* result_data_for_exchange = mmap(NULL, SIZE_SHARED_MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED,
                                          file_descriptor_for_exchange, 0);
    error_processing(result_data_for_exchange == MAP_FAILED, "Mmap error");
    strcpy(result_data_for_exchange, "\0");
    char file_name[MAX_SIZE_OF_STRING];
    error_processing(read(STDIN_FILENO, file_name, sizeof(file_name)) <= 0, "Error reading from stdin");
    string_correction(file_name);

    int file_descriptor;
    error_processing((file_descriptor = open(file_name, O_RDONLY)) == -1, "Can't open file");

    sem_t* semaphore = sem_open(SEMAPHORE_NAME, O_CREAT, ACCESS_MODE, 0);
    error_processing(semaphore == SEM_FAILED, "Sem_open error");

    pid_t process_id = fork();
    error_processing(process_id < 0, "Process creation error");

    if (process_id == 0) {
        error_processing(dup2(file_descriptor, STDIN_FILENO) < 0, "Error dup2");
        error_processing(execl("child", NULL) < 0, "Child process startup error");
    }

    while (true) {
        sem_wait(semaphore);
        char* answer = (char*) result_data_for_exchange;
        if (strcmp(answer, "\0") == 0) {
            break;
        }
        error_processing(strcmp(answer, "-1") == 0, "Division by 0");
        printf("%s\n", answer);
        sem_post(semaphore);
    }

    sem_close(semaphore);
    sem_unlink(SEMAPHORE_NAME);
    shm_unlink(SHARED_MEMORY_NAME);
    return 0;
}