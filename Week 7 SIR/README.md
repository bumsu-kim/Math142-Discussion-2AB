
The simulation is not based on the mathematical SIR model, but it simulates the following:
    * Given the population of N individuals,
    * at each time step, each individual meets others randomly ("contacts"),
    * and the disease may spread (with given "chance to transmit").
    * Each infected individual will have random time period for recovery. After this period, they get recovered
    * We keep track of the number of Susceptible (S), Infected (I), and Recovered (R) individuals.

This seems quite reasonable. We will see if the traditional SIR model actually fits with this simulation.

To run the simulation, build and run main.cpp.

For the parameters, Use, for instance,
	* N = 1000000
	* I0 = 0.0001
	* Time Unit = 0.5 (half days)
	* Avg Period to Recover = 3 (three days)
	* Avg Contact = 4 (each person meets four people per day in average)
	* Chnace to Transmit = 0.125 (12.5% chance to transmit per contact)
	* Time Period = 150 (observe for 150 days)

TIP: For VS users, Use "release" mode with "x64" setting when you're building.
With the release mode, this runs in around 5 sec on my PC, so it shouldn't take too long on your computer, too.

After you run the program, you'll see some csv file with ugly name (info on parameters is in the file name).
Copy its name, and use it as "fname" in "PlotStats.m", a MATLAB script.

It will 
    * First plot S, I, and R over time.
    * Next, will compare the SIR model with the simulation, i.e., 
    *
    * beta = (contacts per person per time) * (chance to transmit per contact)
    * gamma = (1 / average recovery time )
    * 
    * Traditional SIR Model assumes
    *
    * S' = -beta * S * I
    * I' =  beta * S * I - gamma * I
    * R' = gamma * I
    *
    * We will see if the number of new infections (-S') at each time step
    *  actually matches with beta*S*I.
    *
    * (R' being gamma*I is more straightforward from the simulation setting)