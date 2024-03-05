# Multithreaded Theater Transaction in C


## Members: Dimitris Tzovanis, Elpida Stasinou


#### Warning: before running the test-res.sh file change the path on line 3, so that the command that runs the program with 100 clients and 100 seeds is called correctly
The code contains a main and a function. First in main we take as input the number of clients and the seed. 
The tables we will use are initialized (and created dynamically) and we create the threads (dynamically). 
We also check the input and whether the threads are created correctly.
Each thread is put into the function where there are initially the callers who accept the threads or not depending on availability. 
This works through a while and a thread condition, it should be mentioned that we have locked this point with a mutex. 
Then with some probability (using the random function) the client (thread) chooses zone, 
and then finds the first available seat in some row that fits all the seats it has booked (the seats are random), 
there we store in tables and in the plan the seats, tickets and zone to use later. 
If there are no seats available it displays an appropriate message, the thread exits the function and the caller is available again.


Otherwise the client (thread) proceeds to the cashier (again releasing the voicemail), i.e. goes on hold, 
in a similar while with condition and mutex to before in case of no cashier availability. 
Then with some probability the card transaction succeeds, where the customer's name, the ticket price is displayed 
(this price is added to a variable which expresses the revenue) and the zone, row and reserved seats are displayed. 
At this point we use the tables we filled in and saved the corresponding values before, so that they are not lost or changed by another thread.
 
However, if the card is rejected, it displays the appropriate message, and the positions are reset from the position table-plane. 
In both cases then the thread is exited from the function and the cashier is available again.
In the end, the threads are deleted in a secure manner, and we print out for each customer their closed positions, 
the position plan, the percentages of each case where the transaction ended (no position, card declined, 
or transaction succeeded) which we measure with variables that we increment within the function on a per-case basis. 
Also shown is the average wait time of the answering machine, cashier and average customer service agent time, 
which is achieved by using the clock function (which measures the time of a particular moment) and a simple subtract and store value in a sum or table. 
Finally, we release all the dynamically bound memory.
