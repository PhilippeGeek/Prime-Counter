/*
Copyright (C) 2015 Gabriel Augendre <gabriel@augendre.info>

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
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

#include "headers/functions.h"

/**
 * Global variables declaration.
 * They will be used by all threads.
 */
long total_counter = 0;
long* primesArray = NULL;
sem_t* sem_counter = NULL;
sem_t* sem_threads = NULL;
sem_t* sem_primesArray = NULL;

/**
 * Can receive two arguments (positive integer) :
 * The first will be considered as the max number to look for primes under.
 * The second will be the number of threads on which to run the computing.
 *
 * If they are not provided, they will be asked.
 */
int main(int argc, const char *argv[]) {
    struct timeval beg, end;

    if (argc == 1 || argc == 2) {
        printf("Possible usage : ./prime.out [<number to search for primes under> [<number of threads>]]\n");
        printf("The missing values will be asked in the program.\n\n");
    }

    // ----- Getting user number
    long userNumber;
    int correctNumberFound = 0;
    if (argc == 2 || argc == 3) {
        char *endptr = 0;
        long argUserNumber = strtol(argv[1], &endptr, 10);

        if (!(*endptr == '\0' && argv[1] != '\0') || argUserNumber < 0) {
            fprintf(stderr, "The first argument is not a valid positive integer.\n");
        }
        else {
            userNumber = argUserNumber;
            correctNumberFound = 1;
        }
    }

    if (!correctNumberFound) {
        userNumber = getNumber("n (max)", 1, 0);
    }

    int numberOfThreads;
    correctNumberFound = 0;
    if (argc == 3) {
        char *endptr = 0;
        int argThreadNumber = (int) strtol(argv[2], &endptr, 10);

        if (!(*endptr == '\0' && argv[1] != '\0') || argThreadNumber < 0) {
            fprintf(stderr, "The second argument is not a valid positive integer.\n");
        }
        else {
            numberOfThreads = argThreadNumber;
            correctNumberFound = 1;
        }
    }

    if (!correctNumberFound) {
        numberOfThreads = getNumber("thread number (1 if you don't know what a thread is)", 1, 0);
    }

    gettimeofday(&beg, NULL);


    // ----- Job count
    long n = userNumber / CHUNK_SIZE;
    if (userNumber % CHUNK_SIZE > 0) {
        ++n;
    }

    // ----- Allocating primes array
    // Using half of userNumber because even numbers can not be primes.
    sem_unlink("/primesArray");
    sem_primesArray = sem_open("/primesArray", O_CREAT, 0644, 1);
    long primesArraySize = userNumber / 2;
    primesArray = (long*) malloc(primesArraySize * sizeof(long));

    // ----- Reserving memory for job queue
    ThreadData* jobs = (ThreadData*) malloc(n * sizeof(ThreadData));
    if (jobs == NULL) {
        printf("Not enough memory available.\nBOOOOM :)\n");
        free(jobs);
        exit(42);
    }

    // ----- Putting data in job queue
    long i;
    long start = 0;
    ThreadData* ptr;
    ThreadData* end_ptr = jobs + n;

    // -- Counts how long is the number for allocating space for semaphore name
    int numberSize = 0;
    long userNumberBuffer = userNumber;
    while (userNumberBuffer % 10 > 0 || userNumberBuffer / 10 > 0) {
        ++numberSize;
        userNumberBuffer /= 10;
    }

    // -- Counts how long is the prefix for semaphore name
    const char* prefix = "/job";
    int prefixCounter = strlen(prefix);

    char* buf = (char*) malloc((prefixCounter + numberSize) * sizeof(char));
    for (ptr = jobs, i = 0; ptr < end_ptr; ++ptr, ++i) {
        long stop = start + CHUNK_SIZE;
        if (stop > userNumber) {
            stop = userNumber;
        }
        ptr->start = start;
        ptr->stop = stop;
        ptr->startInArray = primesArray + start / 2;
        sprintf(buf, "%s%ld", prefix, i);
        sem_unlink(buf);
        ptr->sem = sem_open(buf, O_CREAT, 0644, 0);
        start = stop + 1;
        if (start > userNumber) {
            start = userNumber;
        }
    }
    free(buf);

    // ----- Semaphores initialization
    sem_unlink("/threads");
    sem_threads = sem_open("/threads", O_CREAT, 0644, numberOfThreads);
    sem_unlink("/counter");
    sem_counter = sem_open("/counter", O_CREAT, 0644, 1);

    // ----- Beginning of computing
    double progress = 0.0;
    for (ptr = jobs; ptr < end_ptr; ++ptr) {
        pthread_t thread;
        sem_wait(sem_threads);
        pthread_create(&thread, NULL, getPrimeCount, ptr);
        sem_wait(ptr->sem);
        if (progress > 1) {
            progress = 1;
        }
        else {
            progress += 1.0 / n;
        }
        printProgress(progress, 70);
    }

    for (i = 0; i < numberOfThreads; ++i) {
        sem_wait(sem_threads);
    }

    for (ptr = jobs; ptr < end_ptr; ++ptr) {
        sem_close(ptr->sem);
    }
    sem_close(sem_threads);
    sem_close(sem_counter);
    free(jobs);
    // ----- End of computing

    gettimeofday(&end, NULL);

    printf("\n%ld found under %ld (included) with %d threads.\n", total_counter, userNumber, numberOfThreads);
    printf("Do you want to display the primes ? (y/N) ");
    if (getUserOkay()) {
        printLongArray(primesArray, primesArraySize);
    }

    unsigned long long beg_millisec =
            (unsigned long long)(beg.tv_sec) * 1000 +
            (unsigned long long)(beg.tv_usec) / 1000;
    unsigned long long end_millisec =
            (unsigned long long)(end.tv_sec) * 1000 +
            (unsigned long long)(end.tv_usec) / 1000;
    double elapsed = ((double) (end_millisec - beg_millisec) / 1000);

    printf("Took %lf seconds\n", elapsed);
    free(primesArray);

    return 0;
}
