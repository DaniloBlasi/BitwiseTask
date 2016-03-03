// inclusions
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

// definition of macros
#define MAX_NUMBER_OF_RECORDS (20)
#define MAX_RECORD_LABEL_SIZE (10)

// definition of data types
typedef struct
{
    char    label[MAX_RECORD_LABEL_SIZE]; // human readable label that allows identifying the record
    uint8_t weight;                       // number of copies of the record in a set of records
    uint8_t frequency;                    // number of times the record is picked randomly
} record_t;

// declaration of function prototypes
static inline void InitializeRandomNumberGenerator();
static inline unsigned int GenerateRandomNumberLowerThan(unsigned int max);
static inline void PrepareArrayOfCumulatedRecordWeights(int n, record_t *r, uint16_t *w);
static inline unsigned int PickRandomRecordIndex(int n,  uint16_t *w);

// definition of local functions called from a single point
static inline void InitializeRandomNumberGenerator()
{
    srand(time(0));
}

static inline unsigned int GenerateRandomNumberLowerThan(
    unsigned int n // one above the maximum number to be randomly generated
)
{
    return(rand() % n); // TODO: review since it is not distributed uniformly
    // note: rand() ranges in [0, RAND_MAX] and
    // (RAND_MAX + 1) is not necessarily a multiple of n
}

static inline void PrepareArrayOfCumulatedRecordWeights(
    int n,       // number of records
    record_t *r, // array of records; assumed to be !=0
    uint16_t *w  // array of cumulated record weights; assumed to be !=0
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

static inline unsigned int PickRandomRecordIndex(
    int n,       // number of records; assumed to be >0
    uint16_t *w  // array of cumulated record weights; assumed to be !=0
)
{
    // declaration of local variables
    unsigned int r; // random number
    unsigned int picked; // flag indicating if a record has been picked
    unsigned int i_min, i_max, i_mid; // array indexes
    
    // generate a random number between 1 and the sum of all the record weights
    r = 1 + GenerateRandomNumberLowerThan(w[n - 1]);
    printf("r %d\n", r);
    
    // pick the record index corresponding to the random number (binary search)
    picked = 0;
    i_min = 0;
    i_max = n - 1;
    i_mid = 0; // if n=0
    do {
        picked = (i_min > i_max);
        if(!picked) {
            i_mid = (i_min + i_max) >> 1;
            if(r <= w[i_mid]) {
                i_max = i_mid - 1;
            }
            else {
                i_min = i_mid + 1;
            }
        }
    } while(!picked);
    
    return i_mid;
}

// definition of the main function
int main() // TODO: add arguments 1. input file name and 2. number of iterations
{
    // declaration of local variables
    unsigned int m = 1; // number of iterations, got as a program's input
    unsigned int n = 7; // number of records (>= 1), to be read from the input file
    unsigned int i; // array indexes
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
    
    // initialization stuff
    InitializeRandomNumberGenerator();
    PrepareArrayOfCumulatedRecordWeights(n, dataset, cumWeights);
    
    // loop up to the given number of iterations,
    // picking a random record and incrementing the related frequency
    for(i = 0; i < m; i++) {
        dataset[PickRandomRecordIndex(n, cumWeights)].frequency++;
    }
    for(i = 0; i < n; i++) {
        printf("w[%d]=%d, f[%d]=%d\n", i, cumWeights[i], i, dataset[i].frequency);
    }
    
    return 0;
}

