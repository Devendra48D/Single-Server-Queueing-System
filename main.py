import math
import sys

MODLUS = 2147483647
MULT1 =    24112
MULT2  =     26143

Q_LIMIT = 100 #Queue length limit
BUSY = 1 #busy int
IDLE = 0 #idle int
E = 2.7182818284590452353602874 # e constant to find natural log

zrng = [1,
1973272912, 281629770,  20006270,1280689831,2096730329,1933576050,
  913566091, 246780520,1363774876, 604901985,1511192140,1259851944,
  824064364, 150493284, 242708531,  75253171,1964472944,1202299975,
  233217322,1911216000, 726370533, 403498145, 993232223,1103205531,
  762430696,1922803170,1385516923,  76271663, 413682397, 726466604,
  336157058,1432650381,1120463904, 595778810, 877722890,1046574445,
   68911991,2088367019, 748545416, 622401386,2122378830, 640690903,
 1774806513,2132545692,2079249579,  78130110, 852776735,1187867272,
 1351423507,1645973084,1997049139, 922510944,2045512870, 898585771,
  243649545,1004818771, 773686062, 403188473, 372279877,1901633463,
  498067494,2087759558, 493157915, 597104727,1530940798,1814496276,
  536444882,1663153658, 855503735,  67784357,1432404475, 619691088,
  119025595, 880802310, 176192644,1116780070, 277854671,1366580350,
 1142483975,2026948561,1053920743, 786262391,1792203830,1494667770,
 1923011392,1433700034,1244184613,1147297105, 539712780,1545929719,
  190641742,1645390429, 264907697, 620389253,1502074852, 927711160,
  364849192,2049576050, 638580085, 547070247]

def lcgrand(stream):
    zi     = zrng[stream]
    lowprd = (zi & 65535) * MULT1
    hi31   = (zi >> 16) * MULT1 + (lowprd >> 16)
    zi     = ((lowprd & 65535) - MODLUS) + ((hi31 & 32767) << 16) + (hi31 >> 15)
    if (zi < 0):
        zi += MODLUS
    lowprd = (zi & 65535) * MULT2
    hi31   = (zi >> 16) * MULT2 + (lowprd >> 16)
    zi     = ((lowprd & 65535) - MODLUS) + ((hi31 & 32767) << 16) + (hi31 >> 15)
    if (zi < 0):
        zi += MODLUS
    zrng[stream] = zi
    return (zi >> 7 | 1) / 16777216.0

def lcgrandst(zset, stream):
    zrng[stream] = zset

def lcgrandgt(stream):
    return zrng[stream]


def main():
    next_event_type = num_custs_delayed = 0
    num_delays_required = num_events = num_in_q = server_status = 0
    area_num_in_q = area_server_status = mean_interarrival = 0.0
    mean_service = sim_time = 0.0
    time_arrival = [0.0 for i in range(Q_LIMIT + 1)]
    time_last_event = 0.0
    time_next_event = [0.0 for i in range(3)]
    total_of_delays = 0.0
	infile = open("input.txt", "r")
	outfile = open("output.txt", "w")

 	# Specify the number of events for the timing function. 
	num_events = 2

    # Read input parameters. 
    vals = infile.readline().strip().split(" ")
    mean_interarrival = vals[0]
    mean_service = vals[1]
    num_delays_required = vals[2]

    # Write report heading and input parameters. 
    outfile.write("Single-server queueing system\n\n")
   	outfile.write("Mean interarrival time " + str(mean_interarrival) + " minutes\n\n")
    outfile.write("Mean service time " + str(mean_service) + " minutes\n\n")
    outfile.write("Number of customers " + str(num_delays_required) + "\n")

    # Initialize the simulation. */
    initialize()

    # Run the simulation while more delays are still needed. 
    while (num_custs_delayed < num_delays_required):

        #Determine the next event.
        timing()

        # Update time-average statistical accumulators. 
        update_time_avg_stats()

        # Invoke the appropriate event function. 
        if next_event_type == 1:
            arrive()
            break
        elif next_event_type == 2:
            depart()
            break

    # Invoke the report generator and end the simulation.
    report()


	infile.close()
	outfile.close()



def initialize():
     #Initialization function. */

    #Initialize the simulation clock. */
    sim_time = 0.0

    #/* Initialize the state variables. */
    server_status   = IDLE
    num_in_q        = 0
    time_last_event = 0.0

    #/* Initialize the statistical counters. */
    num_custs_delayed  = 0
    total_of_delays    = 0.0
    area_num_in_q      = 0.0
    area_server_status = 0.0

    #/* Initialize event list.  Since no customers are present, the departure
    #   (service completion) event is eliminated from consideration. */
    time_next_event[1] = sim_time + expon(mean_interarrival)
    time_next_event[2] = 1.0e+30


def timing():  
    #/* Timing function. */

    min_time_next_event = float(1.0e+29)

    next_event_type = 0

    #/* Determine the event type of the next event to occur. */
    for i in range(1, num_events+1):
        if time_next_event[i] < min_time_next_event:
            min_time_next_event = time_next_event[i]
            next_event_type = i
   
    #/* Check to see whether the event list is empty. */
    if (next_event_type == 0):

        #/* The event list is empty, so stop the simulation. */
        outfile.write("\nEvent list empty at time " + str(sim_time) + "\n")
        sys.exit()


    #/* The event list is not empty, so advance the simulation clock. */
    sim_time = min_time_next_event


def arrive():
    #/* Arrival event function. */


    #/* Schedule next arrival. */
    time_next_event[1] = sim_time + expon(mean_interarrival)

    #/* Check to see whether server is busy. */
    if (server_status == BUSY): 

        #/* Server is busy, so increment number of customers in queue. */
        num_in_q += 1

        #/* Check to see whether an overflow condition exists. */
        if (num_in_q > Q_LIMIT): 

            #/* The queue has overflowed, so stop the simulation. */
            outfile.write("\nOverflow of the array time_arrival at")
            outfile.write(" time " + str(sim_time) + "\n")
            sys.exit()


        #/* There is still room in the queue, so store the time of arrival of the
        #   arriving customer at the (new) end of time_arrival. */
        time_arrival[num_in_q] = sim_time

    else:

        #/* Server is idle, so arriving customer has a delay of zero.  (The
        #   following two statements are for program clarity and do not affect
        #   the results of the simulation.) */
        delay            = 0.0
        total_of_delays += delay

        #/* Increment the number of customers delayed, and make server busy. */
        num_custs_delayed += 1
        server_status = BUSY

        #/* Schedule a departure (service completion). */
        time_next_event[2] = sim_time + expon(mean_service)


def depart():
    #/* Departure event function. */

    #/* Check to see whether the queue is empty. */
    if (num_in_q == 0):

        #/* The queue is empty so make the server idle and eliminate the
        #   departure (service completion) event from consideration. */
        server_status      = IDLE
        time_next_event[2] = 1.0e+30

    else:

        #/* The queue is nonempty, so decrement the number of customers in
        #   queue. */
        num_in_q -= 1

        #/* Compute the delay of the customer who is beginning service and update
        #   the total delay accumulator. */
        delay            = sim_time - time_arrival[1]
        total_of_delays += delay

        #/* Increment the number of customers delayed, and schedule departure. */
        num_custs_delayed += 1
        time_next_event[2] = sim_time + expon(mean_service) 

        #/* Move each customer in queue (if any) up one place. */
        for i in range(1, num_in_q+1):
            time_arrival[i] = time_arrival[i + 1]

def report():
    #/* Report generator function. */

    #/* Compute and write estimates of desired measures of performance. */
    outfile.write("\n\nAverage delay in queue " + str(total_of_delays/float(num_custs_delayed))+ " minutes\n\n")
    outfile.write("Average number in queue " + str(area_num_in_q/float(sim_time))+" \n\n")
    outfile.write("Server utilization" + str(area_server_status / float(sim_time)) + " \n\n", )
    outfile.write("Time simulation ended " + str(sim_time) +" minutes")


def update_time_avg_stats():
    #/* Update area accumulators for time-average statistics. */


    #/* Compute time since last event, and update last-event-time marker. */
    time_since_last_event = sim_time - time_last_event
    time_last_event = sim_time

    #/* Update area under number-in-queue function. */
    area_num_in_q      += num_in_q * time_since_last_event

    #/* Update area under server-busy indicator function. */
    area_server_status += server_status * time_since_last_event


#Exponential variate generation function. */
#Return an exponential random variate with mean "mean".
def expon(mean):
    return -1 * mean * math.log(lcgrand(1), e)

