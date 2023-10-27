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

const char* SHARED_MEMORY_NAME = "share_memory_data";
const char* SEMAPHORE_NAME = "semaphore";
const int ACCESS_MODE = 0666;
const int SIZE_SHARED_MEMORY = 4096;
const int MAX_SIZE_OF_STRING = 50;

void error_processing(bool exception, char* bug_report) {
    if (exception) {
        write(STDERR_FILENO, bug_report, strlen(bug_report) * sizeof(char));
        shm_unlink(SHARED_MEMORY_NAME);
        sem_unlink(SEMAPHORE_NAME);
        exit(-1);
    }
}