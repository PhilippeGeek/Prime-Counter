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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>

#include "headers/functions.h"

extern long total_counter;
extern long* primesArray;
extern sem_t* sem_counter;
extern sem_t* sem_threads;
extern sem_t* sem_primesArray;

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
int isPrime(const long NUMBER) {
    if (NUMBER <= 1) // Negatives, 0 and 1 are not prime.
        return 0;

    if (NUMBER == 2) // Two is the only even prime number.
        return 1;

    if (NUMBER % 2 == 0) // Others are not prime.
        return 0;

    long i;
    long square = (int) sqrt(NUMBER);
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
long getNumber(const char* nom, long min, long max) {
    int ok = 0;
    long n = 0;
    int checkMax = 1;
    if (max <= min) {
        checkMax = 0;
    }
    do {
        if (checkMax) {
            printf("Input %ld < %s < %ld : ", min - 1, nom, max + 1);
        }
        else {
            printf("Input %s > %ld : ", nom, min - 1);
        }
        ok = scanf("%ld", &n);
        emptyBuffer();
    } while (!ok || n < min || (checkMax && n > max));

    return n;
}

/**
 * arg should be an instance of threadData struct.
 * Counts the number of primes between start and stop.
 * Increments the total_counter after the end of range.
 */
void* getPrimeCount(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    long start = data->start;
    long stop = data->stop;
    long* startInArray = data->startInArray;
    sem_post(data->sem);
    long primesArrayLocal[CHUNK_SIZE / 2] = {-1};

    long i;
    int counter = 0;
    long* primesArrayLocalPtr;

    for (i = start, primesArrayLocalPtr = primesArrayLocal; i <= stop; ++i) {
        if (isPrime(i)) {
            *primesArrayLocalPtr = i;
            ++counter;
            ++primesArrayLocalPtr;
        }
    }

    sem_wait(sem_counter);
    total_counter += counter;
    sem_post(sem_counter);

    sem_wait(sem_primesArray);
    long* globalArrayPtr;
    const long* globalArrayEnd = startInArray + counter;
    for (globalArrayPtr = startInArray, primesArrayLocalPtr = primesArrayLocal; globalArrayPtr < globalArrayEnd; ++globalArrayPtr, ++primesArrayLocalPtr) {
        *globalArrayPtr = *primesArrayLocalPtr;
    }
    sem_post(sem_primesArray);

    sem_post(sem_threads);
    pthread_exit(NULL);
}

void printLongArray(const long* array, const int size) {
    const long* arrayPtr;
    const long* arrayEnd = array + size;
    int i;
    for (arrayPtr = array, i = 0; arrayPtr < arrayEnd; ++arrayPtr) {
        if (*arrayPtr != 0) {
            if (i == 0) {
                printf("[%12ld", *arrayPtr);
            }
            else {
                printf(" %12ld", *arrayPtr);
            }
            if ((i + 1) % 10 == 0) {
                printf("\n");
            }
            ++i;
        }
    }
    printf("]\n");
}
