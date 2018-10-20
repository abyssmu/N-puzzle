#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <thread>

#include "Npuzzle.h"

bool duplicate(
	const Npuzzle::Board b,
	Npuzzle::map& closed,
	const int n)
{
	return closed.count(Npuzzle::encode(b, n));
}

void addQueue(
	const Npuzzle::Board b,
	const Npuzzle::Board parent,
	Npuzzle::set& open,
	Npuzzle::map& closed,
	const int n)
{
	auto c = new Npuzzle::Structures::Container;

	c->board = b;
	c->heuristic = Npuzzle::heuristic(b, n);

	open.emplace(c);

	closed.insert({ Npuzzle::encode(b, n), Npuzzle::encode(parent, n) });
}

void addMoves(
	const Npuzzle::Board b,
	Npuzzle::set& open,
	Npuzzle::map& closed,
	const int n)
{
	auto moves = std::vector<Npuzzle::Board>(4);
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
	Npuzzle::set& open,
	Npuzzle::map& closed)
{
	//Used for set
	//open.clear();

	//Used for priority queue
	while (!open.empty())
	{
		delete open.top();

		open.pop();
	}

	closed.clear();
}

void printBoard(
	const Npuzzle::Board b,
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
	Npuzzle::i64 b,
	Npuzzle::map closed,
	const int n)
{
	std::vector<Npuzzle::Board> solution;

	do
	{
		auto p = b;

		solution.push_back(Npuzzle::decode(b, n));

		b = closed[p];
	} while (b != 0);

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
	Npuzzle::Board& curr,
	Npuzzle::set& open,
	Npuzzle::map& closed,
	const int n)
{
	cleanup(open, closed);

	curr = Npuzzle::createBoard(n);

	addQueue(curr, Npuzzle::Board(n * n), open, closed, n);
}

void writeBoard(
	const Npuzzle::Board b,
	const int n)
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

void solve(
	Npuzzle::Board& curr,
	Npuzzle::set& open,
	Npuzzle::map& closed,
	const int n)
{
	auto solved = false;

	//Create initial board
	curr = Npuzzle::createBoard(n);

	//Test state
	//curr = { 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 1, 14, 15 };

	addQueue(curr, Npuzzle::Board(n * n), open, closed, n);

	while (!solved)
	{
		//Used for set
		//auto top = *open.begin();
		
		curr = open.top()->board;


		if (open.top()->heuristic == 0)
		{
			solved = true;
		}
		else
		{
			//writeBoard(curr, n);

			//Used for set
			//open.erase(top);

			//Used for priority queue
			delete open.top();
			open.pop();

			addMoves(curr, open, closed, n);
		}
	}
}