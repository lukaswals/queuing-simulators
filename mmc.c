/*******************************************************************************
*                           M/M/c Queue Simulator
********************************************************************************
* Notes: There are some problems with the "Server Utilization" metric, resulting
* in negative values when simulation is run for a sufficiently long time
*------------------------------------------------------------------------------*
* Build Command:
* gcc -o mmc mmc.c -lm
*------------------------------------------------------------------------------*
* Execute command:
* ./mm10
*------------------------------------------------------------------------------*
* Author: Lucas German Wals Ochoa
*******************************************************************************/

/*******************************************************************************
* Includes
*******************************************************************************/
#include <stdio.h>              // Needed for printf()
#include <stdlib.h>             // Needed for exit() and rand()
#include <unistd.h>             // Needed for getopts()
#include <stdbool.h>            // Needed for bool type
#include "utils.h"              // Needed for expntl()

/*******************************************************************************
* Defined constants and variables
* NOTE: All TIME constants are defined in seconds!
*******************************************************************************/
#define SIM_TIME   1.0e9        // Simulation time
#define ARR_TIME   90.00        // Mean time between arrivals
#define SERV_TIME  60.00        // Mean service time
#define NUM_SERVERS  10         // Number of servers in the system

/*******************************************************************************
* Function Prototypes
*******************************************************************************/
static void show_usage(char *name);
int min_departure(double arr[], int capacity); // Find the index of the minimum departure time
int idle_server(double arr[], int capacity);  // Position in array to store cust. departure time

/*******************************************************************************
* Main Function
*******************************************************************************/
int main(int argc, char **argv)
{
    int opt;    // Hold the options passed as argument
    double endTime = SIM_TIME;        // Total time to do Simulation
    double arrTime = ARR_TIME;        // Mean time between arrivals
    double departTime = SERV_TIME;    // Mean service time
    int c = NUM_SERVERS;              // Number of servers in the system

    double time = 0.0;                  // Current Simulation time
    double nextArrival = 0.0;           // Time for next arrival
    double nextDeparture = SIM_TIME;    // Time for next departure
    
    int nextDepartIndex;                // Index of next departure time in array
    int arrayIndex = 0;                 // Auxiliar variable 
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
        while ( (opt = getopt(argc, argv, "a:d:s:c:")) != -1 )
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
                case 'c':
                    c = atoi(optarg);
                    break;
                default:    // '?' unknown option
                    show_usage( argv[0] );
            }
        }
    }

    double custDepartures[c]; // Departure times of serving customers
    for (int i=0; i < c; i++)
        custDepartures[i] = SIM_TIME;   // Fill the array with maximum time

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
            if (n <= c)
            {
                if (n == c)
                    lastBusyTime = time;    // Set "last start of busy time"
                
                arrayIndex = idle_server(custDepartures, c);
                custDepartures[arrayIndex] = time + expntl(departTime);
                if (n == 1)
                {
                    nextDepartIndex = arrayIndex;
                    nextDeparture = custDepartures[nextDepartIndex];    
                }
            } // end of if "n <= c"
            
        }
        // Departure occurred
        else
        {
            time = nextDeparture;
            s = s + n * (time - lastEventTime); // Update area under "s" curve
            n--;    // Customers in system decrease
            lastEventTime = time;   // "last event time" for next event
            departures++;           // Increment number of completions
            custDepartures[nextDepartIndex] = SIM_TIME; // Set server as empty
            if (n > 0)
            {               
                if (n == c-1) // Update busy time when at least one server idle
                    busyTime = busyTime + time - lastBusyTime;

                if (n >= c)   // Calculate departure of a waiting customer
                    custDepartures[nextDepartIndex] = time + expntl(departTime);
                // Look for the next departure time
                nextDepartIndex = min_departure(custDepartures, c);
                nextDeparture = custDepartures[nextDepartIndex];
            }
            else
                nextDeparture = SIM_TIME;
        } // end of departure event
    }

    // Compute outputs
    x = departures / time;  // Compute throughput rate
    u = busyTime / time;    // Compute server utilization
    l = s / time;             // Avg number of customers in the system
    w = l / x;              // Avg Sojourn time

    // Output results
    printf("<-------------------------------------------------------------> \n");
    printf("<           *** Results for M/M/%d simulation ***             > \n", c);
    printf("<-------------------------------------------------------------> \n");
    printf("-  INPUTS: \n");
    printf("-    Total simulation time        = %.4f sec \n", endTime);
    printf("-    Mean time between arrivals   = %.4f sec \n", arrTime);
    printf("-    Mean service time            = %.4f sec \n", departTime);
    printf("-    # of Servers in system       = %d servers \n", c);
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
*       min_departure(double arr[], int capacity)
********************************************************************************
* Function that return the index of the minimum departure time
* - Input: arr (array of departures)
* - Input: capacity (size of the array)
*******************************************************************************/
int min_departure(double arr[], int capacity)
{
    int index = 0;

    for (int i=1; i < capacity; i++)
    {
        if (arr[i] < arr[index])
            index = i;
    }
//    printf(" - INDEX = %d \n", index);
    return index;
}

/*******************************************************************************
*       idle_server(double arr[], int capacity)
********************************************************************************
* Function that return the index of an "idle server"
* It's used to determine to which position of the array we will save the
* departure time of the client (which server is serving the customer)
* - Input: arr (array of departures)
* - Input: capacity (size of the array)
*******************************************************************************/
int idle_server(double arr[], int capacity)
{
    int index = 0;
    bool idle = false;

    for (int i=1; (i < capacity && !idle); i++)
    {
        if (arr[i] == SIM_TIME)
        {
            index = i;
            idle = true;
        }
    }
//    printf(" - INDEX = %d \n", index);
    return index;
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
    printf("\t-c\tNumber of servers in the system\n");
    exit(EXIT_SUCCESS);
}
