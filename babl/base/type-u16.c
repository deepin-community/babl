/* babl - dynamically extendable universal pixel conversion library.
 * Copyright (C) 2005, Øyvind Kolås.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, see
 * <https://www.gnu.org/licenses/>.
 */

#include "config.h"
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#include "babl-internal.h"
#include "babl-base.h"


static inline void
convert_double_u16_scaled (BablConversion *conversion,
                           double          min_val,
                           double          max_val,
                           uint16_t        min,
                           uint16_t        max,
                           char           *src,
                           char           *dst,
                           int             src_pitch,
                           int             dst_pitch,
                           long            n)
{
  while (n--)
    {
      double   dval = *(double *) src;
      uint16_t u16val;

      if (dval < min_val)
        u16val = min;
      else if (dval > max_val)
        u16val = max;
      else
        u16val = (dval - min_val) / (max_val - min_val) * (max - min) + min + 0.5;

      *(uint16_t *) dst = u16val;
      dst              += dst_pitch;
      src              += src_pitch;
    }
}

static inline void
convert_u16_double_scaled (BablConversion *conversion,
                           double          min_val,
                           double          max_val,
                           uint16_t        min,
                           uint16_t        max,
                           char           *src,
                           char           *dst,
                           int             src_pitch,
                           int             dst_pitch,
                           long            n)
{
  while (n--)
    {
      int    u16val = *(uint16_t *) src;
      double dval;

      if (u16val < min)
        dval = min_val;
      else if (u16val > max)
        dval = max_val;
      else
        dval = (u16val - min) / (double) (max - min) * (max_val - min_val) + min_val;

      (*(double *) dst) = dval;
      dst              += dst_pitch;
      src              += src_pitch;
    }
}

#define MAKE_CONVERSIONS(name, min_val, max_val, min, max)      \
  static void \
  convert_ ## name ## _double (BablConversion *c, void *src, \
                               void *dst, \
                               int src_pitch, \
                               int dst_pitch, \
                               long n)                               \
  { \
    convert_u16_double_scaled (c, min_val, max_val, min, max, \
                               src, dst, src_pitch, dst_pitch, n); \
  }                                                               \
  static void \
  convert_double_ ## name (BablConversion *c, void *src, \
                           void *dst, \
                           int src_pitch, \
                           int dst_pitch, \
                           long n)                                 \
  { \
    convert_double_u16_scaled (c, min_val, max_val, min, max, \
                               src, dst, src_pitch, dst_pitch, n); \
  }

MAKE_CONVERSIONS (u16, 0.0, 1.0, 0, UINT16_MAX)

static inline void
convert_float_u16_scaled (BablConversion *conversion,
                          double          min_val,
                          double          max_val,
                          uint16_t        min,
                          uint16_t        max,
                          char           *src,
                          char           *dst,
                          int             src_pitch,
                          int             dst_pitch,
                          long            n)
{
  while (n--)
    {
      float   dval = *(float *) src;
      uint16_t u16val;

      if (dval < min_val)
        u16val = min;
      else if (dval > max_val)
        u16val = max;
      else
        u16val = (dval - min_val) / (max_val - min_val) * (max - min) + min + 0.5f;

      *(uint16_t *) dst = u16val;
      dst              += dst_pitch;
      src              += src_pitch;
    }
}

static inline void
convert_u16_float_scaled (BablConversion *conversion,
                          double          min_val,
                          double          max_val,
                          uint16_t        min,
                          uint16_t        max,
                          char           *src,
                          char           *dst,
                          int             src_pitch,
                          int             dst_pitch,
                          long            n)
{
  while (n--)
    {
      int    u16val = *(uint16_t *) src;
      float dval;

      if (u16val < min)
        dval = min_val;
      else if (u16val > max)
        dval = max_val;
      else
        dval = (u16val - min) / (float) (max - min) * (max_val - min_val) + min_val;

      (*(float *) dst) = dval;
      dst             += dst_pitch;
      src             += src_pitch;
    }
}

#define MAKE_CONVERSIONS_float(name, min_val, max_val, min, max)      \
  static void \
  convert_ ## name ## _float (BablConversion *c, void *src, \
                               void *dst, \
                               int src_pitch, \
                               int dst_pitch, \
                               long n)                               \
  { \
    convert_u16_float_scaled (c, min_val, max_val, min, max, \
                               src, dst, src_pitch, dst_pitch, n); \
  }                                                               \
  static void \
  convert_float_ ## name (BablConversion *c, void *src, \
                           void *dst, \
                           int src_pitch, \
                           int dst_pitch, \
                           long n)                                 \
  { \
    convert_float_u16_scaled (c, min_val, max_val, min, max, \
                               src, dst, src_pitch, dst_pitch, n); \
  }

MAKE_CONVERSIONS_float (u16, 0.0, 1.0, 0, UINT16_MAX)


void
BABL_SIMD_SUFFIX (babl_base_type_u16) (void)
{
  babl_type_new (
    "u16",
    "id", BABL_U16,
    "bits", 16,
    NULL);

  babl_conversion_new (
    babl_type_from_id (BABL_U16),
    babl_type_from_id (BABL_DOUBLE),
    "plane", convert_u16_double,
    NULL
  );

  babl_conversion_new (
    babl_type_from_id (BABL_DOUBLE),
    babl_type_from_id (BABL_U16),
    "plane", convert_double_u16,
    NULL
  );

  babl_conversion_new (
    babl_type_from_id (BABL_U16),
    babl_type_from_id (BABL_FLOAT),
    "plane", convert_u16_float,
    NULL
  );

  babl_conversion_new (
    babl_type_from_id (BABL_FLOAT),
    babl_type_from_id (BABL_U16),
    "plane", convert_float_u16,
    NULL
  );
}
