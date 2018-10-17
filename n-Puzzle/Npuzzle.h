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
		std::vector<int> b,
		int n)
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
		std::vector<int> b,
		int n)
	{
		int count = 0;

		for (int i = 0; i < n * n - 1; ++i)
		{
			for (int j = i + 1; j < n * n; ++j)
			{
				if (b[j] < b[i])
				{
					++count;
				}
			}
		}

		return count;
	}

	bool checkSolvable(
		std::vector<int> b,
		int n)
	{
		//Assume board is not solvable
		bool solvable = false;
		int count = 0;

		Point zero = findZero(b, n);
		assert((zero.x != -1) && (zero.y != -1));

		count = countInv(b, n);

		//If width is odd and count is even
		if ((n & 1) && (count & 0))
		{
			return true;
		}
		//If width is even, zeroY pos is odd from bottom, and count is even
		else if (((n - zero.y) & 1) && (count & 0))
		{
			return true;
		}
		//If width is even, zeroY pos is even from bottom, and count is odd
		else if (count & 1)
		{
			return true;
		}

		return solvable;
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
		int n)
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
		std::vector<int> b,
		int n,
		Point zero,
		int newPos)
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
		std::vector<int> b,
		int n)
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
		std::vector<int> b,
		int n)
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
		std::vector<int> b,
		int n)
	{
		int count = 0;

		for (int i = 0; i < n * n; ++i)
		{
			//Check if b[i] is in the right spot
			if (b[i] != i)
			{
				//Calculate row and col it's supposed to be in
				int x = b[i] % n;
				int y = b[i] / n;

				//Calculate row and col it's in
				int bx = i % n;
				int by = i / n;

				//Check cols
				if (x == bx)
				{
					bool found = false;

					//Check above
					if (b[i] < i)
					{
						int colStart = i - n;

						for (int j = colStart; j >= 0; j -= n)
						{
							if ((j != b[i]) && !found)
							{
								if ((b[i] - b[j]) % n == 0)
								{
									++count;
								}
							}
							else if ((j == b[i]) && !found)
							{
								if ((b[i] - b[j]) % n == 0)
								{
									++count;
								}

								found = true;
							}
							else
							{
								break;
							}
						}
					}

					//Check below
					if (b[i] > i)
					{
						int colEnd = n * (n - 1) + bx;

						for (int j = i + 4; j <= colEnd; j += 4)
						{
							if ((j != b[i]) && !found)
							{
								if ((b[i] - b[j]) % n == 0)
								{
									++count;
								}
							}
							else if ((j == b[i]) && !found)
							{
								if ((b[i] - b[j]) % n == 0)
								{
									++count;
								}

								found = true;
							}
							else
							{
								break;
							}
						}
					}
				}

				//Check rows
				if (y == by)
				{
					bool found = false;

					//Check left
					if (b[i] < i)
					{
						int rowStart = i - 1;

						for (int j = rowStart; j >= by * n; --j)
						{
							if ((j != b[i]) && !found)
							{
								if (((b[i] - b[j]) < 0) && (abs(b[i] - b[j]) < n))
								{
									++count;
								}
							}
							else if ((j == b[i]) && !found)
							{
								if (((b[i] - b[j]) < 0) && (abs(b[i] - b[j]) < n))
								{
									++count;
								}

								found = true;
							}
							else
							{
								break;
							}
						}
					}

					//Check right
					if (b[i] > i)
					{
						int nextRowStart = n * (by + 1);

						for (int j = i + 1; j < nextRowStart; ++j)
						{
							if ((j != b[i]) && !found)
							{
								if (((b[i] - b[j]) > 0) && (abs(b[i] - b[j]) < n))
								{
									++count;
								}
							}
							else if ((j == b[i]) && !found)
							{
								if (((b[i] - b[j]) > 0) && (abs(b[i] - b[j]) < n))
								{
									++count;
								}

								found = true;
							}
							else
							{
								break;
							}
						}
					}
				}
			}
		}

		return 2 * count;
	}

	int manhattan(
		std::vector<int> b,
		int n)
	{
		//Create solved board
		int m = 0;

		//Calculate manhattan distance for each value
		for (int i = 0; i < n * n; ++i)
		{
			if (b[i] != i)
			{
				int bX, bY, x, y;

				bX = b[i] % n;
				bY = b[i] / n;

				x = i % n;
				y = i / n;

				m += abs(bX - x) + abs(bY - y);
			}
		}

		return m;
	}
	
	int heuristic(
		std::vector<int> b,
		int n)
	{
		return manhattan(b, n) + linear(b, n);
	}

	std::vector<int> left(
		std::vector<int> b,
		int n)
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
		std::vector<int> b,
		int n)
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
		std::vector<int> b,
		int n)
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