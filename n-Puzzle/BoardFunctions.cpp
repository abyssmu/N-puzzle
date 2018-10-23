#include "Npuzzle.h"

#include <algorithm>
#include <numeric>
#include <random>

Npuzzle::Point Npuzzle::findZero(
	const Board& b,
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

bool Npuzzle::BoardFunctions::isEven(
	const int& i)
{
	return (i % 2 == 0);
}

int Npuzzle::BoardFunctions::inversions(
	const Board& b,
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

bool Npuzzle::BoardFunctions::solvable(
	const Board& b,
	const int& n)
{
	auto count = inversions(b, n);

	if (!isEven(n))
	{
		return isEven(count);
	}

	return (isEven(findZero(b, n).second) != isEven(count));
}

Npuzzle::Board Npuzzle::BoardFunctions::createBoard(
	const int& n)
{
	auto b = Board(n * n);
	auto rng = std::mt19937_64(std::random_device()());

	//Fill vector from 0 to n * n
	std::iota(b.begin(), b.end(), 0);

	do
	{
		std::shuffle(b.begin(), b.end(), rng);
	} while (!solvable(b, n));

	return b;
}

int Npuzzle::BoardFunctions::linear(
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

int Npuzzle::BoardFunctions::manhattan(
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

int Npuzzle::BoardFunctions::heuristic(
	const Board& b,
	const int& n)
{
	return manhattan(b, n) + linear(b, n);
}

Npuzzle::Board Npuzzle::BoardFunctions::swapPos(
	const Board& b,
	const Point& oldPos,
	const Point& newPos,
	const int& n)
{
	Board move = b;
	std::swap(move[oldPos.first + oldPos.second * n],
		move[newPos.first + newPos.second * n]);

	return move;
}

bool Npuzzle::BoardFunctions::between(
	const int& a,
	const int& b,
	const int& c)
{
	return ((a < c) && (c < b));
}

Npuzzle::Board Npuzzle::BoardFunctions::move(
	const Board& b,
	const Point& offset,
	const Point& oldZero,
	const int& n)
{
	auto newPos = Point(oldZero.first + offset.first, oldZero.second + offset.second);

	if (!between(-1, n, newPos.first) || !between(-1, n, newPos.second))
	{
		return Board(0);
	}

	return swapPos(b, oldZero, newPos, n);
}