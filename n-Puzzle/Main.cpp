#include "Npuzzle.h"

int getInterval();
void setMinMax(
	std::pair<int, double>& p,
	std::pair<int, double>& min,
	std::pair<int, double>& max);
void solveWatch(std::pair<bool, int> info);
std::pair<bool, int> userInput();

int main()
{
	auto info = userInput();

	if (info.first)
	{
		auto interval = getInterval();

		auto p = Npuzzle::Solver::solve(interval, info.first, info.second);

		std::cout << p.first << " steps in\t" << p.second << "s." << std::endl;
	}
	else
	{
		solveWatch(info);
	}
	
	std::cin.get();
	std::cin.get();

	return 0;
}

int getInterval()
{
	auto interval = 0;

	std::cout << "How long between moves? - t(ms) = ";
	std::cin >> interval;

	system("CLS");

	return interval;
}

void setMinMax(
	std::pair<int, double>& p,
	std::pair<int, double>& min,
	std::pair<int, double>& max)
{
	if (p.first < min.first)
	{
		min.first = p.first;
	}

	if (p.second < min.second)
	{
		min.second = p.second;
	}

	if (p.first > max.first)
	{
		max.first = p.first;
	}

	if (p.second > max.second)
	{
		max.second = p.second;
	}
}

void solveWatch(std::pair<bool, int> info)
{
	auto trials = 0;
	auto totalSteps = 0;
	auto totalT = 0.0;
	auto min = std::pair<int, double>(INT_MAX, INT_MAX);
	auto max = std::pair<int, double>(0, 0);

	system("CLS");

	std::cout << "How many trials? ";
	std::cin >> trials;

	system("CLS");

	for (auto i = 0; i < trials; ++i)
	{
		auto p = Npuzzle::Solver::solve(0, info.first, info.second);

		std::cout << p.first << " steps in\t" << p.second << "s." << std::endl;

		setMinMax(p, min, max);

		totalSteps += p.first;
		totalT += p.second;
	}

	std::cout << "Average steps: " << totalSteps / trials << std::endl;
	std::cout << "Average time: " << totalT / trials << std::endl;
	std::cout << "Min steps: " << min.first << std::endl;
	std::cout << "Min time: " << min.second << std::endl;
	std::cout << "Max steps: " << max.first << std::endl;
	std::cout << "Max time: " << max.second << std::endl;
}

std::pair<bool, int> userInput()
{
	auto n = 0;

	do
	{
		if (n > 4)
		{
			system("CLS");

			std::cout << "Board size cannot be greater than 4." << std::endl << "This functionality has not been added yet." << std::endl;
		}

		std::cout << "Input size of board: ";
		std::cin >> n;
	} while (n > 4);

	system("CLS");

	auto solve = false;
	auto go = true;

	while (go)
	{
		auto input = ' ';

		std::cout << "Do you want to watch the solve (y/n)? ";
		std::cin >> input;

		if (input == 'y')
		{
			solve = true;
			go = false;
		}
		else if (input == 'n')
		{
			solve = go = false;
		}
	}

	system("CLS");

	return { solve, n };
}