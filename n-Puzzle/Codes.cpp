#include "Npuzzle.h"

Npuzzle::Board Npuzzle::Codes::decode(
	i64& code,
	const int& n)
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

Npuzzle::i64 Npuzzle::Codes::encode(
	const Board& b,
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