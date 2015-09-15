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

#include <stdio.h>
#include <math.h>

#define NMAX 2000001

void emptyBuffer() {
    while (getchar() != '\n') ;
}

int isPrime(const unsigned int NUMBER) {
    if (NUMBER <= 1)
        return 0;

    if (NUMBER == 2)
        return 1;

    if (NUMBER % 2 == 0)
        return 0;

    unsigned int i;
    int square = (int) sqrt(NUMBER);
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
    unsigned int found = 0;
    unsigned int i;
    unsigned int counter = 0;
    for (i = 2; i <= USER_NUMBER; ++i) {
        if (isPrime(i)) {
            if (!found) {
                printf("List of prime numbers under %d (included):\n", USER_NUMBER);
            }
            printf("%7d ", i);
            if ((counter + 1) % 10 == 0) {
                printf("\n");
            }
            found = 1;
            ++counter;
        }
    }

    if (!found) {
        printf("No prime number found under %d (included)", USER_NUMBER);
    }

    return 0;
}
