#include <stdio.h>
#include <stdlib.h>
#include <time.h>

inline void InitializeRandomNumberGenerator()
{
    srand(time(0));
}

inline unsigned int GenerateRandomNumber(unsigned int max)
{
    return(rand() % max); // TODO: review since it is not distributed uniformly
    // x = (rand() + 1) * n / (RAND_MAX + 1)
}

int main()
{
    int i;
    InitializeRandomNumberGenerator();
    for(i = 0; i < 5; i++) {
        printf("%d\n", GenerateRandomNumber(5));
    }
    return 0;
}

