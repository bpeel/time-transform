#include <stdio.h>
#include <glib.h>

typedef struct _CoglMatrix
{
  /* column 0 */
  float xx;
  float yx;
  float zx;
  float wx;

  /* column 1 */
  float xy;
  float yy;
  float zy;
  float wy;

  /* column 2 */
  float xz;
  float yz;
  float zz;
  float wz;

  /* column 3 */
  float xw;
  float yw;
  float zw;
  float ww;
} CoglMatrix;

typedef struct _Point2f
{
  float x;
  float y;
} Point2f;

typedef struct _Point3f
{
  float x;
  float y;
  float z;
} Point3f;

#define N_TRANSFORMS G_MAXINT

static void
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

static void
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

static void
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

#define TIME_FUNC(func)                                                 \
  g_timer_start (timer);                                                \
                                                                        \
  for (i = 0; i < N_TRANSFORMS; i++)                                    \
    func (&matrix,                                                      \
          /* stride in */                                               \
          sizeof (float) * 2,                                           \
          /* points in */                                               \
          verts,                                                        \
          /* stride out */                                              \
          sizeof (float) * 3,                                           \
          verts_out,                                                    \
          4);                                                           \
                                                                        \
  printf (#func "time = %f\n", g_timer_elapsed (timer, NULL));          \
                                                                        \
  printf ("result = (%f,%f,%f),(%f,%f,%f),(%f,%f,%f),(%f,%f,%f)\n",     \
          verts_out[0], verts_out[1], verts_out[2],                     \
          verts_out[3], verts_out[4], verts_out[5],                     \
          verts_out[6], verts_out[7], verts_out[8],                     \
          verts_out[9], verts_out[10], verts_out[11]);

int
main (int argc, char **argv)
{
  CoglMatrix matrix =
    {
      .xx = 0.00124999997,
      .yx = 0,
      .zx = 0,
      .wx = 0,
      .xy = -0,
      .yy = -0.00166666671,
      .zy = -0,
      .wy = -0,
      .xz = 0,
      .yz = 0,
      .zz = 0.00124999997,
      .wz = 0,
      .xw = 0.16624999,
      .yw = 0.176666677,
      .zw = -0.866025388,
      .ww = 1
    };
  GTimer *timer;
  int i;
  float verts[] = { 0, 0, 0,
                    0, 213, 0,
                    200, 213, 0,
                    200, 0, 0 };
  float verts_out[G_N_ELEMENTS (verts)] =
    { 1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3 };

  timer = g_timer_new ();

  TIME_FUNC (transform_points_f2_sse);
  TIME_FUNC (transform_points_f2_gcc);
  TIME_FUNC (transform_points_f3_sse);
  TIME_FUNC (transform_points_f3_gcc);

  return 0;
}
