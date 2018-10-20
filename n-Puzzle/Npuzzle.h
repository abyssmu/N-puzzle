#include <assert.h>
#include <cstdint>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <unordered_map>
#include <vector>

namespace Npuzzle
{
	using Board = std::vector<int>;
	using i64 = std::uint_fast64_t;

	namespace Structures
	{
		struct Point
		{
			int x, y;
		};

		struct Container
		{
			int heuristic;
			Board board;
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
		const Board b,
		const int n)
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

	//Count inversions in board
	int inversions(
		const Board b,
		const int n)
	{
		auto count = 0;

		for (auto i = 0; i < n * n - 1; ++i)
		{
			for (auto j = i + 1; j < n * n; ++j)
			{
				if (b[i] == 0)
				{
					if (b[j] < n * n)
					{
						++count;
					}
				}
				else if (b[j] < b[i])
				{
					++count;
				}
			}
		}

		return count;
	}

	bool solvable(
		const Board b,
		const int n)
	{
		auto zero = findZero(b, n);
		auto count = inversions(b, n);

		//If width is odd and count is even
		if ((n & 1) && !(count & 1))
		{
			return true;
		}
		//If width is even
		else
		{
			//If zero y pos is odd from bottom, and count is even
			if (((n - zero.y) & 1) && !(count & 1))
			{
				return true;
			}
			else if (count & 1)
			{
				return true;
			}
		}

		return false;
	}

	Board createBoard(
		const int n)
	{
		auto b = Board(n * n);
		auto rng = std::mt19937_64(std::random_device()());

		do
		{
			//Fill vector from 0 to n * n
			std::iota(b.begin(), b.end(), 0);

			//Randomize vector
			std::shuffle(b.begin(), b.end(), rng);

		} while (!solvable(b, n));

		return b;
	}

	Board decode(
		i64& code,
		const int n)
	{
		static Board b(n * n);

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
		const Board b,
		const int n)
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

	int linear(
		const Board b,
		const int n)
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
		const Board b,
		const int n)
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
		const Board b,
		const int n)
	{
		return manhattan(b, n) + linear(b, n);
	}

	Board swapPos(
		const Board b,
		const int n,
		const Structures::Point zero,
		const int newPos)
	{
		auto oldPos = 0;
		Board move(n * n);

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

	Board down(
		const Board b,
		const int n)
	{
		Structures::Point zero = findZero(b, n);
		auto newPos = zero.y + 1;

		//Check if move is possible
		if (newPos > (n - 1))
		{
			return Board(0);
		}

		//Create new board based on newPos
		return swapPos(b, n, zero, zero.x + (newPos * n));
	}

	Board left(
		const Board b,
		const int n)
	{
		Structures::Point zero = findZero(b, n);
		auto newPos = zero.x - 1;

		//Check if move is possible
		if (newPos < 0)
		{
			return Board(0);
		}

		//Create new board based on newPos
		return swapPos(b, n, zero, newPos + (zero.y * n));
	}

	std::vector<int> right(
		const Board b,
		const int n)
	{
		Structures::Point zero = findZero(b, n);
		auto newPos = zero.x + 1;

		//Check if move is possible
		if (newPos > (n - 1))
		{
			return Board(0);
		}

		//Create new board based on newPos
		return swapPos(b, n, zero, newPos + (zero.y * n));
	}

	Board up(
		const Board b,
		const int n)
	{
		Structures::Point zero = findZero(b, n);
		auto newPos = zero.y - 1;

		//Check if move is possible
		if (newPos < 0)
		{
			return Board(0);
		}

		//Create new board based on newPos
		return swapPos(b, n, zero, zero.x + (newPos * n));
	}
}