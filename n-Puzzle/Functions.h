#include <chrono>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <thread>

#include "Npuzzle.h"

bool duplicate(
	std::vector<int> b,
	std::unordered_map<std::uint_fast64_t, std::uint_fast64_t>& closed,
	const int n)
{
	return closed.count(Npuzzle::encode(b, n));
}

void addQueue(
	std::vector<int> b,
	std::vector<int> parent,
	std::priority_queue<Npuzzle::Container*, std::vector<Npuzzle::Container*>, Npuzzle::GreaterThanByHeur>& open,
	std::unordered_map<std::uint_fast64_t, std::uint_fast64_t>& closed,
	const int n)
{
	auto c = new Npuzzle::Container;

	c->code = Npuzzle::encode(b, n);
	c->heuristic = Npuzzle::heuristic(b, n);

	open.emplace(c);

	closed.insert({ Npuzzle::encode(b, n), Npuzzle::encode(parent, n) });
}

void addMoves(
	const std::vector<int> b,
	std::priority_queue<Npuzzle::Container*, std::vector<Npuzzle::Container*>, Npuzzle::GreaterThanByHeur>& open,
	std::unordered_map<std::uint_fast64_t, std::uint_fast64_t>& closed,
	const int n)
{
	auto moves = std::vector<std::vector<int>>(4);
	auto parent = b;

	moves[0] = Npuzzle::up(b, n);
	moves[1] = Npuzzle::down(b, n);
	moves[2] = Npuzzle::left(b, n);
	moves[3] = Npuzzle::right(b, n);

	for (auto i = 0; i < 4; ++i)
	{
		if (moves[i].size() == (n * n))
		{
			if (!duplicate(moves[i], closed, n))
			{
				addQueue(moves[i], parent, open, closed, n);
			}
		}
	}
}

void cleanup(
	std::priority_queue<Npuzzle::Container*, std::vector<Npuzzle::Container*>, Npuzzle::GreaterThanByHeur>& open,
	std::unordered_map<std::uint_fast64_t, std::uint_fast64_t>& closed)
{
	while (!open.empty())
	{
		delete open.top();
		open.pop();
	}

	closed.clear();
}

void printBoard(
	const std::vector<int> b,
	const int n)
{
	for (auto j = 0; j < n * n; ++j)
	{
		std::cout << b[j] << "\t";

		if (j % n == 3)
		{
			std::cout << std::endl;
		}
	}
}

int print(
	std::uint_fast64_t b,
	std::unordered_map<std::uint_fast64_t, std::uint_fast64_t> closed,
	const int n)
{
	std::vector<std::vector<int>> solution;

	solution.push_back(Npuzzle::decode(b, n));

	for (auto p = closed[b]; p != 0; p = closed[p])
	{
		solution.push_back(Npuzzle::decode(p, n));
	}

	system("CLS");

	auto size = int(solution.size() - 1);

	for (auto i = size; i >= 0; --i)
	{
		printBoard(solution[i], n);

		std::this_thread::sleep_for(std::chrono::milliseconds(25));

		if (i != 0)
		{
			system("CLS");
		}
	}

	return size;
}

void reset(
	std::vector<int>& curr,
	std::priority_queue<Npuzzle::Container*, std::vector<Npuzzle::Container*>, Npuzzle::GreaterThanByHeur>& open,
	std::unordered_map<std::uint_fast64_t, std::uint_fast64_t>& closed,
	const int n)
{
	cleanup(open, closed);

	curr = Npuzzle::createBoard(n);

	addQueue(curr, std::vector<int>(n * n), open, closed, n);
}

void solve(
	std::vector<int>& curr,
	std::priority_queue<Npuzzle::Container*, std::vector<Npuzzle::Container*>, Npuzzle::GreaterThanByHeur>& open,
	std::unordered_map<std::uint_fast64_t, std::uint_fast64_t>& closed,
	const int n)
{
	auto solved = false;

	//Create initial board
	curr = Npuzzle::createBoard(n);

	addQueue(curr, std::vector<int>(n * n), open, closed, n);

	while (!solved)
	{
		if (open.top()->heuristic == 0)
		{
			solved = true;
		}
		else
		{
			curr = Npuzzle::decode(open.top()->code, n);

			delete open.top();
			open.pop();

			addMoves(curr, open, closed, n);
		}
	}
}