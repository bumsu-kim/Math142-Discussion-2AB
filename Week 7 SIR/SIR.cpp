#include "SIR.h"

namespace SIR {
	// set a seed and define a random number generator
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);

	Individual::Individual(Status _status) : status(_status), status_next(_status), sick_time(0), period_to_recover(14) {}

	void Individual::infect(int sick_period, Timing when) {
		period_to_recover = sick_period;
		if (when == Timing::now) { // if "now"
			status = infected;
		}
		status_next = infected; // always
	}

	void Individual::recover(Timing when) {
		if (when == Timing::now) { // if "now"
			status = recovered;
		}
		status_next = recovered; // always
	}
	
	void Individual::apply_status() {
		status = status_next;
	}
	Population::Population(luint _N) : pop(_N) {}

	void Population::infect_individual(luint index, luint period_to_recover, uVec& iv, Timing when) {
		pop[index].infect(period_to_recover, when);
		iv.push_back(index);
	}

	void Population::infect_individual(luint index, luint period_to_recover, uSet& iv, Timing when) {
		pop[index].infect(period_to_recover, when);
		iv.insert(index);
	}

	void Population::recover_individual(luint index, uVec& rv) {
		pop[index].recover();
		rv.push_back(index);
	}

	Individual& Population::operator[](luint idx) {
		return pop[idx];
	}

	const Individual& Population::operator[](luint idx) const {
		return pop[idx];
	}

	bool Population::is_recovered(luint id) const {
		return pop[id].sick_time >= pop[id].period_to_recover;
	}

	bool Population::is_infected(luint id) const {
		return pop[id].status == infected;
	}

	bool Population::is_susceptible(luint id) const {
		return pop[id].status == susceptible;
	}

	luint Simulation::pick() const {
		return std::rand() % N;
	}

	luint Simulation::get_contacts() {
		return contacts_gen(generator);
	}

	luint Simulation::get_recovery() {
		return period_to_recover(generator);
	}

	bool Simulation::transmit() const {
		return (std::rand() * 1.0 / RAND_MAX < chance_to_transmit);
	}

	Simulation::Simulation(
		luint _N,
		double I0,
		double _time_unit,
		double avgContacts,
		double chance,
		double avgSickPeriod) :
		N(_N),
		S(_N* (1. - I0)),
		I(_N* I0),
		R(_N - S - I),
		time_unit(_time_unit),
		curr_time(0),
		avg_contact_per_time(avgContacts),
		chance_to_transmit(chance),
		avg_time_to_recover(avgSickPeriod),
		P(_N),
		I_peak(0),
		S_at_I_peak(S),
		time_at_I_peak(0),
		new_infections(0),
		contacts_gen(avgContacts / 2), //  div by 2 (otherwise x2 larger number of contacts; come and go)
		period_to_recover(1.0 / avgSickPeriod)
	{
		// initialize list_infected
		for (size_t i = 0; i < I; ++i) {
			P.infect_individual(i, get_recovery(), list_infected, Timing::now);
		}

		// compute the basic reproductive number ( R0 = gamma / beta)
		R0 = (1. / avgSickPeriod) / (avg_contact_per_time * chance_to_transmit);

		// display the initial stats
		std::cout
			<< "\nn_contact_per_time = " << avg_contact_per_time
			<< "\nchance_to_transmit = " << chance_to_transmit
			<< "\nbeta = " << avg_contact_per_time* chance_to_transmit
			<< "\nchance_to_recover = " << (1. / avg_time_to_recover) // per time step
			<< "\nreproductive number = " << R0
			<< "\n";
		display();
	}

	void Simulation::next_time_step() {
		// update the time
		++curr_time;

		if (I == 0) { // if no infected people, nothing to do
			return;
		}

		// increment the sick_time for the infected
		for (luint id : list_infected) {
			++P[id].sick_time;
		}

		// list of susceptible individuals to be infected at the end of the step
		std::vector<luint> list_next_infected;
		// list of infected individuals to be recovered at the end of the step
		std::vector<luint> list_next_recovered;
		// simulate contacts
		for (size_t i = 0; i < N; ++i) { // for each individual
			luint nContacts = get_contacts(); // determine the number of contacts
			switch (P[i].status) { // according to their status
			case susceptible:
				for (size_t j = 0; j < nContacts; ++j) { // for each contact
					luint id_met = pick(); // find someone to contact
					if (P.is_infected(id_met) && transmit()) { // if the other is infected, and transmission occurs
						// infect this individual with a random recovery period, and put this individual in the list
						P.infect_individual(i, get_recovery(), list_next_infected);
					}
				}
				break;
			case infected:
				for (size_t j = 0; j < nContacts; ++j) { // for each contact
					luint id_met = pick(); // find someone to contact
					if (transmit() && P.is_susceptible(id_met)) { // if the transmission occurs, and the other is susceptible
						// // infect the other with a random recovery period and put the other in the list
						P.infect_individual(id_met, get_recovery(), list_next_infected);
					}
				}
				if (P.is_recovered(i)) { // check if this individual is being recovered at the next time step
					P.recover_individual(i, list_next_recovered);
				}
				break;
			default: // this individual is recovered
				// nothing to do
				break;
			}
		}

		// after all contacts are over, change their status and re-compute statistics

		// apply recovery status (recovered just now)
		for (luint id : list_next_recovered) {
			P[id].apply_status(); // apply status
			list_infected.erase(id); // erase from the infected list
		}
		R += list_next_recovered.size();

		// apply infectious status (infected just now)
		for (luint id : list_next_infected) {
			P[id].apply_status();
			list_infected.insert(id);
		}


		// measure the size of each population
		new_infections = list_next_infected.size();
		I = list_infected.size();
		S = N - I - R;
		
		// update if it's a new peak for I
		if (I > I_peak) {
			I_peak = I;
			S_at_I_peak = S;
			time_at_I_peak = curr_time;
		}
	}
	void Simulation::display() const {
		std::cout << "Time: " << curr_time*time_unit << "\n"
			<< "Susceptible\tInfected\tRecovered\n"
			<< std::setw(16) << std::left << S
			<< std::setw(16) << std::left << I
			<< std::setw(16) << std::left << R << "\n\n";
	}
	void Simulation::display_stat() const {
		std::cout
			<< "\nn_contact_per_time = " << avg_contact_per_time
			<< "\nchance_to_transmit = " << chance_to_transmit
			<< "\nbeta = " << avg_contact_per_time * chance_to_transmit
			<< "\nchance_to_recover = " << (1. / avg_time_to_recover) // per time step
			<< "\nreproductive number = " << R0
			<< "\n";
		display();
		std::cout
			<< "I peak time: " << time_at_I_peak*time_unit
			<< "\nI peak : " << I_peak
			<< "\nS at I's peak: " << S_at_I_peak
			<< "\nReproductive Number: " << R0
			<< "\nReproductive Number * N: " << R0 * N
			<< "\n" << std::endl;

	}
	void Simulation::write(std::ostream& os) const {
		os << time_unit * curr_time << ", "
			<< S << ", "
			<< I << ", "
			<< R << ", "
			<< new_infections << ", "
			<< static_cast<double>(S) * I * chance_to_transmit * avg_contact_per_time /N
			<< "\n";
		/*if (static_cast<double>(S) * I * chance_to_transmit * avg_contact_per_time / N < 1e-6) {
			std::cout << "******Why?*****\n";
			display_stat();
			char ch;
			std::cin >> ch;
		}*/
	}
	void Simulation::write_header(std::ostream& os) const {
		os << "time, "
			<< "S, "
			<< "I, "
			<< "R, "
			<< "New Infcetions, "
			<< "beta*S*I"
			<< "\n";
	}
};