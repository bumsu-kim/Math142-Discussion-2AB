#include "SIR.h"
#include <sstream>

int main() {
	std::srand(std::time(nullptr));
	/*
	* Use, for instance,
	* N = 1000000
	* I0 = 0.0001
	* Time Unit = 0.5 (half days)
	* Avg Period to Recover = 3 (three days)
	* Avg Contact = 4 (each person meets four people per day in average)
	* Chnace to Transmit = 0.125 (12.5% chance to transmit per contact)
	* Time Period = 150 (observe for 150 days)
	*/
	SIR::luint N = 10000;
	std::cout << "Enter the size of the entire population(N): ";
	std::cin >> N;

	double I0 = 0.01;
	std::cout << "Enter the initial fraction of infected people(I_0): ";
	std::cin >> I0;

	double time_unit;
	std::cout << "Enter the unit of the time step (e.g. hour, day, week, etc.) in days (e.g. 0.5, 1, 7, 30, etc.): ";
	std::cin >> time_unit;

	double avg_period_to_recover = 14.0;
	std::cout << "Enter the average time for recovery in days (1/gamma): ";
	std::cin >> avg_period_to_recover;
	avg_period_to_recover /= time_unit;
	double gamma = 1./avg_period_to_recover;  // recovery chance per time step

	double avg_Contact = 1.5;
	std::cout << "Enter the average number of contacts per person per day: ";
	std::cin >> avg_Contact;
	avg_Contact *= time_unit;

	double chance_to_transmit = 0.11;
	std::cout << "Enter the chance to transmit the desease per contact: ";
	std::cin >> chance_to_transmit;

	double time_period;
	std::cout << "Enter the entire time period to see in days: ";
	std::cin >> time_period;
	int N_time_steps = (time_period + 0.5) / time_unit;

	SIR::Simulation Sim(N, I0, time_unit, avg_Contact, chance_to_transmit, avg_period_to_recover);

	std::ostringstream oss;
	oss << "output_N_" << N << "_I0_" << I0 << "_timeUnit_" << time_unit
		<< "_gamma_" << gamma << "_avgContact_" << avg_Contact
		<< "_TransmitChance_" << chance_to_transmit
		<< "_time_period_" << time_period << ".csv";

	std::ofstream ofs(oss.str());
	std::cout << "Initialization done.\n";
	Sim.write_header(ofs);
	Sim.display();

	// do simulation here
	int disp_cnt = 0;
	for (size_t i = 0; i < N_time_steps; ++i) {
		Sim.next_time_step();
		// display
		if (N_time_steps >= 100 && i > disp_cnt*N_time_steps / 10) {
			++disp_cnt;
			Sim.display();
		}
		// save to file
		Sim.write(ofs);
	}

	// Display the final stat
	Sim.display_stat();
	ofs.close();

	// prevent the program to be terminated
	char ch;
	std::cin >> ch;
	return 0;
}