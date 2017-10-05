/* External definitions for single-server queueing system. */
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include "simlib/src/simlib.h" /*Required for use of silib.c */
//#include "lcgrand.h"  /* Header file for random-number generator. */
using namespace std;

#define Q_LIMIT 100  /* Limit on queue length. */
#define BUSY      1  /* Mnemonics for server's being busy */
#define IDLE      0  /* and idle. */
#define EVENT_ARRIVAL 1 /*Event tyoe for arrival*/
#define EVENT_DEPARTURE 2 /*Event type for departure*/
#define LIST_QUEUE 1 /*List number for queue*/
#define LIST_SERVER 2 /*List number for server*/
#define SAMPST_DELAYS 1 /*sampst variable for delays in queue*/
#define STREAM_INTERARRIVAL 1 /*Random-number stream for interarrivals*/
#define STREAM_SERVICE 2 /*Random0number stream for service times*/

int   next_event_type, num_custs_delayed, num_delays_required, num_events,
      num_in_q, server_status, maxatr;
float area_num_in_q, area_server_status, mean_interarrival, mean_service,
      sim_time, time_arrival[Q_LIMIT + 1], time_last_event, time_next_event[3],
      total_of_delays;

void  initialize(void);
void init_model(void);
void  timing(void);
void  arrive(void);
void  depart(void);
void  report(void);
void  arrive1(void);
void  depart1(void);
void  report1(void);
void  update_time_avg_stats(void);
//float expon(float mean);

ifstream input("input.txt");
ofstream output("output3.txt");    


int main(){  /* Main function. */

    /* Open input and output files. */

    /* Specify the number of events for the timing function. */
    num_events = 2;

    /* Read input parameters. */
    input>>mean_interarrival>>mean_service>>num_delays_required;

    
    /* Write report heading and input parameters. */
    output<<"Single-server queueing system\n\n";
    output<<"Mean interarrival time "<< mean_interarrival<<" minutes\n\n";
    output<<"Mean service time "<< mean_service<<" minutes\n\n";
    output<<"Number of customers "<< num_delays_required<<" \n\n";

    /* Initialize the simulation. */
    //initialize();
    init_simlib();

    maxatr = 4;

    init_model(); //Initialize the model

    /* Run the simulation while more delays are still needed. */
    while (num_custs_delayed < num_delays_required) {

        /* Determine the next event. */
        timing();

        /* Update time-average statistical accumulators. */
        //update_time_avg_stats();

        /* Invoke the appropriate event function. */
        switch (next_event_type) {
            case EVENT_ARRIVAL:
                arrive();
                break;
            case EVENT_DEPARTURE:
                depart();
                break;
        }
    }

    /* Invoke the report generator and end the simulation. */
    report();

    input.close();
    output.close();

    return 0;
}

void init_model(){
    num_custs_delayed = 0;
    event_schedule(sim_time + expon(mean_interarrival,
     STREAM_INTERARRIVAL), EVENT_ARRIVAL);
}


void initialize(void){  /* Initialization function. */

    /* Initialize the simulation clock. */
    sim_time = 0.0;

    /* Initialize the state variables. */
    server_status   = IDLE;
    num_in_q        = 0;
    time_last_event = 0.0;

    /* Initialize the statistical counters. */
    num_custs_delayed  = 0;
    total_of_delays    = 0.0;
    area_num_in_q      = 0.0;
    area_server_status = 0.0;

    /* Initialize event list.  Since no customers are present, the departure
       (service completion) event is eliminated from consideration. */
    time_next_event[1] = sim_time + expon(mean_interarrival);
    time_next_event[2] = 1.0e+30;
}


void timing(void){  /* Timing function. */

    int   i;
    float min_time_next_event = 1.0e+29;

    next_event_type = 0;

    /* Determine the event type of the next event to occur. */
    for (i = 1; i <= num_events; ++i)
        if (time_next_event[i] < min_time_next_event) {
            min_time_next_event = time_next_event[i];
            next_event_type     = i;
        }

    /* Check to see whether the event list is empty. */
    if (next_event_type == 0) {

        /* The event list is empty, so stop the simulation. */
        output<<"\nEvent list empty at time " <<sim_time <<" \n";
        exit(1);
    }

    /* The event list is not empty, so advance the simulation clock. */
    sim_time = min_time_next_event;
}


void arrive(void){
    event_schedule(sim_time + expon(mean_interarrival,
     STREAM_INTERARRIVAL), EVENT_ARRIVAL);
    if (list_size[LIST_SERVER] == 1){
        transfer[1] = sim_time;
        list_file(LAST, LIST_QUEUE);
    }
    else{
        sampst(0.0, SAMPST_DELAYS);
        ++num_custs_delayed;
        list_file(FIRST, LIST_SERVER);
        event_schedule(sim_time+expon(mean_service, 
            STREAM_SERVICE), EVENT_DEPARTURE);
    }
}

void arrive1(void){  /* Arrival event function. */

    float delay;

    /* Schedule next arrival. */
    time_next_event[1] = sim_time + expon(mean_interarrival);

    /* Check to see whether server is busy. */
    if (server_status == BUSY) {

        /* Server is busy, so increment number of customers in queue. */
        ++num_in_q;

        /* Check to see whether an overflow condition exists. */
        if (num_in_q > Q_LIMIT) {

            /* The queue has overflowed, so stop the simulation. */
           output<<"\nOverflow of the array time_arrival at";
           output<<" time "<<sim_time<<" \n";
            exit(2);
        }

        /* There is still room in the queue, so store the time of arrival of the
           arriving customer at the (new) end of time_arrival. */
        time_arrival[num_in_q] = sim_time;
    }

    else {

        /* Server is idle, so arriving customer has a delay of zero.  (The
           following two statements are for program clarity and do not affect
           the results of the simulation.) */
        delay            = 0.0;
        total_of_delays += delay;

        /* Increment the number of customers delayed, and make server busy. */
        ++num_custs_delayed;
        server_status = BUSY;

        /* Schedule a departure (service completion). */
        time_next_event[2] = sim_time + expon(mean_service);
    }
}

void depart(void){
    if (list_size[LIST_QUEUE] == 0){
        list_remove(FIRST, LIST_SERVER);
    }
    else{
        list_remove(FIRST, LIST_QUEUE);
        sampst(sim_time-transfer[1], SAMPST_DELAYS);
        ++num_custs_delayed;
        event_schedule(sim_time+expon(mean_service, 
            STREAM_SERVICE), EVENT_DEPARTURE);
    }
}

void depart1(void){  /* Departure event function. */

    int   i;
    float delay;

    /* Check to see whether the queue is empty. */
    if (num_in_q == 0) {

        /* The queue is empty so make the server idle and eliminate the
           departure (service completion) event from consideration. */
        server_status      = IDLE;
        time_next_event[2] = 1.0e+30;
    }

    else {

        /* The queue is nonempty, so decrement the number of customers in
           queue. */
        --num_in_q;

        /* Compute the delay of the customer who is beginning service and update
           the total delay accumulator. */
        delay            = sim_time - time_arrival[1];
        total_of_delays += delay;

        /* Increment the number of customers delayed, and schedule departure. */
        ++num_custs_delayed;
        time_next_event[2] = sim_time + expon(mean_service);

        /* Move each customer in queue (if any) up one place. */
        for (i = 1; i <= num_in_q; ++i)
            time_arrival[i] = time_arrival[i + 1];
    }
}

void report(void){
    output<<"Delays in queue, in minutes:\n";
    out_sampst(output, SAMPST_DELAYS, SAMPST_DELAYS);
    output<<"\n Queue length (1) and server utilization(2):\n";
    out_filest(output, LIST_QUEUE, LIST_SERVER);
    output<<"Time simulation ended: " <<sim_time<<" minutes\n";
}

void report1(void){  /* Report generator function. */

    /* Compute and write estimates of desired measures of performance. */
    output<<"\n\nAverage delay in queue "<< total_of_delays/num_custs_delayed<<" minutes\n\n";
    output<<"Average number in queue "<<area_num_in_q/sim_time<<" \n\n";
    output<<"Server utilization "<< area_server_status/sim_time<<" \n\n";
    output<<"Time simulation ended "<<sim_time<<" minutes";
}


void update_time_avg_stats(void){  /* Update area accumulators for time-average
                                     statistics. */

    float time_since_last_event;

    /* Compute time since last event, and update last-event-time marker. */
    time_since_last_event = sim_time - time_last_event;
    time_last_event       = sim_time;

    /* Update area under number-in-queue function. */
    area_num_in_q      += num_in_q * time_since_last_event;

    /* Update area under server-busy indicator function. */
    area_server_status += server_status * time_since_last_event;
}

/*float expon(float mean){ 
    return -mean * log(lcgrand(1));
}*/



