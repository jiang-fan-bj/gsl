#include <config.h>
#include <stdlib.h>
#include <gsl_rng.h>

/* This is the old BSD rand() generator. The sequence is

         x_{n+1} = (a x_n + c) mod m 

   with a = 1103515245, c = 12345 and m = 2^31 = 2147483648. The seed
   specifies the initial value, x_0.

   It is not very good. The low bits of successive numbers are
   correlated. */

unsigned long int bad_rand_get (void * vstate);
void bad_rand_set (void * state, unsigned int s);
void bad_rand_set_with_state (void * vstate, const void * vinit_state,
			 unsigned int s);

static const unsigned long int m = 2147483648UL ;
static const int a = 1103515245 ;
static const int q = 2 ;
static const int r = -59546842 ;
static const int c = 12345 ;

typedef struct {
  unsigned int x;
} bad_rand_state_t ;

unsigned long int bad_rand_get (void *vstate)
{
    bad_rand_state_t * state = (bad_rand_state_t *)vstate;

    const unsigned int x = state->x ;

    const int h  = x / q;    
    const int t = a * (x - h * q) - h * r + c;

    if (t < 0) 
      {
	state->x = t + m;
      }
    else
      {
	state->x = t ;
      }

    return state->x;
}


void bad_rand_set(void * vstate, unsigned int s)
{
  bad_rand_state_t * state = (bad_rand_state_t *) vstate;
  
  state->x = s ;

  return;
}

static const gsl_rng_type bad_rand_type = { "bad-rand",  /* name */
					     2147483648UL,  /* RAND_MAX */
					     sizeof(bad_rand_state_t), 
					     &bad_rand_set, 
					     &bad_rand_get } ;

const gsl_rng_type * gsl_rng_bad_rand = &bad_rand_type ;

