#pragma once

#include <cstdint>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

namespace Npuzzle
{
	using Board = std::vector<int>;
	using Point = std::pair<int, int>;

	Point findZero(
		const Board& b,
		const int& n);

	namespace BoardFunctions
	{
		std::string encode(
			const Board& b,
			const int& n);

		bool isEven(
			const int& i);

		int inversions(
			const Board& b,
			const int& n);

		bool solvable(
			const Board& b,
			const int& n);

		Board createBoard(
			const int& n);

		int linear(
			const Board& b,
			const int& n);

		int manhattan(
			const Board& b,
			const int& n);

		int heuristic(
			const Board& b,
			const int& n);

		Board swapPos(
			const Board& b,
			const Point& oldPos,
			const Point& newPos,
			const int& n);

		bool between(
			const int& a,
			const int& b,
			const int& c);

		Board move(
			const Board& b,
			const Point& offset,
			const Point& oldZero,
			const int& n);
	}

	namespace Structures
	{
		struct BoardInfo
		{
			int heuristic;
			Board board;
		};

		struct LessThanByHeur
		{
			bool operator()(
				const BoardInfo* lhs,
				const BoardInfo* rhs) const
			{
				return lhs->heuristic > rhs->heuristic;
			}
		};
	}
	
	using pQueue = std::priority_queue<Npuzzle::Structures::BoardInfo*, std::vector<Npuzzle::Structures::BoardInfo*>, Npuzzle::Structures::LessThanByHeur>;
	using uMap = std::unordered_map<std::string, Board>;

	namespace Solver 
	{
		bool duplicate(
			const Board& b,
			uMap& closed,
			const int& n);

		void addQueue(
			const Board& b,
			const Board& p,
			pQueue& open,
			uMap& closed,
			const int& n);

		void addMoves(
			const Board& b,
			pQueue& open,
			uMap& closed,
			const int& n);

		void printBoard(
			const Board& b,
			const int& n);

		std::pair<int, double> reverseSolution(
			Board b,
			uMap& closed,
			const int& interval,
			const double& t,
			const bool& solve,
			const int& n);

		void writeBoard(
			const Board& b,
			const int& n);

		void cleanup(
			pQueue& open,
			uMap& closed);

		std::pair<int, double> solve(
			const int& interval,
			const bool& solve,
			const int& n);
	}
}