/*******************************************************************************
*                           M/M/1 Queue Simulator
********************************************************************************
* Notes: UNDER CONSTRUCTION
*------------------------------------------------------------------------------*
* Build Command:
* gcc -o mm1 mm1.c -lm
*------------------------------------------------------------------------------*
* Execute command:
* ./mm1
*------------------------------------------------------------------------------*
* Author: Lucas German Wals Ochoa
*******************************************************************************/

/*******************************************************************************
* Includes
*******************************************************************************/
#include <stdio.h>              // Needed for printf()
#include <stdlib.h>             // Needed for exit() and rand()
#include <unistd.h>             // Needed for getopts()
#include "utils.h"              // Needed for expntl()

/*******************************************************************************
* Defined constants and variables
* NOTE: All TIME constants are defined in seconds!
*******************************************************************************/
#define SIM_TIME   1.0e9        // Simulation time
#define ARR_TIME   90.00        // Mean time between arrivals
#define SERV_TIME  60.00        // Mean service time

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
static void show_usage(char *name);

/*******************************************************************************
* Main Function
*******************************************************************************/
int main(int argc, char **argv)
{
    int opt;    // Hold the options passed as argument
    double endTime = SIM_TIME;        // Total time to do Simulation
    double arrTime = ARR_TIME;        // Mean time between arrivals
    double departTime = SERV_TIME;    // Mean service time

    double time = 0.0;          // Current Simulation time
    double nextArrival = 0.0;         // Time for next arrival
    double nextDeparture = SIM_TIME;  // Time for next departure
    unsigned int n = 0;           // Actual number of customers in the system

    unsigned int departures = 0;  // Total number of customers served
    double busyTime = 0.0;        // Total busy time
    double s = 0.0;               // Area of number of customers in system
    double lastEventTime = time;  // Variable for "last event time"
    double lastBusyTime;          // Variable for "last start of busy time"
    double x;     // Throughput rate
    double u;     // Utilization of system
    double l;     // Average number of customers in system
    double w;     // Average Sojourn time

    if (argc > 1)
    {     
        while ( (opt = getopt(argc, argv, "a:d:s:")) != -1 )
        {
            switch (opt) {
                case 'a':
                    arrTime = atof(optarg);
                    break;
                case 'd':
                    departTime = atof(optarg);
                    break;
                case 's':
                    endTime = atof(optarg);
                    break;
                default:    // '?' unknown option
                    show_usage( argv[0] );
            }
        }
    }

    // Simulation loop
    while (time < endTime)
    {
        // Arrival occurred
        if (nextArrival < nextDeparture)
        {
            time = nextArrival;
            s = s + n * (time - lastEventTime);  // Update area under "s" curve
            n++;    // Customers in system increase
            lastEventTime = time;   // "last event time" for next event
            nextArrival = time + expntl(arrTime);
            if (n == 1) // System is full, only have 1 server
            {
                lastBusyTime = time;    // Set "last start of busy time"
                nextDeparture = time + expntl(departTime);
            }
        }
        // Departure occurred
        else
        {
            time = nextDeparture;
            s = s + n * (time - lastEventTime); // Update area under "s" curve
            n--;    // Customers in system decrease
            lastEventTime = time;   // "last event time" for next event
            departures++;           // Increment number of completions
            if (n > 0)
                nextDeparture = time + expntl(departTime);
            else
            {
                nextDeparture = SIM_TIME;
                // Update busy time sum when no customers
                busyTime = busyTime + time - lastBusyTime;
            }
        }
    }

    // Compute outputs
    x = departures / time;  // Compute throughput rate
    u = busyTime / time;    // Compute server utilization
    l = s / time;           // Avg number of customers in the system
    w = l / x;              // Avg Sojourn time

    // Output results
    printf("<-------------------------------------------------------------> \n");
    printf("<            *** Results for M/M/1 simulation ***             > \n");
    printf("<-------------------------------------------------------------> \n");
    printf("-  INPUTS: \n");
    printf("-    Total simulation time        = %.4f sec \n", endTime);
    printf("-    Mean time between arrivals   = %.4f sec \n", arrTime);
    printf("-    Mean service time            = %.4f sec \n", departTime);
    printf("<-------------------------------------------------------------> \n");
    printf("-  OUTPUTS: \n");
    printf("-    # of Customers served        = %u cust \n", departures);
    printf("-    Throughput rate              = %f cust/sec \n", x);
    printf("-    Server utilization           = %f %% \n", 100.0 * u);
    printf("-    Avg # of cust. in system     = %f cust \n", l);
    printf("-    Mean Sojourn time            = %f sec \n", w);
    printf("<-------------------------------------------------------------> \n");
}

/*******************************************************************************
*       show_usage(char *name)
********************************************************************************
* Function that return a message of how to use this program
* - Input: name (the name of the executable)
*******************************************************************************/
static void show_usage(char *name)
{
    printf("\nUsage: \n");
    printf("%s [option] value \n", name);
    printf("\n");
    printf("Options: \n");
    printf("\t-a\tMean time between arrivals (in seconds) \n");
    printf("\t-d\tMean service time (in seconds) \n");
    printf("\t-s\tTotal simulation time (in seconds) \n");
    exit(EXIT_SUCCESS);
}

