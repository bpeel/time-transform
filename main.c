#include "header.h"

#define N_TRANSFORMS G_MAXINT

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
          4);                                                           \
                                                                        \
  printf (#func " time = %f\n", g_timer_elapsed (timer, NULL));         \
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
