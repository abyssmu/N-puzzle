#include <chrono>
#include <iostream>
#include <math.h>
#include <queue>
#include <random>
#include <time.h>
#include <unordered_map>
#include <vector>
#include <Windows.h>

const int n = 4;

struct Container
{
	int b[n * n] = {};
	int heuristic;
	Container* parent = 0;
};

struct GreaterThanByCost
{
	bool operator()(const Container* lhs, const Container* rhs)
	{
		return lhs->heuristic > rhs->heuristic;
	}
};

class Manager
{
public:
	Manager();
	Manager(const Manager& other);
	~Manager();

	void Run();

	int calculateManhattan(int b[]);
	int calculateLinear(int b[]);
	bool checkSolvable();

	UINT64 encode(int b[]);
	int* decode(UINT64 code);

	void addMoves();

	void up();
	void down();
	void left();
	void right();

	void findZero();
	void swapPos(Container* move, int newPos);

	bool checkDuplicate(Container* move);

	void printSolution(Container* top);

private:
	std::priority_queue<Container*, std::vector<Container*>, GreaterThanByCost> open;
	std::unordered_map<UINT64, bool> closed;

	Container* current = 0;
	int zeroX, zeroY;

	std::chrono::system_clock::time_point start;
	std::chrono::system_clock::time_point end;
};