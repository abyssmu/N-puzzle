#include <chrono>
#include <iostream>
#include <unordered_map>
#include <queue>
#include <thread>

#include "Npuzzle.h"

bool duplicate(
	std::vector<int> b,
	std::unordered_map<std::uint_fast64_t, std::uint_fast64_t>& closed,
	int n)
{
	return closed.count(Npuzzle::encode(b, n));
}

void addQueue(
	std::vector<int> b,
	std::vector<int> parent,
	std::priority_queue<Npuzzle::Container*, std::vector<Npuzzle::Container*>, Npuzzle::GreaterThanByHeur>& open,
	std::unordered_map<std::uint_fast64_t, std::uint_fast64_t>& closed,
	int n)
{
	Npuzzle::Container* c = new Npuzzle::Container;

	c->code = Npuzzle::encode(b, n);
	c->heuristic = Npuzzle::heuristic(b, n);

	open.push(c);

	closed.insert({ Npuzzle::encode(b, n), Npuzzle::encode(parent, n) });
}

void addMoves(
	std::vector<int>& b,
	std::priority_queue<Npuzzle::Container*, std::vector<Npuzzle::Container*>, Npuzzle::GreaterThanByHeur>& open,
	std::unordered_map<std::uint_fast64_t, std::uint_fast64_t>& closed,
	int n)
{
	std::vector<std::vector<int>> moves(4);
	std::vector<int> parent = b;

	moves[0] = Npuzzle::up(b, n);
	moves[1] = Npuzzle::down(b, n);
	moves[2] = Npuzzle::left(b, n);
	moves[3] = Npuzzle::right(b, n);

	for (int i = 0; i < 4; ++i)
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

int print(
	std::uint_fast64_t b,
	std::unordered_map<std::uint_fast64_t, std::uint_fast64_t> closed,
	int n)
{
	std::vector<std::vector<int>> rev;
	int size = 0;

	while (closed[b] != 0)
	{
		rev.insert(rev.begin(), Npuzzle::decode(b, n));

		b = closed[b];
	}

	size = rev.size();

	system("CLS");

	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < n * n; ++j)
		{
			std::cout << rev[i][j] << "\t";

			if (j % n == 3)
			{
				std::cout << std::endl;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(25));

		if (i != (size - 1))
		{
			system("CLS");
		}
	}

	return size;
}

void solve(
	std::vector<int>& curr,
	std::priority_queue<Npuzzle::Container*, std::vector<Npuzzle::Container*>, Npuzzle::GreaterThanByHeur>& open,
	std::unordered_map<std::uint_fast64_t, std::uint_fast64_t>& closed,
	int n)
{
	bool solved = false;

	//Create initial board
	curr = Npuzzle::createBoard(n);

	addQueue(curr, std::vector<int>(n * n), open, closed, n);

	while (!solved)
	{
		if (open.top()->heuristic == 0)
		{
			solved = true;
		}

		curr = Npuzzle::decode(open.top()->code, n);

		open.pop();

		addMoves(curr, open, closed, n);
	}
}