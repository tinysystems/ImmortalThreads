/*
 * dnn_calc.c
 *
 *  Created on: 25 Haz 2020
 *      Author: eren
 */
#include "immortality.h"

#include "includes/dnnlib/dnn.h"
_immortal_function(convol, mat_t *src, mat_t *dest, mat_t *w, mat_t *b);
_immortal_function(convol_depth, mat_t *src, mat_t *dest, mat_t *w, mat_t *b);
_immortal_function(fc_sparse, mat_t *src, mat_t *dest, mat_t *weight,
                   mat_t *bias);
_immortal_function(fc_dense, mat_t *src, mat_t *dest, mat_t *weight,
                   mat_t *bias);
_immortal_function(pooling, mat_t *src, mat_t *dest);
_immortal_function(relu, mat_t *src, mat_t *dest);
_immortal_function_with_retval(prediction, uint16_t, mat_t *src);
#include "includes/dnnlib/dnn_calc.h"
_immortal_function(con_mul, mat_t *src, mat_t *dest, mat_t *filter);
_immortal_function(fc_sparse_mul, mat_t *src, mat_t *dest, mat_t *filter);
_immortal_function(fc_dense_mul, mat_t *src, mat_t *dest, mat_t *filter);
_immortal_function(ds_zero, mat_t *src, mat_t *dest);
_immortal_function(ds_add, mat_t *src, mat_t *dest, mat_t *filter);
_immortal_function(dm_add, mat_t *src, mat_t *dest, mat_t *bias);
#include <stdbool.h>
#include <stdlib.h>

static __attribute__((section("persistent"))) /*__fram*/ mat_t buf = {
    .data = mat_buffers[0]};
static __attribute__((section("persistent"))) /*__fram*/ mat_t *buffer = &buf;

_immortal_function(con_mul, mat_t *src, mat_t *dest, mat_t *filter) {
  _begin(con_mul);
  _def mat_t *inter;
  _WR(inter, buffer);
  _def uint16_t rows /*MAT_GET_DIM*/ /*(dest, 0)*/;
  _WR(rows, (mat_get_dim(dest, 0))); // input rows
  _def uint16_t cols /*MAT_GET_DIM*/ /*(dest, 1)*/;
  _WR(cols, (mat_get_dim(dest, 1))); // input columns
  (mat_reshape(inter, ((uint16_t[]){rows, cols}),
               (sizeof((uint16_t[]){rows, cols}) /
                sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(inter, rows, cols)*/;

  _def uint16_t frows;
  _WR(frows, filter->sparse.dims[1]); // filter rows
  _def uint16_t fcols;
  _WR(fcols, filter->sparse.dims[2]); // filter columns
  _def uint16_t total_elements /*MAT_GET_DIM*/ /*(filter, 0)*/;
  _WR(total_elements, (mat_get_dim(filter, 0)));
  _def uint16_t swap;
  _WR(swap, 1);

  _def uint16_t pos;
  _WR(pos, 0);
  _def uint16_t idx;
  _WR(idx, 0);

  for (pos; pos < total_elements;) {
    _WR_SELF(uint16_t, swap, swap ^ 0x01);
    _def mat_t *tmp;
    _WR(tmp, dest);
    if (pos != 0) { // Swap buffers
      _WR(dest, inter);
      _WR(inter, tmp);
    }
    _def _Bool /*bool*/ zero /*false*/;
    _WR(zero, 0);
    if (pos == 0) {
      _WR(zero, 1) /*true*/;
      _WR_SELF(uint16_t, idx, idx + filter->sparse.offsets[pos]);
    }
    _def uint16_t k;
    _WR(k, idx / (fcols * frows)); // Layers
    _def uint16_t l;
    _WR(l, (idx % (fcols * frows)) / fcols); // Rows
    _def uint16_t n;
    _WR(n, idx % fcols); // Cols

    _def fixed f /*MAT_GET*/ /*(filter, pos)*/;
    _WR(f, (((sizeof((uint16_t[]){pos}) / sizeof(uint16_t)) == 1)
                ? *(filter->data + ((uint16_t[]){pos})[0])
                : ((sizeof((uint16_t[]){pos}) / sizeof(uint16_t)) == 2)
                      ? *(filter->data +
                          ((uint16_t[]){pos})[0] * filter->strides[0] +
                          ((uint16_t[]){pos})[1])
                      : ((sizeof((uint16_t[]){pos}) / sizeof(uint16_t)) == 3)
                            ? *(filter->data +
                                ((uint16_t[]){pos})[0] * filter->strides[0] +
                                filter->strides[1] * ((uint16_t[]){pos})[1] +
                                ((uint16_t[]){pos})[2])
                            : mat_get(filter, ((uint16_t[]){pos}),
                                      (sizeof((uint16_t[]){pos}) /
                                       sizeof(uint16_t)))));

    _def fixed *dest_ptr /*MAT_PTR*/ /*(dest, 0, 0)*/;
    _WR(dest_ptr,
        (((sizeof((uint16_t[]){0, 0}) / sizeof(uint16_t)) == 1)
             ? (dest->data + ((uint16_t[]){0, 0})[0])
             : ((sizeof((uint16_t[]){0, 0}) / sizeof(uint16_t)) == 2)
                   ? (dest->data + ((uint16_t[]){0, 0})[0] * dest->strides[0] +
                      ((uint16_t[]){0, 0})[1])
                   : ((sizeof((uint16_t[]){0, 0}) / sizeof(uint16_t)) == 3)
                         ? (dest->data +
                            ((uint16_t[]){0, 0})[0] * dest->strides[0] +
                            dest->strides[1] * ((uint16_t[]){0, 0})[1] +
                            ((uint16_t[]){0, 0})[2])
                         : mat_ptr(dest, ((uint16_t[]){0, 0}),
                                   (sizeof((uint16_t[]){0, 0}) /
                                    sizeof(uint16_t)))));
    _def fixed *inter_ptr /*MAT_PTR*/ /*(inter, 0, 0)*/;
    _WR(inter_ptr,
        (((sizeof((uint16_t[]){0, 0}) / sizeof(uint16_t)) == 1)
             ? (inter->data + ((uint16_t[]){0, 0})[0])
             : ((sizeof((uint16_t[]){0, 0}) / sizeof(uint16_t)) == 2)
                   ? (inter->data +
                      ((uint16_t[]){0, 0})[0] * inter->strides[0] +
                      ((uint16_t[]){0, 0})[1])
                   : ((sizeof((uint16_t[]){0, 0}) / sizeof(uint16_t)) == 3)
                         ? (inter->data +
                            ((uint16_t[]){0, 0})[0] * inter->strides[0] +
                            inter->strides[1] * ((uint16_t[]){0, 0})[1] +
                            ((uint16_t[]){0, 0})[2])
                         : mat_ptr(inter, ((uint16_t[]){0, 0}),
                                   (sizeof((uint16_t[]){0, 0}) /
                                    sizeof(uint16_t)))));
    {
      _def uint16_t i;
      _WR(i, 0);
      for (; i < rows * params.stride[1];) {
        _def fixed *src_ptr /*MAT_PTR*/
            /*(src, k, i + l, n)*/;
        _WR(src_ptr,
            (((sizeof((uint16_t[]){k, i + l, n}) / sizeof(uint16_t)) == 1)
                 ? (src->data + ((uint16_t[]){k, i + l, n})[0])
                 : ((sizeof((uint16_t[]){k, i + l, n}) / sizeof(uint16_t)) == 2)
                       ? (src->data +
                          ((uint16_t[]){k, i + l, n})[0] * src->strides[0] +
                          ((uint16_t[]){k, i + l, n})[1])
                       : ((sizeof((uint16_t[]){k, i + l, n}) /
                           sizeof(uint16_t)) == 3)
                             ? (src->data +
                                ((uint16_t[]){k, i + l, n})[0] *
                                    src->strides[0] +
                                src->strides[1] *
                                    ((uint16_t[]){k, i + l, n})[1] +
                                ((uint16_t[]){k, i + l, n})[2])
                             : mat_ptr(src, ((uint16_t[]){k, i + l, n}),
                                       (sizeof((uint16_t[]){k, i + l, n}) /
                                        sizeof(uint16_t)))));
        {
          _def uint16_t j;
          _WR(j, 0);
          for (; j < cols * params.stride[2];) {
            _def fixed w;
            _WR(w, 0);
            if (!params.same_padding ||
                (i + l < (mat_get_dim(src, 1)) /*MAT_GET_DIM*/ /*(src, 1)*/ &&
                 j + n < (mat_get_dim(src, 2)) /*MAT_GET_DIM*/ /*(src, 2)*/)) {
              _WR(w, f_mul(f, *src_ptr)) /*F_MUL*/ /*(f, *src_ptr)*/;
            }
            if (!zero) {
              _WR_SELF(fixed, w, w + *inter_ptr) /*F_ADD*/ /*(w, *inter_ptr)*/;
              _WR_SELF(fixed *, inter_ptr, inter_ptr + 1);
            }

            _WR(*dest_ptr, w);
            _WR_SELF(fixed *, dest_ptr, dest_ptr + 1);
            _WR_SELF(fixed *, src_ptr, src_ptr + params.stride[2]);
            _WR_SELF(uint16_t, j, j + params.stride[2]);
          }
        }
        _WR_SELF(uint16_t, i, i + params.stride[1]);
      }
    }
    _WR_SELF(uint16_t, idx, idx + filter->sparse.offsets[pos + 1]);
    _WR_SELF(uint16_t, pos, pos + 1);
  }

  if (swap) {
    {
      _def uint16_t i;
      _WR(i, 0);
      for (; i < rows;) {
        {
          _def uint16_t j;
          _WR(j, 0);
          for (; j < cols;) {
            if (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 1)) {
              _WR(*(inter->data + ((uint16_t[]){i, j})[0]),
                  (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 1)
                       ? *(dest->data + ((uint16_t[]){i, j})[0])
                       : ((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 2)
                             ? *(dest->data +
                                 ((uint16_t[]){i, j})[0] * dest->strides[0] +
                                 ((uint16_t[]){i, j})[1])
                             : ((sizeof((uint16_t[]){i, j}) /
                                 sizeof(uint16_t)) == 3)
                                   ? *(dest->data +
                                       ((uint16_t[]){i, j})[0] *
                                           dest->strides[0] +
                                       dest->strides[1] *
                                           ((uint16_t[]){i, j})[1] +
                                       ((uint16_t[]){i, j})[2])
                                   : mat_get(dest, ((uint16_t[]){i, j}),
                                             (sizeof((uint16_t[]){i, j}) /
                                              sizeof(uint16_t)))));
            } else {
              if (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 3)) {
                _WR(*(inter->data +
                      ((uint16_t[]){i, j})[0] * inter->strides[0] +
                      inter->strides[1] * ((uint16_t[]){i, j})[1] +
                      ((uint16_t[]){i, j})[2]),
                    (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 1)
                         ? *(dest->data + ((uint16_t[]){i, j})[0])
                         : ((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) ==
                            2)
                               ? *(dest->data +
                                   ((uint16_t[]){i, j})[0] * dest->strides[0] +
                                   ((uint16_t[]){i, j})[1])
                               : ((sizeof((uint16_t[]){i, j}) /
                                   sizeof(uint16_t)) == 3)
                                     ? *(dest->data +
                                         ((uint16_t[]){i, j})[0] *
                                             dest->strides[0] +
                                         dest->strides[1] *
                                             ((uint16_t[]){i, j})[1] +
                                         ((uint16_t[]){i, j})[2])
                                     : mat_get(dest, ((uint16_t[]){i, j}),
                                               (sizeof((uint16_t[]){i, j}) /
                                                sizeof(uint16_t)))));
              } else {
                mat_set(
                    inter,
                    (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 1)
                         ? *(dest->data + ((uint16_t[]){i, j})[0])
                         : ((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) ==
                            2)
                               ? *(dest->data +
                                   ((uint16_t[]){i, j})[0] * dest->strides[0] +
                                   ((uint16_t[]){i, j})[1])
                               : ((sizeof((uint16_t[]){i, j}) /
                                   sizeof(uint16_t)) == 3)
                                     ? *(dest->data +
                                         ((uint16_t[]){i, j})[0] *
                                             dest->strides[0] +
                                         dest->strides[1] *
                                             ((uint16_t[]){i, j})[1] +
                                         ((uint16_t[]){i, j})[2])
                                     : mat_get(dest, ((uint16_t[]){i, j}),
                                               (sizeof((uint16_t[]){i, j}) /
                                                sizeof(uint16_t)))),
                    ((uint16_t[]){i, j}),
                    (sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)));
              }
            } /*MAT_SET*/
            /*(inter, MAT_GET(dest, i, j), i, j)*/;
            _WR_SELF(uint16_t, j, j + 1);
          }
        }
        _WR_SELF(uint16_t, i, i + 1);
      }
    }
  }
  _end(con_mul);
}

_immortal_function(fc_sparse_mul, mat_t *src, mat_t *dest, mat_t *filter) {
  _begin(fc_sparse_mul);

  _def uint16_t rows /*MAT_GET_DIM*/ /*(dest, 0)*/;
  _WR(rows, (mat_get_dim(dest, 0)));

  {
    _def uint16_t i;
    _WR(i, 0);
    for (; i < rows;) {
      _def uint16_t start;
      _WR(start, filter->sparse.sizes[i]);
      _def uint16_t end;
      _WR(end, filter->sparse.sizes[i + 1]);
      _def uint16_t col_idx;
      _WR(col_idx, start);
      _def fixed *filter_ptr /*MAT_PTR*/
          /*(filter, col_idx)*/;
      _WR(filter_ptr,
          (((sizeof((uint16_t[]){col_idx}) / sizeof(uint16_t)) == 1)
               ? (filter->data + ((uint16_t[]){col_idx})[0])
               : ((sizeof((uint16_t[]){col_idx}) / sizeof(uint16_t)) == 2)
                     ? (filter->data +
                        ((uint16_t[]){col_idx})[0] * filter->strides[0] +
                        ((uint16_t[]){col_idx})[1])
                     : ((sizeof((uint16_t[]){col_idx}) / sizeof(uint16_t)) == 3)
                           ? (filter->data +
                              ((uint16_t[]){col_idx})[0] * filter->strides[0] +
                              filter->strides[1] * ((uint16_t[]){col_idx})[1] +
                              ((uint16_t[]){col_idx})[2])
                           : mat_ptr(filter, ((uint16_t[]){col_idx}),
                                     (sizeof((uint16_t[]){col_idx}) /
                                      sizeof(uint16_t)))));
      _def fixed *dest_ptr /*MAT_PTR*/ /*(dest, i, 0)*/;
      _WR(dest_ptr,
          (((sizeof((uint16_t[]){i, 0}) / sizeof(uint16_t)) == 1)
               ? (dest->data + ((uint16_t[]){i, 0})[0])
               : ((sizeof((uint16_t[]){i, 0}) / sizeof(uint16_t)) == 2)
                     ? (dest->data +
                        ((uint16_t[]){i, 0})[0] * dest->strides[0] +
                        ((uint16_t[]){i, 0})[1])
                     : ((sizeof((uint16_t[]){i, 0}) / sizeof(uint16_t)) == 3)
                           ? (dest->data +
                              ((uint16_t[]){i, 0})[0] * dest->strides[0] +
                              dest->strides[1] * ((uint16_t[]){i, 0})[1] +
                              ((uint16_t[]){i, 0})[2])
                           : mat_ptr(dest, ((uint16_t[]){i, 0}),
                                     (sizeof((uint16_t[]){i, 0}) /
                                      sizeof(uint16_t)))));
      _def uint16_t *offset;
      _WR(offset, filter->sparse.offsets + col_idx);

      /*
       if(i == pos_bak.i && j == pos_bak.j) { // Restore it i, j are the same
           *dest_ptr = val_bak;
       }*/
      if (start == end) {
        // val_bak = 0;
        // pos_bak.i = i;
        _WR(*dest_ptr, 0);
      }

      {
        _def uint16_t j;
        _WR(j, 0);
        for (; j < end - start;) {
          _def fixed w /*F_MUL*/ /*(MAT_GET(src, *offset, 0),
                                  *filter_ptr++)*/
              ;
          {
            _def fixed *tmp_var_0;
            _WR(tmp_var_0, filter_ptr);
            _WR_SELF(fixed *, filter_ptr, filter_ptr + 1);
            _WR(w,
                f_mul((((sizeof((uint16_t[]){*offset, 0}) / sizeof(uint16_t)) ==
                        1)
                           ? *(src->data + ((uint16_t[]){*offset, 0})[0])
                           : ((sizeof((uint16_t[]){*offset, 0}) /
                               sizeof(uint16_t)) == 2)
                                 ? *(src->data +
                                     ((uint16_t[]){*offset, 0})[0] *
                                         src->strides[0] +
                                     ((uint16_t[]){*offset, 0})[1])
                                 : ((sizeof((uint16_t[]){*offset, 0}) /
                                     sizeof(uint16_t)) == 3)
                                       ? *(src->data +
                                           ((uint16_t[]){*offset, 0})[0] *
                                               src->strides[0] +
                                           src->strides[1] *
                                               ((uint16_t[]){*offset, 0})[1] +
                                           ((uint16_t[]){*offset, 0})[2])
                                       : mat_get(
                                             src, ((uint16_t[]){*offset, 0}),
                                             (sizeof((uint16_t[]){*offset, 0}) /
                                              sizeof(uint16_t)))),
                      *tmp_var_0));
          }
          if (j == 0) {
            _WR(*dest_ptr, w); // Zeroing the vector
          } else {

            _WR_SELF(fixed, w, w + *dest_ptr) /*F_ADD*/ /*(w, *dest_ptr)*/;
            _WR(*dest_ptr, w);
          }
          _WR_SELF(uint16_t *, offset, offset + 1);
          _WR_SELF(uint16_t, j, j + 1);
        }
      }
      _WR_SELF(fixed *, dest_ptr, dest_ptr + 1);
      _WR_SELF(uint16_t, i, i + 1);
    }
  }
  _end(fc_sparse_mul);
}

_immortal_function(fc_dense_mul, mat_t *src, mat_t *dest, mat_t *filter) {
  _begin(fc_dense_mul);

  _def uint16_t rows /*MAT_GET_DIM*/ /*(filter, 0)*/;
  _WR(rows, (mat_get_dim(filter, 0)));
  _def uint16_t cols /*MAT_GET_DIM*/ /*(filter, 1)*/;
  _WR(cols, (mat_get_dim(filter, 1)));
  _def fixed *dest_ptr /*MAT_PTR*/ /*(dest, 0, 0)*/;
  _WR(dest_ptr,
      (((sizeof((uint16_t[]){0, 0}) / sizeof(uint16_t)) == 1)
           ? (dest->data + ((uint16_t[]){0, 0})[0])
           : ((sizeof((uint16_t[]){0, 0}) / sizeof(uint16_t)) == 2)
                 ? (dest->data + ((uint16_t[]){0, 0})[0] * dest->strides[0] +
                    ((uint16_t[]){0, 0})[1])
                 : ((sizeof((uint16_t[]){0, 0}) / sizeof(uint16_t)) == 3)
                       ? (dest->data +
                          ((uint16_t[]){0, 0})[0] * dest->strides[0] +
                          dest->strides[1] * ((uint16_t[]){0, 0})[1] +
                          ((uint16_t[]){0, 0})[2])
                       : mat_ptr(
                             dest, ((uint16_t[]){0, 0}),
                             (sizeof((uint16_t[]){0, 0}) / sizeof(uint16_t)))));

  {
    _def uint16_t i;
    _WR(i, 0);
    for (; i < rows;) {
      _def fixed *src_ptr /*MAT_PTR*/ /*(src, 0, 0)*/;
      _WR(src_ptr,
          (((sizeof((uint16_t[]){0, 0}) / sizeof(uint16_t)) == 1)
               ? (src->data + ((uint16_t[]){0, 0})[0])
               : ((sizeof((uint16_t[]){0, 0}) / sizeof(uint16_t)) == 2)
                     ? (src->data + ((uint16_t[]){0, 0})[0] * src->strides[0] +
                        ((uint16_t[]){0, 0})[1])
                     : ((sizeof((uint16_t[]){0, 0}) / sizeof(uint16_t)) == 3)
                           ? (src->data +
                              ((uint16_t[]){0, 0})[0] * src->strides[0] +
                              src->strides[1] * ((uint16_t[]){0, 0})[1] +
                              ((uint16_t[]){0, 0})[2])
                           : mat_ptr(src, ((uint16_t[]){0, 0}),
                                     (sizeof((uint16_t[]){0, 0}) /
                                      sizeof(uint16_t)))));
      _def uint16_t k;
      _WR(k, 0);
      {
        _def uint16_t j;
        _WR(j, 0);
        for (; j < cols;) {
          _def fixed w /*F_MUL*/ /*(MAT_GET(filter, i, j), *src_ptr)*/;
          _WR(w,
              f_mul(
                  (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 1)
                       ? *(filter->data + ((uint16_t[]){i, j})[0])
                       : ((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 2)
                             ? *(filter->data +
                                 ((uint16_t[]){i, j})[0] * filter->strides[0] +
                                 ((uint16_t[]){i, j})[1])
                             : ((sizeof((uint16_t[]){i, j}) /
                                 sizeof(uint16_t)) == 3)
                                   ? *(filter->data +
                                       ((uint16_t[]){i, j})[0] *
                                           filter->strides[0] +
                                       filter->strides[1] *
                                           ((uint16_t[]){i, j})[1] +
                                       ((uint16_t[]){i, j})[2])
                                   : mat_get(filter, ((uint16_t[]){i, j}),
                                             (sizeof((uint16_t[]){i, j}) /
                                              sizeof(uint16_t)))),
                  *src_ptr));
          if (k > 0) {
            _WR_SELF(fixed, w, w + *dest_ptr) /*F_ADD*/ /*(w, *dest_ptr)*/;
          }
          _WR(*dest_ptr, w);
          _WR_SELF(uint16_t, k, k + 1);
          _WR_SELF(fixed *, src_ptr, src_ptr + 1);
          _WR_SELF(uint16_t, j, j + 1);
        }
      }
      _WR_SELF(fixed *, dest_ptr, dest_ptr + 1);
      _WR_SELF(uint16_t, i, i + 1);
    }
  }
  _end(fc_dense_mul);
}

_immortal_function(ds_add, mat_t *src, mat_t *dest, mat_t *filter) {
  _begin(ds_add);

  _def uint16_t rows /*MAT_GET_DIM*/ /*(src, 0)*/;
  _WR(rows, (mat_get_dim(src, 0)));
  _def uint16_t cols /*MAT_GET_DIM*/ /*(src, 1)*/;
  _WR(cols, (mat_get_dim(src, 1)));
  {
    _def uint16_t i;
    _WR(i, 0);
    for (; i < rows;) {
      {
        _def uint16_t j;
        _WR(j, 0);
        for (; j < cols;) {
          _def fixed w /*F_ADD*/
              /*(MAT_GET(src, i, j), MAT_GET(filter, 0))*/;
          _WR(w,
              (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 1)
                   ? *(src->data + ((uint16_t[]){i, j})[0])
                   : ((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 2)
                         ? *(src->data +
                             ((uint16_t[]){i, j})[0] * src->strides[0] +
                             ((uint16_t[]){i, j})[1])
                         : ((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) ==
                            3)
                               ? *(src->data +
                                   ((uint16_t[]){i, j})[0] * src->strides[0] +
                                   src->strides[1] * ((uint16_t[]){i, j})[1] +
                                   ((uint16_t[]){i, j})[2])
                               : mat_get(src, ((uint16_t[]){i, j}),
                                         (sizeof((uint16_t[]){i, j}) /
                                          sizeof(uint16_t)))) +
                  (((sizeof((uint16_t[]){0}) / sizeof(uint16_t)) == 1)
                       ? *(filter->data + ((uint16_t[]){0})[0])
                       : ((sizeof((uint16_t[]){0}) / sizeof(uint16_t)) == 2)
                             ? *(filter->data +
                                 ((uint16_t[]){0})[0] * filter->strides[0] +
                                 ((uint16_t[]){0})[1])
                             : ((sizeof((uint16_t[]){0}) / sizeof(uint16_t)) ==
                                3)
                                   ? *(filter->data +
                                       ((uint16_t[]){0})[0] *
                                           filter->strides[0] +
                                       filter->strides[1] *
                                           ((uint16_t[]){0})[1] +
                                       ((uint16_t[]){0})[2])
                                   : mat_get(filter, ((uint16_t[]){0}),
                                             (sizeof((uint16_t[]){0}) /
                                              sizeof(uint16_t)))));
          if (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 1)) {
            _WR(*(dest->data + ((uint16_t[]){i, j})[0]), w);
          } else {
            if (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 3)) {
              _WR(*(dest->data + ((uint16_t[]){i, j})[0] * dest->strides[0] +
                    dest->strides[1] * ((uint16_t[]){i, j})[1] +
                    ((uint16_t[]){i, j})[2]),
                  w);
            } else {
              mat_set(dest, w, ((uint16_t[]){i, j}),
                      (sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)));
            }
          } /*MAT_SET*/ /*(dest, w, i, j)*/;
          _WR_SELF(uint16_t, j, j + 1);
        }
      }
      _WR_SELF(uint16_t, i, i + 1);
    }
  }
  _end(ds_add);
}

_immortal_function(ds_zero, mat_t *src, mat_t *dest) {
  _begin(ds_zero);
  _def uint16_t rows /*MAT_GET_DIM*/ /*(src, 0)*/;
  _WR(rows, (mat_get_dim(src, 0)));
  _def uint16_t cols /*MAT_GET_DIM*/ /*(src, 1)*/;
  _WR(cols, (mat_get_dim(src, 1)));
  {
    _def uint16_t i;
    _WR(i, 0);
    for (; i < rows;) {
      {
        _def uint16_t j;
        _WR(j, 0);
        for (; j < cols;) {
          if (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 1)) {
            _WR(*(dest->data + ((uint16_t[]){i, j})[0]), 0);
          } else {
            if (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 3)) {
              _WR(*(dest->data + ((uint16_t[]){i, j})[0] * dest->strides[0] +
                    dest->strides[1] * ((uint16_t[]){i, j})[1] +
                    ((uint16_t[]){i, j})[2]),
                  0);
            } else {
              mat_set(dest, 0, ((uint16_t[]){i, j}),
                      (sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)));
            }
          } /*MAT_SET*/ /*(dest, 0, i, j)*/;
          _WR_SELF(uint16_t, j, j + 1);
        }
      }
      _WR_SELF(uint16_t, i, i + 1);
    }
  }
  _end(ds_zero);
}

_immortal_function(dm_add, mat_t *src, mat_t *dest, mat_t *bias) {
  _begin(dm_add);

  // Bias

  _def uint16_t rows /*MAT_GET_DIM*/ /*(src, 0)*/;
  _WR(rows, (mat_get_dim(src, 0)));
  _def uint16_t cols /*MAT_GET_DIM*/ /*(src, 1)*/;
  _WR(cols, (mat_get_dim(src, 1)));

  {
    _def uint16_t i;
    _WR(i, 0);
    for (; i < rows;) {
      {
        _def uint16_t j;
        _WR(j, 0);
        for (; j < cols;) {
          _def fixed w /*F_ADD*/
              /*(MAT_GET(src, i, j), MAT_GET(bias, i, j))*/;
          _WR(w,
              (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 1)
                   ? *(src->data + ((uint16_t[]){i, j})[0])
                   : ((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 2)
                         ? *(src->data +
                             ((uint16_t[]){i, j})[0] * src->strides[0] +
                             ((uint16_t[]){i, j})[1])
                         : ((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) ==
                            3)
                               ? *(src->data +
                                   ((uint16_t[]){i, j})[0] * src->strides[0] +
                                   src->strides[1] * ((uint16_t[]){i, j})[1] +
                                   ((uint16_t[]){i, j})[2])
                               : mat_get(src, ((uint16_t[]){i, j}),
                                         (sizeof((uint16_t[]){i, j}) /
                                          sizeof(uint16_t)))) +
                  (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 1)
                       ? *(bias->data + ((uint16_t[]){i, j})[0])
                       : ((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 2)
                             ? *(bias->data +
                                 ((uint16_t[]){i, j})[0] * bias->strides[0] +
                                 ((uint16_t[]){i, j})[1])
                             : ((sizeof((uint16_t[]){i, j}) /
                                 sizeof(uint16_t)) == 3)
                                   ? *(bias->data +
                                       ((uint16_t[]){i, j})[0] *
                                           bias->strides[0] +
                                       bias->strides[1] *
                                           ((uint16_t[]){i, j})[1] +
                                       ((uint16_t[]){i, j})[2])
                                   : mat_get(bias, ((uint16_t[]){i, j}),
                                             (sizeof((uint16_t[]){i, j}) /
                                              sizeof(uint16_t)))));
          if (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 1)) {
            _WR(*(dest->data + ((uint16_t[]){i, j})[0]), w);
          } else {
            if (((sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)) == 3)) {
              _WR(*(dest->data + ((uint16_t[]){i, j})[0] * dest->strides[0] +
                    dest->strides[1] * ((uint16_t[]){i, j})[1] +
                    ((uint16_t[]){i, j})[2]),
                  w);
            } else {
              mat_set(dest, w, ((uint16_t[]){i, j}),
                      (sizeof((uint16_t[]){i, j}) / sizeof(uint16_t)));
            }
          } /*MAT_SET*/ /*(dest, w, i, j)*/;
          _WR_SELF(uint16_t, j, j + 1);
        }
      }
      _WR_SELF(uint16_t, i, i + 1);
    }
  }
  _end(dm_add);
}
