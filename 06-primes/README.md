Write a parallel  program that finds all prime numbers in a given range of values, passed as parameters of the command line. 
In order to check if a number is prime, please use the following code: 

// see http://en.wikipedia.org/wiki/Primality_test

static bool is_prime(ull n) {
   if (n <= 3) return n > 1; // 1 is not prime !
   if (n % 2 == 0 || n % 3 == 0) return false; 
  for (ull i = 5; i * i <= n; i += 6) {
      if (n % i == 0 || n % (i + 2) == 0) 
         return false;
  }
  return true;
}


Consider your favorite parallel programming framework among the ones we've seen so far (C++ native threads, OpenMP, GrPPI) 
and possibly implement more that one version (e.g. a native C++ and an OpenMP version) comparing the relative performances.
