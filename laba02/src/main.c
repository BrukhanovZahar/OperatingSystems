#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

const int MAX_SIZE_OF_STRING = 50;
const int SIZE_OF_RESULT_ARRAY = 3;

typedef struct {
    int number_of_remaining_throws_in_round;
    int first_score;
    int second_score;
    int number_of_experiments;
    int* result_array;
} thread_data;

void error_processing(bool exception, char* bug_report) {
    if (exception) {
        write(STDERR_FILENO, bug_report, strlen(bug_report) * sizeof(char));
        exit(STDERR_FILENO);
    }
}

void limiting_the_number_of_threads(int* number_of_threads) {
    int maximum_number_of_threads_in_the_system = (int) sysconf(_SC_NPROCESSORS_ONLN);

    if (*number_of_threads > maximum_number_of_threads_in_the_system) {

        *number_of_threads = maximum_number_of_threads_in_the_system;

        char maximum_number_of_threads_in_the_system_string[MAX_SIZE_OF_STRING];
        sprintf(maximum_number_of_threads_in_the_system_string, "%d", maximum_number_of_threads_in_the_system);

        write(STDOUT_FILENO,
              "the limit of threads in the system has been exceeded, the number of threads has been set: ",
              strlen("the limit of threads in the system has been exceeded, the number of threads has been set: "));
        write(STDOUT_FILENO, maximum_number_of_threads_in_the_system_string,
              strlen(maximum_number_of_threads_in_the_system_string));
        write(STDOUT_FILENO, "\n", 1);
    }
}

void input_string_correction(char* string) {
    for (unsigned long symbol_index = 0; symbol_index < strlen(string); ++symbol_index) {
        if (string[symbol_index] == '\n') {
            string[symbol_index] = '\0';
            return;
        }
    }
}

int read_int(const char* message_to_the_user) {
    char input_buffer[MAX_SIZE_OF_STRING];

    if (strlen(message_to_the_user) > 0) {
        write(STDOUT_FILENO, message_to_the_user, strlen(message_to_the_user));
    }

    error_processing((read(STDIN_FILENO, input_buffer, sizeof(input_buffer)) <= 0), "console reading error");
    input_string_correction(input_buffer);

    return atoi(input_buffer);
}

void output_information_as_a_result_of_program(int* result,
                                               const int64_t time_of_program,
                                               const int number_of_threads,
                                               const int number_of_experiments) {

    const float chance_of_winning_the_first_player = (float) result[1] / (float) number_of_experiments;
    const float chance_of_winning_the_second_player = (float) result[2] / (float) number_of_experiments;

    char time_of_program_string[MAX_SIZE_OF_STRING];
    char number_of_threads_string[MAX_SIZE_OF_STRING];
    char result_first_player[MAX_SIZE_OF_STRING];
    char result_second_player[MAX_SIZE_OF_STRING];
    char result_draw[MAX_SIZE_OF_STRING];
    char chance_of_winning_the_first_player_string[MAX_SIZE_OF_STRING];
    char chance_of_winning_the_second_player_string[MAX_SIZE_OF_STRING];

    sprintf(time_of_program_string, "%lld", time_of_program);
    sprintf(number_of_threads_string, "%d", number_of_threads);
    sprintf(result_first_player, "%d", result[1]);
    sprintf(result_second_player, "%d", result[2]);
    sprintf(result_draw, "%d", result[0]);
    sprintf(chance_of_winning_the_first_player_string, "%f", chance_of_winning_the_first_player);
    sprintf(chance_of_winning_the_second_player_string, "%f", chance_of_winning_the_second_player);

    write(STDOUT_FILENO, "Number of thread = ", strlen("number of thread = "));
    write(STDOUT_FILENO, number_of_threads_string, strlen(number_of_threads_string));
    write(STDOUT_FILENO, "\n", 1);
    write(STDOUT_FILENO, "time spent on the game = ", strlen("time spent on the game = "));
    write(STDOUT_FILENO, time_of_program_string, strlen(time_of_program_string));
    write(STDOUT_FILENO, "\n", 1);
    write(STDOUT_FILENO, "The result is a draw: ", strlen("The result is a draw: "));
    write(STDOUT_FILENO, result_draw, strlen(result_draw));
    write(STDOUT_FILENO, "\n", 1);
    write(STDOUT_FILENO, "The result won first: ", strlen("The result won first: "));
    write(STDOUT_FILENO, result_first_player, strlen(result_first_player));
    write(STDOUT_FILENO, "\n", 1);
    write(STDOUT_FILENO, "The result won second: ", strlen("The result won second: "));
    write(STDOUT_FILENO, result_second_player, strlen(result_second_player));
    write(STDOUT_FILENO, "\n\n", 2);
    write(STDOUT_FILENO, "The chance of winning the first player: ",
          strlen("The chance of winning the first player: "));
    write(STDOUT_FILENO, chance_of_winning_the_first_player_string, strlen(chance_of_winning_the_first_player_string));
    write(STDOUT_FILENO, "\n", 1);
    write(STDOUT_FILENO, "The chance of winning the second player: ",
          strlen("The chance of winning the second player: "));
    write(STDOUT_FILENO, chance_of_winning_the_second_player_string,
          strlen(chance_of_winning_the_second_player_string));
    write(STDOUT_FILENO, "\n", 1);

}

int one_game(const int number_of_remaining_throws_in_round, int first_score, int second_score) {

    for (int tour_number = 0; tour_number < number_of_remaining_throws_in_round; ++tour_number) {
        first_score += rand() % 6 + 1 + rand() % 6 + 1;
        second_score += rand() % 6 + 1 + rand() % 6 + 1;
    }

    if (first_score > second_score) {
        return 1;
    } else if (first_score < second_score) {
        return 2;
    } else {
        return 0;
    }
}

void* thread_function(void* parameters) {
    thread_data* data = (thread_data*) parameters;

    for (int index_of_experiments = 0; index_of_experiments < data->number_of_experiments; ++index_of_experiments) {
        int winner = one_game(data->number_of_remaining_throws_in_round, data->first_score, data->second_score);

        if (winner == 1) {
            data->result_array[1] += 1;
        } else if (winner == 2) {
            data->result_array[2] += 1;
        } else {
            data->result_array[0] += 1;
        }
    }

    pthread_exit(NULL);
}

void starting_and_configuring_threads(const int number_of_threads_from_the_user,
                                      const int number_of_throws_in_tour,
                                      const int tour_number,
                                      const int number_points_of_first_player,
                                      const int number_points_of_second_player,
                                      const int number_of_experiments,
                                      int* result) {

    int experiments_per_thread = number_of_experiments / number_of_threads_from_the_user;
    pthread_t* threads_array = (pthread_t*) malloc(number_of_threads_from_the_user * sizeof(pthread_t));
    thread_data* array_thread_data = (thread_data*) malloc(number_of_threads_from_the_user * sizeof(thread_data));

    for (int index_of_array_thread = 0;
         index_of_array_thread < number_of_threads_from_the_user;
         ++index_of_array_thread) {

        array_thread_data[index_of_array_thread].number_of_remaining_throws_in_round =
                number_of_throws_in_tour - tour_number;
        array_thread_data[index_of_array_thread].first_score = number_points_of_first_player;
        array_thread_data[index_of_array_thread].second_score = number_points_of_second_player;
        array_thread_data[index_of_array_thread].number_of_experiments = experiments_per_thread;
        array_thread_data[index_of_array_thread].result_array = malloc(SIZE_OF_RESULT_ARRAY * sizeof(int));

        if (index_of_array_thread == number_of_threads_from_the_user - 1) {
            array_thread_data[index_of_array_thread].number_of_experiments +=
                    number_of_experiments % number_of_threads_from_the_user;
        }

        error_processing((pthread_create(&(threads_array[index_of_array_thread]), NULL, thread_function,
                                         &array_thread_data[index_of_array_thread])) != 0, "Error creating thread");

    }

    for (int index_of_array_thread = 0;
         index_of_array_thread < number_of_threads_from_the_user;
         ++index_of_array_thread) {

        error_processing((pthread_join(threads_array[index_of_array_thread], NULL)) != 0, "Thread termination error");

        for (int index_of_result_array = 0; index_of_result_array < SIZE_OF_RESULT_ARRAY; ++index_of_result_array) {
            result[index_of_result_array] += array_thread_data[index_of_array_thread].result_array[index_of_result_array];
        }

        free(array_thread_data[index_of_array_thread].result_array);
    }

    free(threads_array);
    free(array_thread_data);
}

int64_t current_time_millis() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (int64_t) (time.tv_sec) * 1000 + (int64_t) (time.tv_usec) / 1000;
}

int main(int argc, char* argv[]) {
    error_processing(argc != 2, "write the number of threads");

    int number_of_threads_from_the_user = atoi(argv[1]);

    limiting_the_number_of_threads(&number_of_threads_from_the_user);

    srand((unsigned) time(NULL));

    int number_of_throws_in_tour = read_int("enter the number of throws: ");
    int tour_number = read_int("enter the tour number: ");
    int number_points_of_first_player = read_int("enter the number of points of the first player: ");
    int number_points_of_second_player = read_int("enter the number of points of the second player: ");
    int number_of_experiments = read_int("enter the number of experiments that the program should perform: ");

    error_processing((number_of_throws_in_tour < tour_number),
                     "Incorrect data: the number of the round cannot be greater than the total number of throws");

    int result[] = {0, 0, 0};

    int64_t start_time = current_time_millis();
    starting_and_configuring_threads(
            number_of_threads_from_the_user,
            number_of_throws_in_tour,
            tour_number,
            number_points_of_first_player,
            number_points_of_second_player,
            number_of_experiments,
            result);
    int64_t stop_time = current_time_millis();

    output_information_as_a_result_of_program(result,
                                              stop_time - start_time,
                                              number_of_threads_from_the_user,
                                              number_of_experiments);

    return 0;
}
