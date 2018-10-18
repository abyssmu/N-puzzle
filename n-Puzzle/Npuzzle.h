#include <assert.h>
#include <cstdint>
#include <numeric>
#include <random>
#include <vector>

namespace Npuzzle
{
	struct Point
	{
		int x, y;
	};

	struct Container
	{
		int heuristic;
		std::uint_fast64_t code;
	};

	struct GreaterThanByHeur
	{
		bool operator()(
			const Container* lhs,
			const Container* rhs)
		{
			return lhs->heuristic > rhs->heuristic;
		}
	};

	Point findZero(
		const std::vector<int> b,
		const int n)
	{
		for (int i = 0; i < n * n; ++i)
		{
			if (b[i] == 0)
			{
				return { i % n, i / n };
			}
		}

		return { -1, -1 };
	}

	//Count inversions in board
	int countInv(
		const std::vector<int> b,
		const int n)
	{
		auto count = 0;

		for (auto i = 0; i < n * n - 1; ++i)
		{
			for (int j = i + 1; j < n * n; ++j)
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

	bool checkSolvable(
		const std::vector<int> b,
		const int n)
	{
		Point zero = findZero(b, n);
		int count = countInv(b, n);

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

	std::vector<int> createBoard(
		const int n)
	{
		std::vector<int> board(n * n);
		std::mt19937_64 rng = std::mt19937_64(std::random_device()());

		do
		{
			//Fill vector from 0 to n * n
			std::iota(board.begin(), board.end(), 0);

			//Randomize vector
			std::shuffle(board.begin(), board.end(), rng);

		} while (!checkSolvable(board, n));

		return board;
	}

	std::vector<int> decode(
		std::uint_fast64_t code,
		const int n)
	{
		static std::vector<int> b(n * n);

		for (int i = (n * n) - 1; i >= 0; --i)
		{
			int val = 0;

			//Get first n bits
			val = code & ((1 << n) - 1);

			//Delete first n bits
			code = code >> n;

			//Save val in board
			b[i] = val;
		}

		return b;
	}

	std::vector<int> swapPos(
		const std::vector<int> b,
		const int n,
		const Point zero,
		const int newPos)
	{
		int oldPos;
		std::vector<int> move(n * n);

		//Calculate old pos
		oldPos = zero.x + (zero.y * n);

		//Copy current board
		for (int i = 0; i < n * n; ++i)
		{
			move[i] = b[i];
		}

		//Swap pos
		move[oldPos] = move[newPos];
		move[newPos] = 0;

		return move;
	}

	std::vector<int> down(
		const std::vector<int> b,
		const int n)
	{
		Point zero = findZero(b, n);
		int newPos = zero.y + 1;

		//Check if move is possible
		if (newPos > (n - 1))
		{
			return std::vector<int>(0);
		}

		//Create new board based on newPos
		return swapPos(b, n, zero, zero.x + (newPos * n));
	}

	std::uint_fast64_t encode(
		const std::vector<int> b,
		const int n)
	{
		std::uint_fast64_t code = 0;

		for (int i = 0; i < n * n; ++i)
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
		const std::vector<int> b,
		const int n)
	{
		auto conflicts = 0;

		std::vector<bool> inCol(n * n);
		std::vector<bool> inRow(n * n);

		for (auto y = 0; y < n; ++y)
		{
			for (auto x = 0; x < n; ++x)
			{
				auto i = y * n + x;

				auto bX = b[i] % n;
				auto bY = b[i] / n;

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

						if (inCol[j] && (b[j] < b[i]))
						{
							++conflicts;
						}
					}
				}

				if (inRow[i])
				{
					for (auto z = x; z < n; ++z)
					{
						auto j = z * n + x;

						if (b[j] == 0)
						{
							continue;
						}

						if (inRow[j] && (b[j] < b[i]))
						{
							++conflicts;
						}
					}
				}
			}
		}

		return 2 * conflicts;
	}

	int manhattan(
		const std::vector<int> b,
		const int n)
	{
		int m = 0;

		std::vector<int> solution(n * n);
		std::iota(solution.begin(), solution.end(), 1);

		solution[n * n - 1] = 0;

		//Calculate manhattan distance for each value
		for (int i = 0; i < n * n; ++i)
		{
			if (b[i] != solution[i])
			{
				int bX, bY, x, y;

				//Calculate goal pos
				if (b[i] == 0)
				{
					bX = n - 1;
					bY = n - 1;
				}
				else
				{
					bX = b[i] % n;
					bY = b[i] / n;
				}

				//Calculate the current pos
				x = i % n;
				y = i / n;

				m += abs(bX - x) + abs(bY - y);
			}
		}

		return m;
	}
	
	int heuristic(
		const std::vector<int> b,
		const int n)
	{
		return manhattan(b, n) + linear(b, n);
	}

	std::vector<int> left(
		const std::vector<int> b,
		const int n)
	{
		Point zero = findZero(b, n);
		int newPos = zero.x - 1;

		//Check if move is possible
		if (newPos < 0)
		{
			return std::vector<int>(0);
		}

		//Create new board based on newPos
		return swapPos(b, n, zero, newPos + (zero.y * n));
	}

	std::vector<int> right(
		const std::vector<int> b,
		const int n)
	{
		Point zero = findZero(b, n);
		int newPos = zero.x + 1;

		//Check if move is possible
		if (newPos > (n - 1))
		{
			return std::vector<int>(0);
		}

		//Create new board based on newPos
		return swapPos(b, n, zero, newPos + (zero.y * n));
	}

	std::vector<int> up(
		const std::vector<int> b,
		const int n)
	{
		Point zero = findZero(b, n);
		int newPos = zero.y - 1;

		//Check if move is possible
		if (newPos < 0)
		{
			return std::vector<int>(0);
		}

		//Create new board based on newPos
		return swapPos(b, n, zero, zero.x + (newPos * n));
	}
}