/* Plain Monte-Carlo. */

/* Author: MJB */
/* RCS: $Id$ */

#ifndef GSL_MONTE_PLAIN_H
#define GSL_MONTE_PLAIN_H

#include <gsl_monte.h>
#include <gsl_rng.h>

typedef struct {
  /* control variables */
  double acc;

  int init_done;
  int check_done;
  int verbose;

  gsl_rng* ranf;

} gsl_monte_plain_state;

int gsl_monte_plain(const gsl_monte_plain_state *state, 
		    const gsl_monte_f_T fun, 
		    const double* xl, const double* xu, const size_t num_dim, 
		    const size_t calls, double* res, double* err);

gsl_monte_plain_state* gsl_monte_plain_alloc(void);

int gsl_monte_plain_validate(gsl_monte_plain_state* state,
                             double xl[], double xu[], 
                             unsigned long num_dim, unsigned long calls);

int gsl_monte_plain_init(gsl_monte_plain_state* state);

void gsl_monte_plain_free (gsl_monte_plain_state* s);

#endif /* GSL_MONTE_PLAIN_H */
