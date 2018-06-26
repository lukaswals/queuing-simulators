#include <math.h>               // Needed for log()

/*******************************************************************************
* Defined constants and variables
*******************************************************************************/
#define CPU 8086                // Determines implementation of ranf to use

// ranf() defines and variables. Extracted from course(EEE6462) material
#define A 16807L 		    /* multiplier (7**5) for ’ranf’ */
#define M 2147483647L 		/* modulus (2**31-1) for ’ranf’ */

static long SeedTable[16]= {0L, /* seeds for streams 1 thru 15 */
1973272912L, 747177549L, 20464843L, 640830765L, 1098742207L,
78126602L, 84743774L, 831312807L, 124667236L, 1172177002L,
1124933064L, 1223960546L, 1878892440L, 1449793615L, 553303732L};

static int strm=1;          /* index of current stream */

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
//double ranf(void);              // Generates random number (double type)
//double expntl(double x);        // Generate exponentially distributed RV

/*******************************************************************************
*       ranf()
********************************************************************************
* Function to generate a random double type number. It's implementation depends
* on the CPU architecture 
*******************************************************************************/
#if CPU==8086
/*
This implementation is for Intel 8086/8 and 80286/386 CPUs using
C compilers with 16-bit short integers and 32-bit long integers.
*/
double ranf(void)
{
    short *p,*q,k; long Hi,Lo;
    p=(short *)&SeedTable[strm]; Hi=*(p+1)*A; /* 16807*H->Hi */
    *(p+1)=0; Lo=SeedTable[strm]*A; /* 16807*L->Lo */
    p=(short *)&Lo; Hi+=*(p+1); /* add high-order bits of Lo to Hi */
    q=(short *)&Hi; /* low-order bits of Hi->Lo */
    *(p+1)=*q&0x7fff; /* clear sign bit */
    k=*(q+1)<<1; if (*q&0x8000) k++; /* Hi bits 31-45->K */
    /* form Z + K [- M] (where Z=Lo) : presubtract M to avoid overflow */
    Lo-=M; Lo+=k; if (Lo<0) Lo+=M;
    SeedTable[strm]=Lo;
    return((double)Lo*4.656612875E-10); /* Lo x 1/(2**31-1) */
}
#endif
#if CPU==680000
double ranf(void)
{
    short *p,*q,k; long Hi,Lo;
    p=(short *)&SeedTable[strm]; Hi=*(p)*A; /* 16807*H->Hi */
    *(p)=0; Lo=SeedTable[strm]*A; /* 16807*L->Lo */
    p=(short *)&Lo; Hi+=*(p); /* add high-order bits of Lo to Hi */
    q=(short *)&Hi; /* low-order bits of Hi->Lo */
    *(p)=*(q+1)&0x7fff; /* clear sign bit */
    k=*(q)<<1; if (*(q+1)&0x8000) k++; /* Hi bits 31-45->K */
    /* form Z + K [- M] (where Z=Lo) : presubtract M to avoid overflow */
    Lo-=M; Lo+=k; if (Lo<0) Lo+=M;
    SeedTable[strm]=Lo;
    return((double)Lo*4.656612875E-10); /* Lo x 1/(2**31-1) */
}
#endif

/*******************************************************************************
*       expntl(double mean)
********************************************************************************
* Function to generate exponentially distributed RVs using the inverse method
* - Input: mean (mean value of distribution)
*******************************************************************************/
double expntl(double mean)
{
    double z; // Uniform random number from 0 to 1

    // Pull a uniform RV (interval: 0 < z < 1) using rand()
    /*
    do
    {
        z = ((double) rand() / RAND_MAX);
    }
    while ((z == 0) || (z == 1));
    return(-mean * log(z));
    */
    return (-mean * log( ranf() ) );
}

