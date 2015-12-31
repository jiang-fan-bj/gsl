/* multifit_nlinear/lmdiag.c
 * 
 * Copyright (C) 2015 Patrick Alken
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/*
 * This module handles the updating of the scaling matrix D
 * in the solution step:
 *
 * [    J     ] dx = - [ f ]
 * [ lambda*D ]        [ 0 ]
 *
 * according to several different strategies.
 */

static int init_diag_levenberg(const gsl_matrix * J, gsl_vector * diag);
static int update_diag_levenberg(const gsl_matrix * J,
                                 gsl_vector * diag);
static int init_diag_marquardt(const gsl_matrix * J, gsl_vector * diag);
static int update_diag_marquardt (const gsl_matrix * J,
                                  gsl_vector * diag);
static int init_diag_more(const gsl_matrix * J, gsl_vector * diag);
static int update_diag_more(const gsl_matrix * J, gsl_vector * diag);

/* Levenberg scaling, D = I */
static int
init_diag_levenberg(const gsl_matrix * J, gsl_vector * diag)
{
  (void)J; /* avoid unused parameter warning */
  gsl_vector_set_all(diag, 1.0);
  return GSL_SUCCESS;
}

static int
update_diag_levenberg(const gsl_matrix * J, gsl_vector * diag)
{
  (void)J;    /* avoid unused parameter warning */
  (void)diag; /* avoid unused parameter warning */

  /* nothing to do */
  return GSL_SUCCESS;
}

/* initialize diagonal scaling matrix D according to Marquardt method */
static int
init_diag_marquardt(const gsl_matrix * J, gsl_vector * diag)
{
  return update_diag_marquardt(J, diag);
}

/* update diagonal scaling matrix D according to Marquardt method */
static int
update_diag_marquardt (const gsl_matrix * J, gsl_vector * diag)
{
  const size_t p = J->size2;
  size_t j;

  for (j = 0; j < p; j++)
    {
      gsl_vector_const_view v = gsl_matrix_const_column(J, j);
      double norm = gsl_blas_dnrm2(&v.vector);

      if (norm == 0.0)
        norm = 1.0;

      gsl_vector_set(diag, j, norm);
    }

  return GSL_SUCCESS;
}

/* initialize diagonal scaling matrix D according to Eq 6.3 of
 * More, 1978 */
static int
init_diag_more(const gsl_matrix * J, gsl_vector * diag)
{
  int status;

  gsl_vector_set_zero(diag);
  status = update_diag_more(J, diag);

  return status;
}

/* update diagonal scaling matrix D according to Eq. 6.3 of
 * More, 1978 */
static int
update_diag_more (const gsl_matrix * J, gsl_vector * diag)
{
  const size_t p = J->size2;
  size_t j;

  for (j = 0; j < p; j++)
    {
      gsl_vector_const_view v = gsl_matrix_const_column(J, j);
      double norm = gsl_blas_dnrm2(&v.vector);
      double *diagj = gsl_vector_ptr(diag, j);

      if (norm == 0.0)
        norm = 1.0;

      *diagj = GSL_MAX(*diagj, norm);
    }

  return GSL_SUCCESS;
}
