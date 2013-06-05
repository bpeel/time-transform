#include "header.h"

#include <stdlib.h>

#define N_TRANSFORMS (1 << 23)

#define TIME_FUNC(func)                                                 \
  g_timer_start (timer);                                                \
                                                                        \
  for (i = 0; i < N_TRANSFORMS; i++)                                    \
    func (&matrix,                                                      \
          /* stride in */                                               \
          sizeof (float) * 3,                                           \
          /* points in */                                               \
          verts,                                                        \
          /* stride out */                                              \
          sizeof (float) * 3,                                           \
          verts_out,                                                    \
          n_verts);                                                     \
                                                                        \
  printf (#func " time = %f\n", g_timer_elapsed (timer, NULL));

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
  int i, n_verts = 4;
  float *verts, *verts_out;

  if (argc > 1)
    n_verts = atoi (argv[1]);

  verts = g_malloc (sizeof (float) * n_verts * 3);
  verts_out = g_malloc (sizeof (float) * n_verts * 3);

  printf ("n_verts = %i (give a command line argument to change)\n", n_verts);

  timer = g_timer_new ();

  TIME_FUNC (transform_points_f3_sse);
  TIME_FUNC (transform_points_f3_gcc);
  TIME_FUNC (transform_points_f2_sse);
  TIME_FUNC (transform_points_f2_gcc);

  return 0;
}
