# Prime counter
This prime counter uses multiple threads in parallel to compute the number of primes lower or equal to the number given by the user.

It then prints the elapsed time of the computing.

## Compilation
### GCC
Simply use GCC to compile this one-file program : `gcc -o prime.out src/prime.c`

### CMake
You can also use CMake. You need CMake v.2.6 or greater.
```console
cmake CMakeLists.txt
make
```

## Usage
### Simple (guided)
Just run the file : `./prime.out` (when in the same folder).  
CMake outputs in `./build` folder.

### Advanced (passing arguments)
```console
prime.out [<number to search for primes under> [<number of threads>]]
```
You can pass one or two arguments to the executable :

1. If only one argument is passed to the program, it will assume that it's the number to search for primes under. The number of threads will be asked by the program.
2. If two arguments are passed, then the first is the number to search for primes under, the second is the maximum number of threads the program should run simultaneously. Choose this second number wisely as it can greatly improve performances or decrease them drastically !
