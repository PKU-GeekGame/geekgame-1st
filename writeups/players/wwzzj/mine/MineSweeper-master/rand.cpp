#include "stdafx.h"


//	Setting seeds:
//	Use of KISS or KISS() as a general 64 - bit RNG requires specifying
//	3 * 64 + 58 = 250 bits for seeds, 64 bits each for x, y, z and 58 for c,
//	resulting in a composite sequence with period around 2 ^ 250.
//	The actual period is
//	(2 ^ 250 + 2 ^ 192 + 2 ^ 64 - 2 ^ 186 - 2 ^ 129) / 6 ~= 2 ^ (247.42) or 10 ^ (74.48).
//	We "lose" 1 + 1.58 = 2.58 bits from maximum possible period, one bit
//	because b = 2 ^ 64, a square, cannot be a primitive root of p = ab - 1,
//	so the best possible order for b is (p - 1) / 2.
//	The periods of MWC and  XSH have gcd 3 = 2 ^ 1.58, so another  1.58
//	bits  are "lost" from the best possible period we could expect
//	from 250 seed bits.
//	
//	Some users may think 250 seed bits are an unreasonable requirement.
//	A good seeding procedure might be to assume the default seed
//	values then let the user choose none, one, two, ..., or all
//	of x, y, z, and c to be reseeded.
//
static U64 kiss_x = 1234567890987654321ULL;
static U64 kiss_c = 123456123456123456ULL;
static U64 kiss_y = 362436362436362436ULL;
static U64 kiss_z = 1066149217761810ULL;
static U64 kiss_t = 0;

static U64 kiss2_x = 1234567890987654321ULL;
static U64 kiss2_c = 123456123456123456ULL;
static U64 kiss2_y = 362436362436362436ULL;
static U64 kiss2_z = 1066149217761810ULL;
static U64 kiss2_t = 0;


void RandomSeed(U64 seed)
{
	kiss_x = seed | 1;
	kiss_c = seed | 2;
	kiss_y = seed | 4;
	kiss_z = seed | 8;
	kiss_t = 0;
}


void RandomSeed2(U64 seed)
{
	kiss2_x = seed | 1;
	kiss2_c = seed | 2;
	kiss2_y = seed | 4;
	kiss2_z = seed | 8;
	kiss2_t = 0;
}


U64 Random(U32 in)
{
	// multiply with carry
	kiss_t = (kiss_x << 58) + kiss_c;
	kiss_c = (kiss_x >> 6);
	kiss_x += kiss_t;
	kiss_c += (kiss_x < kiss_t ? 1 : 0);
	// XOR shift
	kiss_y ^= (kiss_y << 13);
	kiss_y ^= (kiss_y >> 17);
	kiss_y ^= (kiss_y << 43);
	// Congruential
	kiss_z = 6906969069LL * kiss_z + 1234567;
	U64 rand = kiss_x + kiss_y + kiss_z;
	if (in == 0)
		return rand;
	else
		return ((rand & 0xFFFFFFFF) * in) >> 32;
}


U64 Random2(U32 in)
{
	// multiply with carry
	kiss2_t = (kiss2_x << 58) + kiss2_c;
	kiss2_c = (kiss2_x >> 6);
	kiss2_x += kiss2_t;
	kiss2_c += (kiss2_x < kiss2_t ? 1 : 0);
	// XOR shift
	kiss2_y ^= (kiss2_y << 13);
	kiss2_y ^= (kiss2_y >> 17);
	kiss2_y ^= (kiss2_y << 43);
	// Congruential
	kiss2_z = 6906969069LL * kiss2_z + 1234567;
	U64 rand = kiss2_x + kiss2_y + kiss2_z;
	if (in == 0)
		return rand;
	else
		return ((rand & 0xFFFFFFFF) * in) >> 32;
}


void	RandomInit(void)
{
	SYSTEMTIME C_time;
	__int64 C_z, C_aux;

	// Make it depend on the date and tick count
	GetSystemTime(&C_time);

	C_aux = C_time.wYear;
	C_z = C_aux << (22 + 6 + 6 + 5 + 5 + 4);

	C_aux = C_time.wMonth;
	C_z += C_aux << (22 + 6 + 6 + 5 + 5);

	C_aux = C_time.wDay;
	C_z += C_aux << (22 + 6 + 6 + 5);

	C_aux = C_time.wHour;
	C_z += C_aux << (22 + 6 + 6);

	C_aux = C_time.wMinute;
	C_z += C_aux << (22 + 6);

	C_aux = C_time.wSecond;
	C_z += C_aux << 22;

	C_aux = GetTickCount();
	C_z += C_aux;
	RandomSeed((U64)C_z);
	RandomSeed2((U64)C_z);
}

