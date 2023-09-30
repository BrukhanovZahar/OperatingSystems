#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


void error_processing(bool exception, char* bug_report) {
    if (exception) {
        write(STDERR_FILENO, bug_report, strlen(bug_report) * sizeof(char));
        exit(-1);
    }
}

void file_name_correction(char* file_name) {
    for (unsigned long symbol_index = 0; symbol_index < strlen(file_name); ++symbol_index) {
        if (file_name[symbol_index] == '\n') {
            file_name[symbol_index] = '\0';
            return;
        }
    }
}

int main() {
    const int max_size_of_string = 50;
    int pipe_[2];
    error_processing((pipe(pipe_) == -1), "pipe error");
    char file_name[max_size_of_string];
    error_processing(read(fileno(stdin), file_name, sizeof(file_name)) <= 0, "error reading form stdin");
    file_name_correction(file_name);
    int file_descriptor;
    error_processing((file_descriptor = open(file_name, O_RDONLY)) == -1, "Can't open file");

    pid_t process_id = fork();
    error_processing((process_id == -1), "process creation error");
    if (process_id == 0) {
        close(pipe_[0]);
        error_processing(dup2(file_descriptor, STDIN_FILENO) < 0, "error dub");
        char out_descriptor[max_size_of_string];
        error_processing(sprintf(out_descriptor, "%d", pipe_[1]) < 0, "error cast");
        error_processing(execl("child", out_descriptor, NULL) < 0, "child process startup error");
    }
    close(pipe_[1]);

    float result_of_child_program;
    char answer[max_size_of_string];
    while ((read(pipe_[0], &result_of_child_program, sizeof(float))) > 0) {
        error_processing(result_of_child_program == -1, "division by 0");
        error_processing(sprintf(answer, "%f", result_of_child_program) < 0, "error cast");
        error_processing(write(STDOUT_FILENO, answer, strlen(answer)) == -1, "response output error");
        error_processing(write(STDOUT_FILENO, "\n", 1) == -1, "line break output error");
    }
    close(pipe_[0]);
    return 0;
}