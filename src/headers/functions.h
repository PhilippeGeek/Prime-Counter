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

#ifndef _FUNCTIONS_
#define _FUNCTIONS_

#include <semaphore.h>

#define CHUNK_SIZE 10000

// Data passed to thread
typedef struct ThreadDataStruct {
    long start;
    long stop;
    long* startInArray;
    sem_t* sem;
} ThreadData;

// Functions declaration
void* getPrimeCount(void* arg);
int getUserOkay();
void emptyBuffer();
int isPrime(const long NUMBER);
long getNumber(const char* nom, long min, long max);
void printLongArray(const long* array, const int size);

#endif
