#include "Functions.h"

int main()
{
	auto start = std::chrono::system_clock::time_point();
	auto end = std::chrono::system_clock::time_point();

	auto b = Npuzzle::Board();

	//Open list contains all unexplored nodes, sorted by heuristic value
	std::set<Npuzzle::Structures::Container*, Npuzzle::Structures::LessThanByHeur> open;
	
	//Closed list contains all explored nodes, with values set to encoded parent board
	std::unordered_map<Npuzzle::i64, Npuzzle::i64> closed;

	auto n = 4;

	//std::cout << "Input size of board: " << std::endl;
	//std::cin >> n;

	start = std::chrono::system_clock::now();
	
	solve(b, open, closed, n);
	
	end = std::chrono::system_clock::now();

	auto t = std::chrono::duration<double>();
	t = end - start;

	auto steps = print(Npuzzle::encode(b, n), closed, n);

	std::cout << std::endl;
	std::cout << std::fixed;
	std::cout << std::setprecision(5);
	std::cout << steps << " steps in " << t.count() << " secs.";

	//Cleanup
	cleanup(open, closed);

	std::cin.get();

	return 0;
}