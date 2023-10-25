#pragma once

#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>

const char* shared_memory_name = "share_memory_data";
const char* semaphore_name = "share_memory_data";
const int access_mode = 0666;
const int size_shared_memory = 4096;
const int max_size_of_string = 50;

void error_processing(bool exception, char* bug_report) {
    if (exception) {
        write(STDERR_FILENO, bug_report, strlen(bug_report) * sizeof(char));
        shm_unlink(shared_memory_name);
        sem_unlink(semaphore_name);
        exit(-1);
    }
}