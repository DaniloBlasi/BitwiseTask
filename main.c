// inclusions
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

// definition of macros
#define MAX_NUMBER_OF_RECORDS (20)
#define MAX_RECORD_LABEL_SIZE (10)

#ifdef DEBUG
    #define ASSERT(COND)\
        if(!(COND)) {\
            printf("Condition '" #COND "' is not met at line %u.\n", __LINE__);\
            while(1);\
        }
#else
    #define ASSERT(COND)
#endif

// definition of data types
typedef struct __attribute__((packed))
{
    char         label[MAX_RECORD_LABEL_SIZE + 1]; // human readable label that allows identifying the record
                                                   // (includes the final null character)
    unsigned int weight;                           // number of copies of the record in a set of records
    unsigned int frequency;                        // number of times the record is picked randomly
} record_t;

// declaration of function prototypes
static inline int ParseArguments(unsigned int argc, char **argv, char **fname, unsigned int *iter);
static inline int ReadFile(char *fname, unsigned int *recs, record_t *recset);
static inline void InitializeRandomNumberGenerator();
static inline unsigned int MostSignificantSetBit(unsigned int word);
static inline unsigned int GenerateRandomNumberLowerThan(unsigned int max);
static inline void PrepareArrayOfCumulatedRecordWeights(unsigned int n, record_t *r, unsigned int *w);
static inline unsigned int PickRandomRecordIndex(unsigned int n, unsigned int *w);

// definition of local functions called from a single point

// parses the arguments passed to this program;
// returns -1 if something goes wrongly, 0 otherwise
static inline int ParseArguments(
    unsigned int argc, // count of arguments
    char **argv, // argument (string) list
    char **fname, // input file name, returned from parsing of the first argument
    unsigned int *iter // number of iterations, returned from parsing of the second argument
)
{
    // i/o parameters validity check (*)
    ASSERT(argv != 0);
    ASSERT(fname != 0);
    ASSERT(iter != 0);
        
    // parse arguments
    if(argc < 3) {
        printf("Incorrect number of arguments.\nPlease specify <file_name> and <number_of_iterations>.\n");
        return(-1);
    }
    
    // (*)
    ASSERT(argv[1] != 0);
    ASSERT(argv[2] != 0);
    
    *fname = argv[1];
    *iter  = atoi(argv[2]);
    
    return 0;
}

// reads a file where a list of records is provided;
// each record shall be written according to the following format: "<label> <weight>";
// returns -1 if something with the reading goes wrongly, 0 otherwise;
// the reading is truncated when the MAX_NUMBER_OF_RECORDS is read;
// records with a null weight are skipped
static inline int ReadFile(
    char *fname, // name of the input file with the list of records
    unsigned int *recs, // number of read records
    record_t *recset // list of read records
)
{
    // i/o parameters validity check
    ASSERT(fname != 0);
    ASSERT(recs != 0);
    ASSERT(recset != 0);
    
    // declaration of local variables
    FILE *fstream; // input file stream
    
    // read the input file
    fstream = fopen(fname, "r");
    if(fstream == NULL) {
        printf("Cannot read the file '%s'.\nPlease verify the path name.\n", fname);
        return(-1);
    }
    *recs = 0;
    while(
        (*recs < MAX_NUMBER_OF_RECORDS) &&
        (fscanf(fstream, "%s %u", recset[*recs].label, &(recset[*recs].weight)) != EOF)
    ) {
        // accept only records with a positive weight
        if(recset[*recs].weight > 0) {
            recset[*recs].frequency = 0;
            *recs += 1;
        }
    }
    fclose(fstream);
    
    return 0;
}

// initializes the generator of pseudo-random numbers with the current time
static inline void InitializeRandomNumberGenerator()
{
    srand(time(0));
}

// returns the MSb set in a word, in the range [1, 32];
// if 0 is returned, then the word is equal to 0
static inline unsigned int MostSignificantSetBit(
    unsigned int word // reference word
)
{
    // declaration of local variables
    unsigned int bit;
    
    // get the MSSB of the word, if it exists, in the range [1, 32]
    for(bit = 32; (bit != 0) && ((word & (0x1 << (bit - 1))) == 0); bit--);
    return(bit);
}

// generates a positive random number, strictly lower than a given number
static inline unsigned int GenerateRandomNumberLowerThan(
    unsigned int n // one above the maximum number to be randomly generated
)
{
    // i/o parameters validity check
    ASSERT(n > 0);
    
    // declaration of local variables
    static unsigned int maxRandMssb = 0;
    uint64_t rand64;
    
    // this allows setting maxRandMssb once for all
    if(maxRandMssb == 0) {
        maxRandMssb = MostSignificantSetBit(RAND_MAX);
    }
    
    // generate a random number lower than the given input limit
    // keeping the uniform distribution
    rand64 = rand();
    rand64 *= n;
    rand64 >>= maxRandMssb; // divided by (RAND_MAX + 1)
    return((unsigned int)rand64);
}

// fills in an array of cumulated record weights
static inline void PrepareArrayOfCumulatedRecordWeights(
    unsigned int n, // number of records
    record_t *r,    // array of records
    unsigned int *w     // array of cumulated record weights
)
{
    // i/o parameters validity check
    ASSERT(n > 0);
    ASSERT(r != 0);
    ASSERT(w != 0);
    
    // declaration of local variables
    unsigned int i;
    
    // fill in the array of cumulated record weights
    for(i = 0; i < n; i++) {
        w[i] = r[i].weight;
        if(i != 0) {
            w[i] += w[i - 1];
        }
    }
}

// picks a random index of an array of records,
// given an associated array of cumulated record weights
static inline unsigned int PickRandomRecordIndex(
    unsigned int n, // number of records
    unsigned int *w     // array of cumulated record weights
)
{
    // i/o parameters validity check
    ASSERT(n > 0);
    ASSERT(w != 0);
    
    // declaration of local variables
    unsigned int r; // random number
    unsigned int i_min, i_max, i_mid; // array indexes
    
    // generate a random number between 1 and the sum of all the record weights
    r = 1 + GenerateRandomNumberLowerThan(w[n - 1]);
    
    // pick the record index corresponding to the random number (like binary search)
    i_min = 0;
    i_max = n - 1;
    do {
        i_mid = (i_min + i_max) >> 1;
        if(r <= w[i_mid]) {
            // check the lower limit, to avoid segmentation fault
            if(i_mid == 0) {
                break;
            }
            i_max = i_mid - 1;
        }
        else {
            // check the higher limit, to avoid segmentation fault
            if(i_mid == (n - 1)) {
                break;
            }
            i_min = i_mid + 1;
        }
    } while(r <= w[i_max]);
    
    return i_mid;
}

// definition of the main function
int main(int argc, char **argv) // TODO: add arguments 1. input file name and 2. number of iterations
{
    // declaration of local variables
    char *filename; // name of the input file, which the records have to be read from
    unsigned int iterations; // number of iterations, got as a program's input
    unsigned int records; // number of records (>= 1), to be read from the input file
    unsigned int i; // array indexes
    record_t recordset[MAX_NUMBER_OF_RECORDS]; // array of records, to be read from the input file
    unsigned int cumWeights[MAX_NUMBER_OF_RECORDS]; // array of cumulated record weights
    
    if(ParseArguments(argc, argv, &filename, &iterations) < 0) {
        return(-1);
    }
    if(ReadFile(filename, &records, recordset) < 0) {
        return(-1);
    }
    
    // initialization stuff
    InitializeRandomNumberGenerator();
    PrepareArrayOfCumulatedRecordWeights(records, recordset, cumWeights);
    
    // loop up to the given number of iterations,
    // picking a random record and incrementing the related frequency
    for(i = 0; i < iterations; i++) {
        recordset[PickRandomRecordIndex(records, cumWeights)].frequency++;
    }
    
    // show the statistics on the stdout
    for(i = 0; i < records; i++) {
        printf(
            "%s:\n- prob. %3u%%, freq. %3u%%\n",
            recordset[i].label,
            100 * recordset[i].weight / cumWeights[records - 1],
            100 * recordset[i].frequency / iterations);
    }
    
    return 0;
}

