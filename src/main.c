/*
   Copyright (C) 2014 Gabriel Augendre <gabriel@augendre.info>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <math.h>
#include <stdlib.h>
#include <stdio.h> // perror(3),printf(3)
#include <pthread.h> // threads
#include <semaphore.h>
#include <sys/time.h>

#define MAX_THREADS 8

struct threadData {
    unsigned int start;
    unsigned int stop;
    unsigned int threadNum;
    sem_t sem;
};

unsigned int total_counter = 0;
sem_t sem_counter;
sem_t sem_threads;

void* getInt(void* userNumber);
void emptyBuffer();
int isPrime(const int NUMBER);
unsigned int getNumber(const char* nom);

void emptyBuffer() {
    while (getchar() != '\n') ;
}

int isPrime(const int NUMBER) {
    if (NUMBER <= 1) // Negatives, 0 and 1 are not prime.
        return 0;

    if (NUMBER == 2) // Two is the only even prime number.
        return 1;

    if (NUMBER % 2 == 0) // Others are not prime.
        return 0;

    int i;
    int square = (int) sqrt(NUMBER);
    /* We already checked 0, 1 and 2.
     * + When checking if N = a * b, either a or b is lower or equal to sqrt(N).
     * Checking above sqrt(N) would be useless.
     */
    for (i = 3; i <= square; ++i) {
        if (NUMBER % i == 0)
            return 0;
    }
    return 1;
}

unsigned int getNumber(const char* nom) {
    int ok = 0;
    unsigned int n = 0;
    do {
        printf("Input %s > 0 : ", nom);
        ok = scanf("%d", &n);
        emptyBuffer();
    } while (!ok || n < 0);

    return n;
}

int main(int argc, const char *argv[]) {
    struct timeval beg, end;

    unsigned int userNumber = getNumber("n (max)");


    gettimeofday(&beg, NULL);

    int n = userNumber / 10000;
    if (userNumber % 10000 > 0) {
        ++n;
    }
    struct threadData* jobs = (struct threadData*) malloc(n * sizeof(struct threadData));
    if (jobs == NULL) {
        printf("Not enough memory available :)\nBOOOOM.\n");
        exit(42);
    }
    unsigned int i;
    unsigned int start = 0;
    struct threadData* ptr;
    struct threadData* end_ptr = jobs + n;
    for (ptr = jobs, i = 0; ptr < end_ptr; ++ptr, ++i) {
        unsigned int stop = start + 10000;
        if (stop > userNumber) {
            stop = userNumber;
        }
        ptr->start = start;
        ptr->stop = stop;
        sem_init(&ptr->sem, 0, 0);
        start = stop + 1;
        if (start > userNumber) {
            start = userNumber;
        }
        printf("Created task start: %d, stop: %d\n", ptr->start, ptr->stop);
    }

    sem_init(&sem_threads, 0, MAX_THREADS);
    sem_init(&sem_counter, 0, 1);

    // ----- Beginning of computing

    for (ptr = jobs; ptr < end_ptr; ++ptr) {
        pthread_t thread;
        sem_wait(&sem_threads);
        pthread_create(&thread, NULL, getInt, ptr);
        sem_wait(&ptr->sem);
    }

    for (i = 0; i < MAX_THREADS; ++i) {
        sem_wait(&sem_threads);
    }

    // ----- End of computing

    printf("\n%d found under %d (included).\n", total_counter, userNumber);

    gettimeofday(&end, NULL);

    unsigned long long beg_millisec =
            (unsigned long long)(beg.tv_sec) * 1000 +
            (unsigned long long)(beg.tv_usec) / 1000;
    unsigned long long end_millisec =
            (unsigned long long)(end.tv_sec) * 1000 +
            (unsigned long long)(end.tv_usec) / 1000;
    double elapsed = ((double) (end_millisec - beg_millisec) / 1000);

    printf("Took %lf seconds\n", elapsed);

    return 0;
}

void* getInt(void* arg) {
    struct threadData* data = arg;
    unsigned int start = data->start;
    unsigned int stop = data->stop;
    unsigned int threadNum = data->threadNum;
    sem_post(&data->sem);

    unsigned int i;
    unsigned int counter = 0;

    for (i = start; i <= stop; ++i) {
        if (isPrime(i)) {
            ++counter;
        }
    }

    sem_wait(&sem_counter);
    total_counter += counter;
    sem_post(&sem_counter);

//    printf("EXITING THREAD %d - {%d, %d} - %d\n", threadNum, start, stop, stop - start);
    sem_post(&sem_threads);
    pthread_exit((void*) counter);
}
