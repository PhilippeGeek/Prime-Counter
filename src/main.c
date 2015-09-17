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
#include <string.h>
#include <unistd.h> // fork(2),getpid(2)
#include <stdio.h> // perror(3),printf(3)
#include <sys/types.h> // getpid(2),wait(2)
#include <sys/wait.h> // wait(2)

#define NMAX 10000000 // Max is set for output reasons. We display output on 7 characters.

void emptyBuffer() {
    while (getchar() != '\n') ;
}

int isPrime(const unsigned int NUMBER) {
    if (NUMBER <= 1) // Negatives, 0 and 1 are not prime.
        return 0;

    if (NUMBER == 2) // Two is the only even prime number.
        return 1;

    if (NUMBER % 2 == 0) // Others are not prime.
        return 0;

    unsigned int i;
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
    const int USER_NUMBER = getNumber("n (max)");
    int p;
    pid_t pid = getpid();
    const int NUMBER_OF_THREADS = 4;
    unsigned int total_counter = 0;
    int state = 0;

    // TODO: Do it with threads and not forks
    for (p = 0; p < NUMBER_OF_THREADS; ++p) {
        if (pid != 0) {
            pid = fork();
            if(pid == 0) {
                unsigned int i;
                unsigned int counter = 0;
                for (i = 2; i <= USER_NUMBER; ++i) {
                    if (isPrime(i)) {
                        ++counter;
                    }
                }
                printf("Exiting with state : %d\n", counter);
                exit(counter);
            } else {
                wait(&state);
                printf("The returned state is : %d\n", state);
                total_counter += state;
            }
        }
    }

    printf("\n%d found under %d (included).\n", total_counter, USER_NUMBER);

    return 0;
}
