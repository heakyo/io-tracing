#include <stdio.h>

/*
 * The number of bits in long type.
 * bits per word
 */
#define _BITSET_BITS (sizeof(long) * 8) //  8*8

/*
 * y = _BITSET_BITS
 * x: total bits
 * The number of word to represent these bits.
 * E.G.
 *     if total bits are 65, we need 2 words
 */
#define __howmany(x, y) (((x) + ((y) - 1)) / (y))
#define __bitset_words(_s) (__howmany(_s, _BITSET_BITS))

/*
 * The actual size in bytes
 */
#define BITSET_SIZE(_s) (__bitset_words((_s)) * sizeof(long))

int main(int argc, char *argv[])
{
	printf("Hello BITSET\n");

	printf("_BITSET_BITS: %ld\n", _BITSET_BITS);
	printf("__bitset_words: %ld\n", __bitset_words(65));
	printf("BITSET_SIZE: %ld\n", BITSET_SIZE(1));

	return 0;
}
