#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define MAX_NUMBER_OF_RECORDS (20)
#define MAX_RECORD_LABEL_SIZE (10)

typedef struct
{
    char    label[MAX_RECORD_LABEL_SIZE]; // human readable label that allows identifying the record
    uint8_t weight;                       // number of copies of the record in a set of records
    uint8_t frequency;                    // number of times the record is picked randomly
} record_t;

inline void InitializeRandomNumberGenerator()
{
    srand(time(0));
}

inline void PrepareArrayOfCumulatedRecordWeights(
    int n,       // number of records
    record_t *r, // array of records
    uint16_t *w  // array of cumulated record weights
)
{
    // r and w are assumed to be != 0
    int i;
    for(i = 0; i < n; i++) {
        w[i] = r[i].weight;
        if(i != 0) {
            w[i] += w[i - 1];
        }
    }
}

inline unsigned int GenerateRandomNumber(
    unsigned int max // max. value of the random number
)
{
    return(rand() % max); // TODO: review since it is not distributed uniformly
    // x = (rand() + 1) * n / (RAND_MAX + 1)
}

int main() // TODO: add arguments 1. input file name and 2. number of iterations
{
    //int m;     // number of iterations, got as a program's input
    int n = 7; // number of records (>= 1), to be read from the input file
    record_t dataset[MAX_NUMBER_OF_RECORDS] = {
        {"red",    5, 0},
        {"orange", 3, 0},
        {"yellow", 2, 0},
        {"green",  2, 0},
        {"blue",   5, 0},
        {"indigo", 6, 0},
        {"violet", 4, 0}
    }; // array of records, to be read from the input file
    uint16_t cumWeights[MAX_NUMBER_OF_RECORDS]; // array of cumulated record weights
    
    InitializeRandomNumberGenerator();
    PrepareArrayOfCumulatedRecordWeights(n, dataset, cumWeights);
    
    int i;
    for(i = 0; i < n; i++) {
        printf("%d\n", cumWeights[i]);
    }
    return 0;
}

