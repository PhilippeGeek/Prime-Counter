# Prime counter
This prime counter uses 8 threads in parallel to compute the number of primes lower or equal to the number given by the user.

It then prints the elapsed time of the computing.

## Compilation
### GCC
Simply use GCC to compile this one-file program : `gcc -o prime.out src/prime.c`

### CMake
You can also use CMake. You need CMake v.2.6 or greater.
```
cmake -G 'Unix Makefiles'
make
```

## Usage
Just run the file : `./prime.out` (when in the same folder).

