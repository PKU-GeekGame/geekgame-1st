
//
//	Defines
//
typedef	signed char			S8;
typedef	signed short		S16;
typedef	signed int			S32;
typedef	unsigned char		U8;
typedef	unsigned short		U16;
typedef	unsigned int		U32;
#if defined(WIN32)
typedef signed __int64		S64;
typedef unsigned __int64	U64;
#else
typedef signed long long	S64;
typedef unsigned long long	U64;
#endif


void RandomInit();
void RandomSeed(U64 seed);
void RandomSeed2(U64 seed);
U64	Random(U32 in);
U64	Random2(U32 in);
