/* Author:  G. Jungman
 * RCS:     $Id$
 */
#include <math.h>
#include <gsl_math.h>
#include <gsl_errno.h>
#include "gsl_sf_gamma.h"
#include "gsl_sf_exp.h"


int gsl_sf_exp_impl(const double x, double * result)
{
  if(x > GSL_LOG_DBL_MAX) {
    *result = 0.0;
    return GSL_EOVRFLW;
  }
  else if(x < GSL_LOG_DBL_MIN) {
    *result = 0.0;
    return GSL_EUNDRFLW;
  }
  else {
    *result = exp(x);
    return GSL_SUCCESS;
  }
}

int gsl_sf_exp_sgn_impl(const double x, const double sgn, double * result)
{
  if(x > GSL_LOG_DBL_MAX) {
    *result = 0.0;
    return GSL_EOVRFLW;
  }
  else if(x < GSL_LOG_DBL_MIN) {
    *result = 0.0;
    return GSL_EUNDRFLW;
  }
  else {
    *result = GSL_SIGN(sgn) * exp(x);
    return GSL_SUCCESS;
  }  
}

int gsl_sf_expm1_impl(const double x, double * result)
{
  const double cut = 0.002;

  if(x < GSL_LOG_DBL_MIN) {
    *result = -1.0;
    return GSL_SUCCESS;
  }
  else if(x < -cut) {
    *result = exp(x) - 1.0;
    return GSL_SUCCESS;
  }
  else if(x < cut) {
    *result = x * (1.0 + 0.5*x*(1.0 + x/3.0*(1.0 + 0.25*x*(1.0 + 0.2*x))));
    return GSL_SUCCESS;
  } 
  else if(x < GSL_LOG_DBL_MAX) {
    *result = exp(x) - 1.0;
    return GSL_SUCCESS;
  }
  else {
    *result = 0.0; /* FIXME: should be Inf */
    return GSL_EOVRFLW;
  }
}


int gsl_sf_exprel_impl(const double x, double * result)
{
  const double cut = 0.002;

  if(x < GSL_LOG_DBL_MIN) {
    *result = -1.0/x;
    return GSL_SUCCESS;
  }
  else if(x < -cut) {
    *result = (exp(x) - 1.0)/x;
    return GSL_SUCCESS;
  }
  else if(x < cut) {
    *result = (1.0 + 0.5*x*(1.0 + x/3.0*(1.0 + 0.25*x*(1.0 + 0.2*x))));
    return GSL_SUCCESS;
  } 
  else if(x < GSL_LOG_DBL_MAX) {
    *result = (exp(x) - 1.0)/x;
    return GSL_SUCCESS;
  }
  else {
    *result = 0.0; /* FIXME: should be Inf */
    return GSL_EOVRFLW;
  }
}


int gsl_sf_exprel_2_impl(double x, double * result)
{
  const double cut = 0.002;

  if(x < GSL_LOG_DBL_MIN) {
    *result = -2.0/x*(1.0 + 1.0/x);
    return GSL_SUCCESS;
  }
  else if(x < -cut) {
    *result = 2.0*(exp(x) - 1.0 - x)/(x*x);
    return GSL_SUCCESS;
  }
  else if(x < cut) {
    *result = (1.0 + 1.0/3.0*x*(1.0 + 0.25*x*(1.0 + 0.2*x*(1.0 + 1.0/6.0*x))));
    return GSL_SUCCESS;
  } 
  else if(x < GSL_LOG_DBL_MAX) {
    *result = 2.0*(exp(x) - 1.0 - x)/(x*x);
    return GSL_SUCCESS;
  }
  else {
    *result = 0.0; /* FIXME: should be Inf */
    return GSL_EOVRFLW;
  }
}


/* Evaluate the continued fraction for exprel.
 * [Abramowitz+Stegun, 4.2.41]
 */
static
int
exprel_n_CF(const int N, const double x, double * result)
{
  const double RECUR_BIG = GSL_SQRT_DBL_MAX;
  const int maxiter = 5000;
  int n = 1;
  double Anm2 = 1.0;
  double Bnm2 = 0.0;
  double Anm1 = 0.0;
  double Bnm1 = 1.0;
  double a1 = 1.0;
  double b1 = 1.0;
  double a2 = -x;
  double b2 = N+1;
  double an, bn;

  double fn;

  double An = b1*Anm1 + a1*Anm2;   /* A1 */
  double Bn = b1*Bnm1 + a1*Bnm2;   /* B1 */
  
  /* One explicit step, before we get to the main pattern. */
  n++;
  Anm2 = Anm1;
  Bnm2 = Bnm1;
  Anm1 = An;
  Bnm1 = Bn;
  An = b2*Anm1 + a2*Anm2;   /* A2 */
  Bn = b2*Bnm1 + a2*Bnm2;   /* B2 */

  fn = An/Bn;

  while(n < maxiter) {
    double old_fn;
    double del;
    n++;
    Anm2 = Anm1;
    Bnm2 = Bnm1;
    Anm1 = An;
    Bnm1 = Bn;
    an = ( GSL_IS_ODD(n) ? ((n-1)/2)*x : -(N+(n/2)-1)*x );
    bn = N + n - 1;
    An = bn*Anm1 + an*Anm2;
    Bn = bn*Bnm1 + an*Bnm2;

    if(fabs(An) > RECUR_BIG || fabs(Bn) > RECUR_BIG) {
      An /= RECUR_BIG;
      Bn /= RECUR_BIG;
      Anm1 /= RECUR_BIG;
      Bnm1 /= RECUR_BIG;
      Anm2 /= RECUR_BIG;
      Bnm2 /= RECUR_BIG;
    }

    old_fn = fn;
    fn = An/Bn;
    del = old_fn/fn;
    
    if(fabs(del - 1.0) < 10.0*GSL_MACH_EPS) break;
  }

  *result = fn;
  if(n == maxiter)
    return GSL_EMAXITER;
  else
    return GSL_SUCCESS;
}


int
gsl_sf_exprel_n_impl(const int N, const double x, double * result)
{
  if(N < 0) {
    *result = 0.0;
    return GSL_EDOM;
  }
  else if(x == 0.0) {
    *result = 1.0;
    return GSL_SUCCESS;
  }
  else if(fabs(x) < GSL_ROOT3_MACH_EPS * N) {
    *result = 1.0 + x/(N+1) * (1.0 + x/(N+2));
    return GSL_SUCCESS;
  }
  else if(N == 0) {
    return gsl_sf_exp_impl(x, result);
  }
  else if(N == 1) {
    return gsl_sf_exprel_impl(x, result);
  }
  else if(N == 2) {
    return gsl_sf_exprel_2_impl(x, result);
  }
  else {
    if(x > 12.0*N) {
      /* Ignore polynomial part; exprel_N(x) ~= e^x N!/x^N
       */
      double lnf_N;
      double lnr;
      gsl_sf_lnfact_impl(N, &lnf_N);
      lnr = x + lnf_N - N*log(x);
      return gsl_sf_exp_impl(lnr, result);
    }
    else if(x > N) {
      /* Write the identity
       *   exprel_n(x) = e^x n! / x^n (1 - Gamma[n,x]/Gamma[n])
       * then use the asymptotic expansion
       * Gamma[n,x] ~ x^(n-1) e^(-x) (1 + (n-1)/x + (n-1)(n-2)/x^2 + ...)
       */
      double lnf_N;
      double lnpre;
      gsl_sf_lnfact_impl(N, &lnf_N);
      lnpre = x + lnf_N - N*log(x);
      if(lnpre < GSL_LOG_DBL_MAX - 5.0) {
        double lg_N;
        double bigGpre = exp(-x + (N-1)*log(x));
	double bigGsum = 1.0;
	double term = 1.0;
	int k;
	for(k=1; k<N; k++) {
	  term *= (N-k)/x;
	  bigGsum += term;
	}
	gsl_sf_lngamma_impl(N, &lg_N);
        *result = exp(lnpre) * (1.0 - bigGpre * exp(-lg_N) * bigGsum);
	return GSL_SUCCESS;
      }
      else {
        *result = 0.0;
	return GSL_EOVRFLW;
      }
    }
    else if(x > -10.0*N) {
      return exprel_n_CF(N, x, result);
    }
    else {
      /* x -> -Infinity asymptotic:
       * exprel_n(x) ~ e^x n!/x^n - n/x (1 + (n-1)/x + (n-1)(n-2)/x + ...)
       *             ~ - n/x (1 + (n-1)/x + (n-1)(n-2)/x + ...)
       */
      double sum  = 1.0;
      double term = 1.0;
      int k;
      for(k=1; k<N; k++) {
        term *= (N-k)/x;
	sum  += term;
      }
      *result = -N/x * sum;
      return GSL_SUCCESS;
    }
  }
}


int gsl_sf_exp_e(const double x, double * result)
{
  int status = gsl_sf_exp_impl(x, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_exp_e", status);
  }
  return status;
}

int gsl_sf_exp_sgn_e(const double x, const double sgn, double * result)
{
  int status = gsl_sf_exp_sgn_impl(x, sgn, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_exp_sgn_e", status);
  }
  return status;
}

int gsl_sf_expm1_e(const double x, double * result)
{
  int status = gsl_sf_expm1_impl(x, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_expm1_e", status);
  }
  return status;
}


int gsl_sf_exprel_e(const double x, double * result)
{
  int status = gsl_sf_exprel_impl(x, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_exprel_e", status);
  }
  return status;
}


int gsl_sf_exprel_2_e(const double x, double * result)
{
  int status = gsl_sf_exprel_2_impl(x, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_exprel_2_e", status);
  }
  return status;
}


int gsl_sf_exprel_n_e(const int n, const double x, double * result)
{
  int status = gsl_sf_exprel_n_impl(n, x, result);
  if(status != GSL_SUCCESS) {
    GSL_ERROR("gsl_sf_exprel_n_e", status);
  }
  return status;
}


double gsl_sf_exp(const double x)
{
  double y;
  int status = gsl_sf_exp_impl(x, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("  gsl_sf_exp", status);
  }
  return y;
}


double gsl_sf_exp_sgn(const double x, const double sgn)
{
  double y;
  int status = gsl_sf_exp_sgn_impl(x, sgn, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("  gsl_sf_exp_sgn", status);
  }
  return y;
}


double gsl_sf_expm1(const double x)
{
  double y;
  int status = gsl_sf_expm1_impl(x, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("  gsl_sf_expm1", status);
  }
  return y;
}


double gsl_sf_exprel(const double x)
{
  double y;
  int status = gsl_sf_exprel_impl(x, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("  gsl_sf_exprel", status);
  }
  return y;
}


double gsl_sf_exprel_2(const double x)
{
  double y;
  int status = gsl_sf_exprel_2_impl(x, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("  gsl_sf_exprel_2", status);
  }
  return y;
}


double gsl_sf_exprel_n(const int n, const double x)
{
  double y;
  int status = gsl_sf_exprel_n_impl(n, x, &y);
  if(status != GSL_SUCCESS) {
    GSL_WARNING("  gsl_sf_exprel_n", status);
  }
  return y;
}
