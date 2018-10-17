#include <iomanip>

#include "Functions.h"

int main()
{
	std::chrono::system_clock::time_point start, end;
	std::vector<int> b;

	//Open list contains all unexplored nodes, sorted by heuristic value
	std::priority_queue<Npuzzle::Container*,
						std::vector<Npuzzle::Container*>,
						Npuzzle::GreaterThanByHeur> open;
	
	//Closed list contains all explored nodes, with values set to encoded parent board
	std::unordered_map<std::uint_fast64_t,
						std::uint_fast64_t> closed;

	int n = 4;

	//std::cout << "Input size of board: " << std::endl;
	//std::cin >> n;

	start = std::chrono::system_clock::now();

	solve(b, open, closed, n);

	end = std::chrono::system_clock::now();

	std::chrono::duration<double> t = end - start;
	int steps = print(Npuzzle::encode(b, n), closed, n);

	std::cout << std::endl;
	std::cout << std::fixed;
	std::cout << std::setprecision(5);
	std::cout << steps << " steps in " << t.count() << " secs.";

	std::cin.get();

	return 0;
}