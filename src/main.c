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
#include <fcntl.h>

#define CHUNK_SIZE 10000

// Data passed to thread
struct threadData {
    unsigned int start;
    unsigned int stop;
    sem_t* sem;
};

/**
 * Global variables declaration.
 * They will be used by all threads.
 */
unsigned int total_counter = 0;
sem_t* sem_counter;
sem_t* sem_threads;

// Functions declaration
void* getPrimeCount(void* arg);
void emptyBuffer();
int isPrime(const int NUMBER);
unsigned int getNumber(const char* nom, unsigned int min, unsigned int max);

/**
 * Empty buffer.
 * To be called after trying to get an int.
 */
void emptyBuffer() {
    while (getchar() != '\n') ;
}

/**
 * Returns 1 if the given number is prime, 0 if not.
 *
 * Loops through all numbers from 3 to the integer part of the square root of the number.
 */
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

/**
 * Get the number from the user.
 * Ensures it will return a positive integer.
 * Will loop until it gets a correct number from the user.
 *
 * 123foo is considered as 123.
 * foo123 is refused.
 * <space>123 is considered as 123.
 */
unsigned int getNumber(const char* nom, unsigned int min, unsigned int max) {
    int ok = 0;
    unsigned int n = 0;
    int checkMax = 1;
    if (max <= min) {
        checkMax = 0;
    }
    do {
        if (checkMax == 1) {
            printf("Input %d < %s < %d : ", min - 1, nom, max + 1);
        }
        else {
            printf("Input %s > %d : ", nom, min - 1);
        }
        ok = scanf("%d", &n);
        emptyBuffer();
    } while (!ok || n < min || (checkMax == 1 && n > max));

    return n;
}

int main(int argc, const char *argv[]) {
    struct timeval beg, end;

    // ----- Getting user number
    unsigned int userNumber = getNumber("n (max)", 1, 0);
    unsigned int numberOfThreads = getNumber("thread number (1 if you don't know what a thread is)", 1, 0);


    gettimeofday(&beg, NULL);


    // ----- Job count
    int n = userNumber / CHUNK_SIZE;
    if (userNumber % CHUNK_SIZE > 0) {
        ++n;
    }

    // ----- Reserving data for job queue
    struct threadData* jobs = (struct threadData*) malloc(n * sizeof(struct threadData));
    if (jobs == NULL) {
        printf("Not enough memory available.\nBOOOOM :)\n");
        exit(42);
    }

    // ----- Putting data in job queue
    unsigned int i;
    unsigned int start = 0;
    struct threadData* ptr;
    struct threadData* end_ptr = jobs + n;

    // -- Counts how long is the number for allocating space for semaphore name
    int numberSize = 0;
    int userNumberBuffer = userNumber;
    while (userNumberBuffer % 10 > 0 || userNumberBuffer / 10 > 0) {
        ++numberSize;
        userNumberBuffer /= 10;
    }

    // -- Counts how long is the prefix for semaphore name
    const char* prefix = "/job";
    int prefixCounter = 0;
    const char* prefixPtr = prefix;
    while (*prefixPtr != '\0') {
        ++prefixCounter;
        ++prefixPtr;
    }

    char* buf = (char*) malloc((prefixCounter + numberSize) * sizeof(char));
    for (ptr = jobs, i = 0; ptr < end_ptr; ++ptr, ++i) {
        unsigned int stop = start + CHUNK_SIZE;
        if (stop > userNumber) {
            stop = userNumber;
        }
        ptr->start = start;
        ptr->stop = stop;
        sprintf(buf, "%s%d", prefix, i);
        sem_unlink(buf);
        ptr->sem = sem_open(buf, O_CREAT, 0644, 0);
        start = stop + 1;
        if (start > userNumber) {
            start = userNumber;
        }
    }

    // ----- Semaphores initialization
    sem_unlink("/threads");
    sem_threads = sem_open("/threads", O_CREAT, 0644, numberOfThreads);
    sem_unlink("/counter");
    sem_counter = sem_open("/counter", O_CREAT, 0644, 1);

    // ----- Beginning of computing
    for (ptr = jobs; ptr < end_ptr; ++ptr) {
        pthread_t thread;
        sem_wait(sem_threads);
        pthread_create(&thread, NULL, getPrimeCount, ptr);
        sem_wait(ptr->sem);
    }

    for (i = 0; i < numberOfThreads; ++i) {
        sem_wait(sem_threads);
    }

    for (ptr = jobs; ptr < end_ptr; ++ptr) {
        sem_close(ptr->sem);
    }
    sem_close(sem_threads);
    sem_close(sem_counter);
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

/**
 * arg should be an instance of threadData struct.
 * Counts the number of primes between start and stop.
 * Increments the total_counter after the end of range.
 */
void* getPrimeCount(void* arg) {
    struct threadData* data = (struct threadData*) arg;
    unsigned int start = data->start;
    unsigned int stop = data->stop;
    sem_post(data->sem);

    unsigned int i;
    unsigned int counter = 0;

    for (i = start; i <= stop; ++i) {
        if (isPrime(i)) {
            ++counter;
        }
    }

    sem_wait(sem_counter);
    total_counter += counter;
    sem_post(sem_counter);

    sem_post(sem_threads);
    pthread_exit(NULL);
}
