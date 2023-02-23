#ifndef __SIR_H__
#define __SIR_H__

#include <iostream> // console output
#include <iomanip>  // formatting
#include <cstdlib>  // rand()
#include <vector>
#include <unordered_set>      // 
#include <fstream>
#include <random>
#include <chrono>

namespace SIR {

	// unsigned int type (can be long long unsigned or size_t if needed)
	using luint = unsigned int;

	// Status of each individual
	enum Status { susceptible, infected, recovered };
	// Timing of infection: now or later
	enum Timing {now, later};
	// class for each individual
	struct Individual {
		Status status, status_next;
		luint sick_time, period_to_recover;
	public:
		// susceptible by default
		Individual(Status _status = susceptible);

		/**
		* Infect an individual
		* by setting next status = infected.
		* 
		* @param period to recover
		*/
		void infect(int sick_period, Timing = later);
		/**
		* Recover an individual from infection
		* by setting next status = recovered
		*/
		void recover(Timing = later);
		/**
		* Apply the next status to the current
		*/
		void apply_status();
	};

	using uVec = std::vector<luint>;
	using uSet = std::unordered_set<luint>;

	// defines and controls the whole population
	class Population {
		std::vector<Individual> pop;
	public:
		/**
		* Initialize the population by
		* creating a vector of individuals
		*/
		Population(luint _N);
		/**
		* Infect an individual
		* 
		* @param index the individaul's index in pop
		* @param period_to_recover
		* @param iv Put this individual's index into this vector to record
		* @param Timing (Optional) now or later. Default is later
		*/
		void infect_individual(luint index, luint period_to_recover, uVec& iv, Timing = later);

		/**
		* Infect an individual
		*
		* @param index the individaul's index in pop
		* @param period_to_recover
		* @param iset Put this individual's index into this set to record
		* @param Timing (Optional) now or later. Default is later
		*/
		void infect_individual(luint index, luint period_to_recover, uSet& iset, Timing = later);

		/**
		* Recover an individual
		* 
		* @param index the individaul's index in pop
		* @param rv Put this individual's index into this vector to record
		*/
		void recover_individual(luint index, uVec& rv);

		/**
		* Determine if an individual is recovered
		* by comparing the peroid to recover and the period of illness so far
		*/
		bool is_recovered(luint id) const;
		/**
		* Determine if an individual is infected
		*/
		bool is_infected(luint id) const;
		/**
		* Determine if an individual is susceptible
		*/
		bool is_susceptible(luint id) const;
		Individual& operator[](luint idx);
		const Individual& operator[](luint idx) const;
	};

	class Simulation {
		luint N; // total population
		luint S; // Susceptible population
		luint I; // Infected population
		luint R; // Recovered population
		double time_unit; // time unit in day (e.g. 0.5 days, 1 day, 7 days, etc.)
		int curr_time; // current time step (starts from 0)
		double avg_contact_per_time; // number of contact per person per time unit (averaged)
		double chance_to_transmit; // chance to transmit per contact
		double avg_time_to_recover; // average number of time steps required for recovery
		Population P; // simulate the entire population as a vector
		std::unordered_set<luint> list_infected; // list of infected people (indices of "pop")
		double R0; // basic reproductive number

		// statistics
		luint I_peak; // find the peak of I
		luint S_at_I_peak; // value of S when I is at its peak
		luint time_at_I_peak; // when I is at its peak
		int new_infections; // number of new infections (transmissions) at each time step (it does not count recovered)

		// random number generators
		std::poisson_distribution<int> contacts_gen; // number of contacts per person per time unit.
		std::exponential_distribution<double> period_to_recover; // number of time steps needed to recover for each infected individual

		// private methods
		/**
		* pick a random person from the population "pop"
		* by picking an integer from [0, N-1] uniformly random
		* 
		* @return an index of pop, in [0, N)
		*/
		luint pick() const; // pick a random person from pop
		/**
		* Draw an integer from "contacts_gen"
		* from a Poisson distribution by default
		* the mean is given when the class is initialized
		* 
		* @return number of contacts for each individual during a time step
		*/
		luint get_contacts();

		/**
		* Draw an integer from "period_to_recover"
		* from an exponential distribution by default
		* the mean is given when the class is initialized
		* 
		* @return number of time steps required for the recovery
		*/
		luint get_recovery();

		/**
		* Determine if disease is transmitted at a contact
		* Chance to transmit is given when the class is initialized
		* 
		* @return true if transmitted
		*/
		bool transmit() const;

	public:
		/**
		* Initalize the Population
		* @param N size of the population
		* @param I0 fraction of the infected (e.g. 0.01)
		* @param _time_unit Unit of the time in days
		* @param avgContacts Average number of contacts per person per time unit
		* @param chance Chance to transmit the disease per contact
		* @param avgSickPeriod Average number of time steps to recover
		*/
		Simulation(luint _N,
			double I0,
			double _time_unit,
			double avgContacts,
			double chance,
			double avgSickPeriod);

		/**
		* Simulate each time step.
		* Each individual meets others, and possibly transmits the disease, or recover from it
		*/
		void next_time_step();

		/**
		* display the current time, and S, I and R
		*/
		void display() const;

		/**
		* display the status (peak info, reproductive number, etc.)
		*/
		void display_stat() const;

		/**
		* Write the current stat to an oupput stream (i.e. a file)
		*/
		void write(std::ostream& os) const;

		/**
		* Write the header to an oupput stream (i.e. a file)
		*/
		void write_header(std::ostream& os) const;
	};
}


#endif
