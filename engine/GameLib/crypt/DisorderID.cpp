#include "DisorderID.h"
#include <math.h>
#include "gmp.h"

#define PRIME_NUM_P 757
#define PRIME_NUM_Q 1321
#define MAX_ID_N (PRIME_NUM_P * PRIME_NUM_Q)
#define E1 13 // e1与(p-1)*(q-1)互质

int DisorderID::generate(int originId) {
	if (originId <= 0 || originId >= MAX_ID_N) {
		return -1;
	}
	mpz_t bigOriginId;
	mpz_init(bigOriginId);
	mpz_set_ui(bigOriginId, originId);

	mpz_t bigPow;
	mpz_init(bigPow);
	mpz_pow_ui(bigPow, bigOriginId, E1);
	
	mpz_t finalId;
	mpz_init(finalId);
	mpz_mod_ui(finalId, bigPow, MAX_ID_N); 

	// finalId = mod(pow(originId, E1), MAX_ID_N)
	return mpz_get_ui(finalId);

}
