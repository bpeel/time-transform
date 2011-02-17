#include <stdio.h>
#include <glib.h>

typedef struct _CoglMatrix
{
  float xx, yx, zx, wx;
  float xy, yy, zy, wy;
  float xz, yz, zz, wz;
  float xw, yw, zw, ww;
} CoglMatrix;

typedef struct _Point2f
{
  float x, y;
} Point2f;

typedef struct _Point3f
{
  float x, y, z;
} Point3f;

void
transform_points_f2_sse (const CoglMatrix *matrix,
                         size_t stride_in,
                         void *points_in,
                         size_t stride_out,
                         void *points_out,
                         int n_points);

void
transform_points_f2_gcc (const CoglMatrix *matrix,
                         size_t stride_in,
                         void *points_in,
                         size_t stride_out,
                         void *points_out,
                         int n_points);

void
transform_points_f3_sse (const CoglMatrix *matrix,
                         size_t stride_in,
                         void *points_in,
                         size_t stride_out,
                         void *points_out,
                         int n_points);

void
transform_points_f3_gcc (const CoglMatrix *matrix,
                         size_t stride_in,
                         void *points_in,
                         size_t stride_out,
                         void *points_out,
                         int n_points);
