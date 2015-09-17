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

#define NMAX 10000000 // Max is set for output reasons. We display output on 7 characters.
#define MAX_THREADS 8

struct threadData {
    int start;
    int stop;
    int threadNum;
    sem_t sem;
};

void* getInt(void* userNumber);
void emptyBuffer();
int isPrime(const int NUMBER);
int getNumber(const char* nom);

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

int getNumber(const char* nom) {
    int ok = 0;
    int n = 0;
    do {
        printf("Input %s < %d : ", nom, NMAX);
        ok = scanf("%d", &n);
        emptyBuffer();
    } while (!ok || n >= NMAX || n < 0);

    return n;
}

int main(int argc, const char *argv[]) {
    int userNumber = getNumber("n (max)");
    pthread_t threads[MAX_THREADS];
    int i;
    int start = 2;

    for (i = 0; i < MAX_THREADS; ++i) {
        struct threadData data;
        data.start = start;
        data.threadNum = i;
        if (i != MAX_THREADS - 1) {
            data.stop = start + (userNumber - start) / 2;
        } else {
            data.stop = userNumber;
            if (data.start > userNumber) {
                data.start = userNumber;
            }
        }
        sem_init(&data.sem, 0, 0);
        printf("CREATING THREAD %d - start : %d - stop : %d (%d)\n", i, data.start, data.stop, data.stop - data.start);
        pthread_create(&(threads[i]), NULL, getInt, &data);
        sem_wait(&data.sem);
        start = data.stop + 1;
    }

    int total_counter = 0;

    for (i = 0; i < MAX_THREADS; ++i) {
        void* res;
        pthread_join(threads[i], &res);
        printf("res %d : %d\n", i, (int) res);
        total_counter += (int) res;
    }

    printf("\n%d found under %d (included).\n", total_counter, userNumber);

    return 0;
}

void* getInt(void* arg) {
    struct threadData* data = arg;
    int threadNum = data->threadNum;
    int start = data->start;
    int stop = data->stop;
    sem_post(&data->sem);

    int i;
    int counter = 0;

    printf("INSIDE THREAD %d - start : %d - stop : %d\n", threadNum, start, stop);
    for (i = start; i <= stop; ++i) {
        if (isPrime(i)) {
            ++counter;
        }
    }

    printf("EXITING THREAD %d with %d\n", threadNum, counter);
    pthread_exit((void*) counter);
}
