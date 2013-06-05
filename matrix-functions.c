#include "header.h"

void
transform_points_f2_sse (const CoglMatrix *matrix,
                         size_t stride_in,
                         void *points_in,
                         size_t stride_out,
                         void *points_out,
                         int n_points)
{
  /* The 'volatile' here is needed because otherwise GCC think that
     none of the outputs are actually used so it will just no-op the
     whole thing */
  asm volatile
    /* Load the matrix into SSE registers xmm4-xmm6 */
    ("movdqu (%3), %%xmm4\n"
     "movdqu 16(%3), %%xmm5\n"
     "movdqu 48(%3), %%xmm6\n"
     /* Jump straight to the loop condition */
     "jmp 0f\n"
     /* loop... */
     "1:\n"
     /* Get next x coordinate into xmm0 */
     "movss (%1), %%xmm0\n"
     /* Get y coordinate into xmm1 */
     "movss 4(%1), %%xmm1\n"
     /* Expand x coordinate to all four floats in xmm0 */
     "shufps $0, %%xmm0, %%xmm0\n"
     /* Expand y coordinate to all four floats in xmm1 */
     "shufps $0, %%xmm1, %%xmm1\n"
     /* Multiply the x coordinate by the first column of the matrix */
     "mulps %%xmm4, %%xmm0\n"
     /* Multiply the y coordinate by the second column of the matrix */
     "mulps %%xmm5, %%xmm1\n"
     /* Add in the fourth column of the matrix */
     "addps %%xmm6, %%xmm0\n"
     /* and the results of multiplying the y coordinate */
     "addps %%xmm1, %%xmm0\n"
     /* write the three resulting parts. we need to do this as three
        separate writes because points_out will only be aligned to
        32-bits and we don't want to overwrite the space after the
        third float */
     /* store first float */
     "movss %%xmm0, (%2)\n"
     /* copy second float */
     "shufps $0xe5, %%xmm0, %%xmm0\n"
     "movss %%xmm0, 4(%2)\n"
     /* copy third float */
     "shufps $0xe6, %%xmm0, %%xmm0\n"
     "movss %%xmm0, 8(%2)\n"
     "add %4, %1\n" /* add stride_in to points_in */
     "add %5, %2\n" /* add stride_out to points_out */
     /* loop condition */
     "0:\n"
     "dec %0\n" /* decrement n_points */
     "jns 1b\n" /* continue if not negative */
     : /* these aren't really outputs but GCC doesn't have any other
          way to specify that we're clobbering them apart from to
          mark them as in-out */
       "+r" (n_points), /* 0 */
       "+r" (points_in), /* 1 */
       "+r" (points_out) /* 2 */
     : "r" (matrix), /* 3 */
       "rin" (stride_in), /* 4 */
       "rin" (stride_out) /* 5 */
     : "xmm0", "xmm1", "xmm4", "xmm5", "xmm6");
}

void
transform_points_f2_gcc (const CoglMatrix *matrix,
                         size_t stride_in,
                         void *points_in,
                         size_t stride_out,
                         void *points_out,
                         int n_points)
{
  int i;

  for (i = 0; i < n_points; i++)
    {
      Point2f p = *(Point2f *)((guint8 *)points_in + i * stride_in);
      Point3f *o = (Point3f *)((guint8 *)points_out + i * stride_out);

      o->x = matrix->xx * p.x + matrix->xy * p.y + matrix->xw;
      o->y = matrix->yx * p.x + matrix->yy * p.y + matrix->yw;
      o->z = matrix->zx * p.x + matrix->zy * p.y + matrix->zw;
    }
}

void
transform_points_f3_sse (const CoglMatrix *matrix,
                         size_t stride_in,
                         void *points_in,
                         size_t stride_out,
                         void *points_out,
                         int n_points)
{
  /* The 'volatile' here is needed because otherwise GCC think that
     none of the outputs are actually used so it will just no-op the
     whole thing */
  asm volatile
    /* Load the matrix into SSE registers xmm4-xmm7 */
    ("movdqu (%3), %%xmm4\n"
     "movdqu 16(%3), %%xmm5\n"
     "movdqu 32(%3), %%xmm6\n"
     "movdqu 48(%3), %%xmm7\n"
     /* Jump straight to the loop condition */
     "jmp 0f\n"
     /* loop... */
     "1:\n"
     /* Get next x coordinate into xmm0 */
     "movss (%1), %%xmm0\n"
     /* Get y coordinate into xmm1 */
     "movss 4(%1), %%xmm1\n"
     /* Get z coordinate into xmm2 */
     "movss 8(%1), %%xmm2\n"
     /* Expand x coordinate to all four floats in xmm0 */
     "shufps $0, %%xmm0, %%xmm0\n"
     /* Expand y coordinate to all four floats in xmm1 */
     "shufps $0, %%xmm1, %%xmm1\n"
     /* Expand z coordinate to all four floats in xmm2 */
     "shufps $0, %%xmm2, %%xmm2\n"
     /* Multiply the x coordinate by the first column of the matrix */
     "mulps %%xmm4, %%xmm0\n"
     /* Multiply the y coordinate by the second column of the matrix */
     "mulps %%xmm5, %%xmm1\n"
     /* Multiply the z coordinate by the third column of the matrix */
     "mulps %%xmm6, %%xmm2\n"
     /* Add in the fourth column of the matrix */
     "addps %%xmm7, %%xmm0\n"
     /* and the results of multiplying the y coordinate */
     "addps %%xmm1, %%xmm0\n"
     /* and the results of multiplying the z coordinate */
     "addps %%xmm2, %%xmm0\n"
     /* write the three resulting parts. we need to do this as three
        separate writes because points_out will only be aligned to
        32-bits and we don't want to overwrite the space after the
        third float */
     /* store first float */
     "movss %%xmm0, (%2)\n"
     /* copy second float */
     "shufps $0xe5, %%xmm0, %%xmm0\n"
     "movss %%xmm0, 4(%2)\n"
     /* copy third float */
     "shufps $0xe6, %%xmm0, %%xmm0\n"
     "movss %%xmm0, 8(%2)\n"
     "add %4, %1\n" /* add stride_in to points_in */
     "add %5, %2\n" /* add stride_out to points_out */
     /* loop condition */
     "0:\n"
     "dec %0\n" /* decrement n_points */
     "jns 1b\n" /* continue if not negative */
     : /* these aren't really outputs but GCC doesn't have any other
          way to specify that we're clobbering them apart from to
          mark them as in-out */
       "+r" (n_points), /* 0 */
       "+r" (points_in), /* 1 */
       "+r" (points_out) /* 2 */
     : "r" (matrix), /* 3 */
       "rin" (stride_in), /* 4 */
       "rin" (stride_out) /* 5 */
     : "xmm0", "xmm1", "xmm2", "xmm4", "xmm5", "xmm6", "xmm7");
}

void
transform_points_f3_gcc (const CoglMatrix *matrix,
                         size_t stride_in,
                         void *points_in,
                         size_t stride_out,
                         void *points_out,
                         int n_points)
{
  int i;

  for (i = 0; i < n_points; i++)
    {
      Point3f p = *(Point3f *)((guint8 *)points_in + i * stride_in);
      Point3f *o = (Point3f *)((guint8 *)points_out + i * stride_out);

      o->x = matrix->xx * p.x + matrix->xy * p.y +
             matrix->xz * p.z + matrix->xw;
      o->y = matrix->yx * p.x + matrix->yy * p.y +
             matrix->yz * p.z + matrix->yw;
      o->z = matrix->zx * p.x + matrix->zy * p.y +
             matrix->zz * p.z + matrix->zw;
    }
}

void
project_points_f3_sse (const CoglMatrix *matrix,
                       size_t stride_in,
                       void *points_in,
                       size_t stride_out,
                       void *points_out,
                       int n_points)
{
  /* The 'volatile' here is needed because otherwise GCC think that
     none of the outputs are actually used so it will just no-op the
     whole thing */
  asm volatile
    /* Load the matrix into SSE registers xmm4-xmm7 */
    ("movdqu (%3), %%xmm4\n"
     "movdqu 16(%3), %%xmm5\n"
     "movdqu 32(%3), %%xmm6\n"
     "movdqu 48(%3), %%xmm7\n"
     /* Jump straight to the loop condition */
     "jmp 0f\n"
     /* loop... */
     "1:\n"
     /* Get next x coordinate into xmm0 */
     "movss (%1), %%xmm0\n"
     /* Get y coordinate into xmm1 */
     "movss 4(%1), %%xmm1\n"
     /* Get z coordinate into xmm2 */
     "movss 8(%1), %%xmm2\n"
     /* Expand x coordinate to all four floats in xmm0 */
     "shufps $0, %%xmm0, %%xmm0\n"
     /* Expand y coordinate to all four floats in xmm1 */
     "shufps $0, %%xmm1, %%xmm1\n"
     /* Expand z coordinate to all four floats in xmm2 */
     "shufps $0, %%xmm2, %%xmm2\n"
     /* Multiply the x coordinate by the first column of the matrix */
     "mulps %%xmm4, %%xmm0\n"
     /* Multiply the y coordinate by the second column of the matrix */
     "mulps %%xmm5, %%xmm1\n"
     /* Multiply the z coordinate by the third column of the matrix */
     "mulps %%xmm6, %%xmm2\n"
     /* Add in the fourth column of the matrix */
     "addps %%xmm7, %%xmm0\n"
     /* and the results of multiplying the y coordinate */
     "addps %%xmm1, %%xmm0\n"
     /* and the results of multiplying the z coordinate */
     "addps %%xmm2, %%xmm0\n"
     /* write the four resulting parts. we can do this as one
      * unaligned write */
     "movdqu %%xmm0, (%2)\n"
     "add %4, %1\n" /* add stride_in to points_in */
     "add %5, %2\n" /* add stride_out to points_out */
     /* loop condition */
     "0:\n"
     "dec %0\n" /* decrement n_points */
     "jns 1b\n" /* continue if not negative */
     : /* these aren't really outputs but GCC doesn't have any other
          way to specify that we're clobbering them apart from to
          mark them as in-out */
       "+r" (n_points), /* 0 */
       "+r" (points_in), /* 1 */
       "+r" (points_out) /* 2 */
     : "r" (matrix), /* 3 */
       "rin" (stride_in), /* 4 */
       "rin" (stride_out) /* 5 */
     : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7");
}

void
project_points_f3_gcc (const CoglMatrix *matrix,
                       size_t stride_in,
                       const void *points_in,
                       size_t stride_out,
                       void *points_out,
                       int n_points)
{
  int i;

  for (i = 0; i < n_points; i++)
    {
      Point3f p = *(Point3f *)((guint8 *)points_in + i * stride_in);
      Point4f *o = (Point4f *)((guint8 *)points_out + i * stride_out);

      o->x = matrix->xx * p.x + matrix->xy * p.y +
             matrix->xz * p.z + matrix->xw;
      o->y = matrix->yx * p.x + matrix->yy * p.y +
             matrix->yz * p.z + matrix->yw;
      o->z = matrix->zx * p.x + matrix->zy * p.y +
             matrix->zz * p.z + matrix->zw;
      o->w = matrix->wx * p.x + matrix->wy * p.y +
             matrix->wz * p.z + matrix->ww;
    }
}

void
project_points_f4_sse (const CoglMatrix *matrix,
                       size_t stride_in,
                       void *points_in,
                       size_t stride_out,
                       void *points_out,
                       int n_points)
{
  /* The 'volatile' here is needed because otherwise GCC think that
     none of the outputs are actually used so it will just no-op the
     whole thing */
  asm volatile
    /* Load the matrix into SSE registers xmm4-xmm7 */
    ("movdqu (%3), %%xmm4\n"
     "movdqu 16(%3), %%xmm5\n"
     "movdqu 32(%3), %%xmm6\n"
     "movdqu 48(%3), %%xmm7\n"
     /* Jump straight to the loop condition */
     "jmp 0f\n"
     /* loop... */
     "1:\n"
     /* Get next x coordinate into xmm0 */
     "movss (%1), %%xmm0\n"
     /* Get y coordinate into xmm1 */
     "movss 4(%1), %%xmm1\n"
     /* Get z coordinate into xmm2 */
     "movss 8(%1), %%xmm2\n"
     /* Get w coordinate into xmm3 */
     "movss 12(%1), %%xmm3\n"
     /* Expand x coordinate to all four floats in xmm0 */
     "shufps $0, %%xmm0, %%xmm0\n"
     /* Expand y coordinate to all four floats in xmm1 */
     "shufps $0, %%xmm1, %%xmm1\n"
     /* Expand z coordinate to all four floats in xmm2 */
     "shufps $0, %%xmm2, %%xmm2\n"
     /* Expand w coordinate to all four floats in xmm3 */
     "shufps $0, %%xmm3, %%xmm3\n"
     /* Multiply the x coordinate by the first column of the matrix */
     "mulps %%xmm4, %%xmm0\n"
     /* Multiply the y coordinate by the second column of the matrix */
     "mulps %%xmm5, %%xmm1\n"
     /* Multiply the z coordinate by the third column of the matrix */
     "mulps %%xmm6, %%xmm2\n"
     /* Multiply the w coordinate by the fourth column of the matrix */
     "mulps %%xmm7, %%xmm3\n"
     /* Add in the results of multiplying the y coordinate */
     "addps %%xmm1, %%xmm0\n"
     /* and the results of multiplying the z coordinate */
     "addps %%xmm2, %%xmm0\n"
     /* and the results of multiplying the w coordinate */
     "addps %%xmm3, %%xmm0\n"
     /* write the four resulting parts. we can do this as one
      * unaligned write */
     "movdqu %%xmm0, (%2)\n"
     "add %4, %1\n" /* add stride_in to points_in */
     "add %5, %2\n" /* add stride_out to points_out */
     /* loop condition */
     "0:\n"
     "dec %0\n" /* decrement n_points */
     "jns 1b\n" /* continue if not negative */
     : /* these aren't really outputs but GCC doesn't have any other
          way to specify that we're clobbering them apart from to
          mark them as in-out */
       "+r" (n_points), /* 0 */
       "+r" (points_in), /* 1 */
       "+r" (points_out) /* 2 */
     : "r" (matrix), /* 3 */
       "rin" (stride_in), /* 4 */
       "rin" (stride_out) /* 5 */
     : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7");
}

void
project_points_f4_gcc (const CoglMatrix *matrix,
                       size_t stride_in,
                       const void *points_in,
                       size_t stride_out,
                       void *points_out,
                       int n_points)
{
  int i;

  for (i = 0; i < n_points; i++)
    {
      Point4f p = *(Point4f *)((guint8 *)points_in + i * stride_in);
      Point4f *o = (Point4f *)((guint8 *)points_out + i * stride_out);

      o->x = matrix->xx * p.x + matrix->xy * p.y +
             matrix->xz * p.z + matrix->xw * p.w;
      o->y = matrix->yx * p.x + matrix->yy * p.y +
             matrix->yz * p.z + matrix->yw * p.w;
      o->z = matrix->zx * p.x + matrix->zy * p.y +
             matrix->zz * p.z + matrix->zw * p.w;
      o->w = matrix->wx * p.x + matrix->wy * p.y +
             matrix->wz * p.z + matrix->ww * p.w;
    }
}
