#include "Npuzzle.h"

#include <array>
#include <fstream>
#include <thread>

bool Npuzzle::Solver::duplicate(
	const Board& b,
	uMap& closed,
	const int& n)
{
	return closed.count(BoardFunctions::encode(b, n));
}

void Npuzzle::Solver::addQueue(
	const Board& b,
	const Board& p,
	pQueue& open,
	uMap& closed,
	const int& n)
{
	auto c = new Structures::BoardInfo;

	c->board = b;
	c->heuristic = BoardFunctions::heuristic(b, n);

	open.emplace(c);

	closed.insert({ BoardFunctions::encode(b, n), p });
}

void Npuzzle::Solver::addMoves(
	const Board& b,
	pQueue& open,
	uMap& closed,
	const int& n)
{
	auto zero = findZero(b, n);

	//Up, down, left, right
	auto moves = std::array<Board, 4>{
		BoardFunctions::move(b, { 0, -1 }, zero, n),
		BoardFunctions::move(b, { 0, 1 }, zero, n),
		BoardFunctions::move(b, { -1, 0 }, zero, n),
		BoardFunctions::move(b, { 1, 0 }, zero, n)};

	for (auto i = 0; i < 4; ++i)
	{
		if (moves[i].size() == (n * n))
		{
			if (!duplicate(moves[i], closed, n))
			{
				addQueue(moves[i], b, open, closed, n);
			}
		}
	}
}

void Npuzzle::Solver::printBoard(
	const Board& b,
	const int& n)
{
	for (auto i = 0; i < (n * n); ++i)
	{
		std::cout << b[i] << "\t";

		if ((i % n) == (n - 1))
		{
			std::cout << std::endl;
		}
	}
}

std::pair<int, double> Npuzzle::Solver::reverseSolution(
	Board b,
	uMap& closed,
	const int& interval,
	const double& t,
	const bool& solve,
	const int& n)
{
	std::vector<Board> solution;

	do
	{
		auto p = b;

		solution.push_back(p);

		b = closed[BoardFunctions::encode(p, n)];
	} while (b != Board(0));

	auto size = int(solution.size() - 1);

	if (solve)
	{
		for (auto i = size; i >= 0; --i)
		{
			printBoard(solution[i], n);

			std::this_thread::sleep_for(std::chrono::milliseconds(interval));

			if (i != 0)
			{
				system("CLS");
			}
		}
	}

	return { size + 1, t };
}

void Npuzzle::Solver::writeBoard(
	const Board& b,
	const int& n)
{
	std::ofstream board("board.csv");

	for (auto i = 0; i < n; ++i)
	{
		for (auto j = 0; j < n; ++j)
		{
			auto k = i * n + j;

			board << b[k] << ",";
		}

		board << std::endl;
	}
}

void Npuzzle::Solver::cleanup(
	pQueue& open,
	uMap& closed)
{
	while (!open.empty())
	{
		delete open.top();

		open.pop();
	}

	closed.clear();
}

std::pair<int, double> Npuzzle::Solver::solve(
	const int& interval,
	const bool& solve,
	const int& n)
{
	//Open list contains all unexplored nodes, sorted by heuristic value
	pQueue open;

	//Closed list contains all explored nodes, with values set to encoded parent board
	uMap closed;

	auto curr = BoardFunctions::createBoard(n);
	//curr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 15 };

	addQueue(curr, Board(0), open, closed, n);

	auto solved = false;
	auto start = std::chrono::system_clock::now();

	while (!solved)
	{
		curr = open.top()->board;

		if (open.top()->heuristic == 0)
		{
			solved = true;
		}
		else
		{
			//writeBoard(curr, n);

			delete open.top();
			open.pop();

			addMoves(curr, open, closed, n);
		}
	}

	auto end = std::chrono::system_clock::now();
	auto t = std::chrono::duration<double>(end - start);
	auto results = reverseSolution(curr, closed, interval, t.count(), solve, n);

	cleanup(open, closed);

	return results;
}