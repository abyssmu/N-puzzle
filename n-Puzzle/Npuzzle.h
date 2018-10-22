#include "Headers.h"

namespace Npuzzle
{
	using i64 = std::uint_fast64_t;

	namespace Boards
	{
		using Board = std::vector<int>;

		int linear(
			const Board& b,
			const int& n)
		{
			auto count = 0;

			Board inCol(n * n), inRow(n * n);

			for (auto y = 0; y < n; ++y)
			{
				for (auto x = 0; x < n; ++x)
				{
					auto i = y * n + x;

					if (b[i] == 0)
					{
						continue;
					}

					auto bX = 0;
					auto bY = 0;

					if (b[i] % n == 0)
					{
						bX = n - 1;
						bY = b[i] / n - 1;
					}
					else
					{
						bX = b[i] % n - 1;
						bY = b[i] / n;
					}

					inCol[i] = (bX == x);
					inRow[i] = (bY == y);
				}
			}

			for (auto y = 0; y < n; ++y)
			{
				for (auto x = 0; x < n; ++x)
				{
					auto i = y * n + x;

					if (b[i] == 0)
					{
						continue;
					}

					if (inCol[i])
					{
						for (auto z = y; z < n; ++z)
						{
							auto j = z * n + x;

							if (b[j] == 0)
							{
								continue;
							}

							if (inCol[j])
							{
								if ((b[j] < b[i]) && ((abs(b[j] - b[i]) % n) == 0))
								{
									++count;
								}
							}
						}
					}

					if (inRow[i])
					{
						auto bI = b[i];

						for (auto z = x + 1; z < n; ++z)
						{
							auto j = y * n + z;
							auto bJ = b[j];

							if (b[j] == 0)
							{
								continue;
							}

							if (inRow[j])
							{
								if ((bJ < bI) && (0 <= (bI - bJ)) && ((bI - bJ) < n))
								{
									++count;
								}
							}
						}
					}
				}
			}

			return 2 * count;
		}

		int manhattan(
			const Board& b,
			const int& n)
		{
			auto m = 0;

			Board solution(n * n);
			std::iota(solution.begin(), solution.end(), 1);

			solution[n * n - 1] = 0;

			//Calculate manhattan distance for each value
			for (auto i = 0; i < n * n; ++i)
			{
				if (b[i] != solution[i])
				{
					auto bX = 0;
					auto bY = 0;
					auto x = 0;
					auto y = 0;

					if (b[i] == 0)
					{
						++i;
					}

					//Calculate goal pos
					if ((b[i] % n) == 0)
					{
						bX = n - 1;
						bY = b[i] / n - 1;
					}
					else
					{
						bX = b[i] % n - 1;
						bY = b[i] / n;
					}

					//Calculate the current pos
					auto val = i + 1;

					if ((val % n) == 0)
					{
						x = n - 1;
						y = val / n - 1;
					}
					else
					{
						x = val % n - 1;
						y = val / n;
					}

					m += abs(bX - x) + abs(bY - y);
				}
			}

			return m;
		}

		int heuristic(
			const Board& b,
			const int& n)
		{
			return manhattan(b, n) + linear(b, n);
		}
	}

	namespace Structures
	{
		struct Point
		{
			int x, y;
		};

		struct Container
		{
			int heuristic;
			Boards::Board board;
		};

		struct LessThanByHeur
		{
			bool operator()(
				const Container* lhs,
				const Container* rhs) const
			{
				return lhs->heuristic > rhs->heuristic;
			}
		};
	}

	//using set = std::set<Structures::Container*, Structures::LessThanByHeur>;
	using set = std::priority_queue<Structures::Container*, std::vector<Structures::Container*>, Structures::LessThanByHeur>;
	using map = std::unordered_map<i64, i64>;

	Structures::Point findZero(
		const Boards::Board& b,
		const int& n)
	{
		for (auto i = 0; i < n * n; ++i)
		{
			if (b[i] == 0)
			{
				return { i % n, i / n };
			}
		}

		return { -1, -1 };
	}

	bool isEven(const int& i)
	{
		return (i % 2 == 0);
	}

	int inversions(
		const Boards::Board& b,
		const int& n)
	{
		auto count = 0;

		for (auto i = 0; i < n * n - 1; ++i)
		{
			for (auto j = i + 1; j < n * n; ++j)
			{
				if ((b[i] == 0) || (b[j] == 0))
				{
					continue;
				}

				if (b[j] < b[i])
				{
					++count;
				}
			}
		}

		return count;
	}

	bool solvable(
		const Boards::Board& b,
		const int& n)
	{
		auto count = inversions(b, n);

		if (!isEven(n))
		{
			return isEven(count);
		}

		return (isEven(findZero(b, n).y) != isEven(count));
	}

	Boards::Board createBoard(
		const int& n)
	{
		auto b = Boards::Board(n * n);
		auto rng = std::mt19937_64(std::random_device()());

		//Fill vector from 0 to n * n
		std::iota(b.begin(), b.end(), 0);

		do
		{
			std::shuffle(b.begin(), b.end(), rng);
		} while (!solvable(b, n));

		return b;
	}

	Boards::Board decode(
		i64& code,
		const int& n)
	{
		static Boards::Board b(n * n);

		for (auto i = (n * n) - 1; i >= 0; --i)
		{
			auto val = 0;

			//Get first n bits
			val = code & ((1 << n) - 1);

			//Delete first n bits
			code = code >> n;

			//Save val in board
			b[i] = val;
		}

		return b;
	}

	i64 encode(
		const Boards::Board& b,
		const int& n)
	{
		i64 code = 0;

		for (auto i = 0; i < n * n; ++i)
		{
			//Set first n bits
			if (i == 0)
			{
				code |= b[i];
			}
			//Set rest of bits
			else
			{
				code = ((code << n) | b[i]);
			}
		}

		return code;
	}

	Boards::Board swapPos(
		const Boards::Board& b,
		const int& n,
		const Structures::Point& zero,
		const int& newPos)
	{
		auto oldPos = 0;
		Boards::Board move(n * n);

		//Calculate old pos
		oldPos = zero.x + (zero.y * n);

		//Copy current board
		for (auto i = 0; i < n * n; ++i)
		{
			move[i] = b[i];
		}

		//Swap pos
		move[oldPos] = move[newPos];
		move[newPos] = 0;

		return move;
	}

	Boards::Board down(
		const Boards::Board& b,
		const int& n)
	{
		Structures::Point zero = findZero(b, n);
		auto newPos = zero.y + 1;

		//Check if move is possible
		if (newPos > (n - 1))
		{
			return Boards::Board(0);
		}

		//Create new board based on newPos
		return swapPos(b, n, zero, zero.x + (newPos * n));
	}

	Boards::Board left(
		const Boards::Board& b,
		const int& n)
	{
		Structures::Point zero = findZero(b, n);
		auto newPos = zero.x - 1;

		//Check if move is possible
		if (newPos < 0)
		{
			return Boards::Board(0);
		}

		//Create new board based on newPos
		return swapPos(b, n, zero, newPos + (zero.y * n));
	}

	Boards::Board right(
		const Boards::Board& b,
		const int& n)
	{
		Structures::Point zero = findZero(b, n);
		auto newPos = zero.x + 1;

		//Check if move is possible
		if (newPos > (n - 1))
		{
			return Boards::Board(0);
		}

		//Create new board based on newPos
		return swapPos(b, n, zero, newPos + (zero.y * n));
	}

	Boards::Board up(
		const Boards::Board& b,
		const int& n)
	{
		Structures::Point zero = findZero(b, n);
		auto newPos = zero.y - 1;

		//Check if move is possible
		if (newPos < 0)
		{
			return Boards::Board(0);
		}

		//Create new board based on newPos
		return swapPos(b, n, zero, zero.x + (newPos * n));
	}

	bool duplicate(
		const Boards::Board& b,
		map& closed,
		const int& n)
	{
		return closed.count(encode(b, n));
	}

	void addQueue(
		const Boards::Board& b,
		const Boards::Board& parent,
		set& open,
		map& closed,
		const int& n)
	{
		auto c = new Structures::Container;

		c->board = b;
		c->heuristic = Boards::heuristic(b, n);

		open.emplace(c);

		closed.insert({ encode(b, n), encode(parent, n) });
	}

	void addMoves(
		const Boards::Board& b,
		set& open,
		map& closed,
		const int& n)
	{
		auto moves = std::vector<Boards::Board>(4);
		auto parent = b;

		moves[0] = up(b, n);
		moves[1] = down(b, n);
		moves[2] = left(b, n);
		moves[3] = right(b, n);

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

	void printBoard(
		const Boards::Board& b,
		const int& n)
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

	std::pair<int, double> print(
		i64 b,
		map& closed,
		const double& t,
		const bool& solve,
		const int& n)
	{
		std::vector<Boards::Board> solution;

		do
		{
			auto p = b;

			solution.push_back(decode(b, n));

			b = closed[p];
		} while (b != 0);

		auto size = int(solution.size() - 1);

		if (solve)
		{
			for (auto i = size; i >= 0; --i)
			{
				printBoard(solution[i], n);

				std::this_thread::sleep_for(std::chrono::milliseconds(25));

				if (i != 0)
				{
					system("CLS");
				}
			}
		}

		return { size + 1, t };
	}

	void writeBoard(
		const Boards::Board& b,
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

	void cleanup(
		set& open,
		map& closed)
	{
		while (!open.empty())
		{
			delete open.top();

			open.pop();
		}

		closed.clear();
	}

	std::pair<int, double> solve(
		const bool& solve,
		const int& n)
	{
		//Open list contains all unexplored nodes, sorted by heuristic value
		Npuzzle::set open;

		//Closed list contains all explored nodes, with values set to encoded parent board
		Npuzzle::map closed;

		auto curr = Npuzzle::createBoard(n);
		//curr = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 0, 15 };

		addQueue(curr, Boards::Board(n * n), open, closed, n);

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
		auto results = print(encode(curr, n), closed, t.count(), solve, n);

		cleanup(open, closed);

		return results;
	}
}