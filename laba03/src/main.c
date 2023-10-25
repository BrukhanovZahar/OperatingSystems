#include "../include/general.h"

void file_name_correction(char* file_name) {
    for (unsigned long symbol_index = 0; symbol_index < strlen(file_name); ++symbol_index) {
        if (file_name[symbol_index] == '\n') {
            file_name[symbol_index] = '\0';
            return;
        }
    }
}

int main() {
    shm_unlink(shared_memory_name);
    sem_unlink(semaphore_name);

    int file_descriptor_for_exchange = shm_open(shared_memory_name, O_CREAT | O_RDWR, access_mode);
    error_processing(file_descriptor_for_exchange == -1, "Shm_open error");
    error_processing(ftruncate(file_descriptor_for_exchange, size_shared_memory) == -1, "Truncate child error");

    void* result_data_for_exchange = mmap(NULL, size_shared_memory, PROT_READ | PROT_WRITE, MAP_SHARED,
                                          file_descriptor_for_exchange, 0);
    error_processing(result_data_for_exchange == MAP_FAILED, "Mmap child error");
    strcpy(result_data_for_exchange, "\0");

    char file_name[max_size_of_string];
    error_processing(read(STDIN_FILENO, file_name, sizeof(file_name)) <= 0, "Error reading from stdin");
    file_name_correction(file_name);

    int file_descriptor;
    error_processing((file_descriptor = open(file_name, O_RDONLY)) == -1, "Can't open file");

    sem_t* semaphore = sem_open(semaphore_name, O_CREAT, access_mode, 0);
    error_processing(semaphore == SEM_FAILED, "Sem_open error");


    pid_t process_id = fork();
    error_processing(process_id < 0, "Process creation error");

    if (process_id == 0) {
        error_processing(dup2(file_descriptor, STDIN_FILENO) < 0, "Error dup2");
        error_processing(execl("child", NULL) < 0, "Child process startup error");
    }

    while(true) {
        sem_wait(semaphore);
        char* answer = (char *) result_data_for_exchange;
        if (strcmp(answer, "\0") == 0) {
            break;
        }
        error_processing(strcmp(answer, "-1") == 0, "Division by 0");
        printf("%s\n", answer);
        sem_post(semaphore);
    }

    sem_close(semaphore);
    shm_unlink(shared_memory_name);
    return 0;
}