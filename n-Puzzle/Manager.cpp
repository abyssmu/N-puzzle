#include "Manager.h"

Manager::Manager()
{
	int nums[n * n];

	zeroX = zeroY = 0;

	current = new Container;

	//Initialize random number generator
	srand((unsigned int)time(0));

	do
	{
		//Initialize nums
		for (int i = 0; i < n * n; ++i)
		{
			nums[i] = 0;
		}

		int val = -1;

		for (int i = 0; i < n * n; ++i)
		{
			bool go = true;

			val = rand() % (n * n);

			//Loop until nums[val] != -1
			while (go)
			{
				if (nums[val] == -1)
				{
					val = rand() % (n * n);
				}
				else
				{
					go = false;
				}
			}

			current->b[i] = val;

			nums[val] = -1;

			//set position of zero
			if (val == 0)
			{
				zeroX = i % n;
				zeroY = i / n;
			}
		}
	} while (!checkSolvable());

	current->manhattan = calculateManhattan(current->b);

	open.push(current);
}

Manager::Manager(const Manager& other)
{}

Manager::~Manager()
{}

void Manager::Run()
{
	start = std::chrono::system_clock::now();

	bool solved = false;

	while (!solved)
	{
		//Check if open.top is solved
		if (open.top()->manhattan == 0)
		{
			end = std::chrono::system_clock::now();

			solved = true;

			std::cout << "Solved.";

			printSolution(open.top());

			return;
		}
		
		//Add open.top to closed
		closed[encode(current->b)] = true;

		//Add moves to open
		addMoves();	
	}
}

//Calculate manhattan value for board
int Manager::calculateManhattan(int b[])
{
	//Create solved board
	int manhattan = 0;

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

			manhattan += abs(bX - x) + abs(bY - y);
		}
	}

	return manhattan;
}

//Check if board is solvable
bool Manager::checkSolvable()
{
	int count = 0;
	int pos = 0;

	//Assume board is not solvable
	bool solvable = false;

	for (int i = 0; i < n * n; ++i)
	{
		for (int j = i + 1; j < n * n; ++j)
		{
			if (current->b[j] < current->b[i])
			{
				++count;
			}
		}
	}

	//If width is odd and count is even
	if ((n % 2 == 1) && (count % 2 == 0))
	{
		solvable = true;
	}
	//If width is even, zeroY pos is odd from bottom, and count is even
	else if (((n - zeroY) % 2 == 1) && (count % 2 == 0))
	{
		solvable = true;
	}
	//If width is even, zeroY pos is even from bottom, and count is odd
	else if (count % 2 == 1)
	{
		solvable = true;
	}

	return solvable;
}

//Check if current board is solution
bool Manager::checkSolution()
{
	for (int i = 0; i < n * n; ++i)
	{
		if (current->b[i] != i)
		{
			return false;
		}
	}

	return true;
}

//Encode binary board
UINT64 Manager::encode(int b[])
{
	UINT64 code = 0;

	for (int i = 0; i < n * n; ++i)
	{
		//Set first four bits
		if (i == 0)
		{
			code |= b[i];
		}
		//Set rest of bits
		else
		{
			code = ((code << 4) | b[i]);
		}
	}

	return code;
}

//Decode binary board
int* Manager::decode(UINT64 code)
{
	static int b[n * n];

	for (int i = (n * n) - 1; i >= 0; --i)
	{
		int val = 0;

		//Get first four bits
		val = code & ((1 << 4) - 1);

		//Delete first four bits
		code = code >> 4;

		//Save val in board
		b[i] = val;

		if (val == 0)
		{
			zeroX = i % n;
			zeroY = i / n;
		}
	}

	return b;
}

void Manager::addMoves()
{
	//Set current to open.top
	current = open.top();
	findZero();

	//Create new move
	Container* move = 0;

	//Remove top from open
	open.pop();

	//Check for directional moves
	up();
	down();
	left();
	right();
}

//Y - 1
void Manager::up()
{
	int newPos;
	Container* move = new Container;

	newPos = zeroY - 1;

	//Check if move is possible
	if (newPos < 0)
	{
		return;
	}

	//Calculate new pos
	newPos = zeroX + (newPos * n);

	//Swap positions
	swapPos(move, newPos);

	//Check for duplicate board
	checkDuplicate(move);
}

//Y + 1
void Manager::down()
{
	int newPos;
	Container* move = new Container;

	newPos = zeroY + 1;

	//Check if move is possible
	if (newPos > (n - 1))
	{
		return;
	}

	//Calculate new pos
	newPos = zeroX + (newPos * n);

	//Swap positions
	swapPos(move, newPos);

	//Check for duplicate board
	checkDuplicate(move);
}

//X - 1
void Manager::left()
{
	int newPos;
	Container* move = new Container;

	newPos = zeroX - 1;

	//Check if move is possible
	if (newPos < 0)
	{
		return;
	}

	//Calculate new pos
	newPos = newPos + (zeroY * n);

	//Swap positions
	swapPos(move, newPos);

	//Check for duplicate board
	checkDuplicate(move);
}

//X + 1
void Manager::right()
{
	int newPos;
	Container* move = new Container;

	newPos = zeroX + 1;
	
	//Check if move is possible
	if (newPos > (n - 1))
	{
		return;
	}

	//Calculate new pos
	newPos = newPos + (zeroY * n);

	//Swap positions
	swapPos(move, newPos);

	//Check for duplicate board
	checkDuplicate(move);
}

void Manager::findZero()
{
	for (int i = 0; i < n * n; ++i)
	{
		if (current->b[i] == 0)
		{
			zeroX = i % n;
			zeroY = i / n;
		}
	}
}

void Manager::swapPos(Container* move, int newPos)
{
	int hold, oldPos;

	//Calculate old pos
	oldPos = zeroX + (zeroY * n);

	//Copy current board
	for (int i = 0; i < n * n; ++i)
	{
		move->b[i] = current->b[i];
	}

	//Swap pos
	hold = move->b[newPos];
	move->b[oldPos] = hold;
	move->b[newPos] = 0;
}

bool Manager::checkDuplicate(Container* move)
{
	UINT64 code = encode(move->b);

	//Check if board has been found
	if (closed[code] == true)
	{
		return false;
	}
	else
	{
		//If it hasn't been found, set container values and add to open
		move->manhattan = calculateManhattan(move->b);
		move->parent = current;

		open.push(move);
	}

	return true;
}

void Manager::printSolution(Container* top)
{
	std::chrono::duration<double> t = end - start;

	Container* curr = top;
	bool go = true;
	int steps = 0;

	std::cout << std::endl;

	while (go)
	{
		for (int i = 0; i < n * n; ++i)
		{
			if (i == (n * n - 1))
			{
				std::cout << curr->b[i];
			}
			else
			{
				std::cout << curr->b[i] << ", ";
			}
		}

		std::cout << std::endl;

		if (curr->parent)
		{
			curr = curr->parent;
		}
		else
		{
			go = false;
		}

		++steps;
	}

	std::cout << std::endl;

	std::cout << steps << " steps in " << t.count() << "s.";

	std::cin.get();
}