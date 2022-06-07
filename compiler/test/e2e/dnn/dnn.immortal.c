/*
 * dnn.c
 *
 *  Created on: 24 Haz 2020
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
#include <stdlib.h>

__attribute__((section("persistent"))) /*__fram*/ dnn_param params;

__attribute__((section(".persistent_hifram "))) /*__hifram*/ fixed
    layer_buffers[3 /*LAYER_BUF_NUMBER*/][0x3D41 /*CONFIG_LAYER_BUF_SIZE*/];
__attribute__((section(".persistent_hifram "))) /*__hifram*/ fixed
    mat_buffers[3 /*MAT_BUF_NUMBER*/][0x310 /*MAT_BUF_SIZE*/];

static __attribute__((section("persistent"))) /*__fram*/ mat_t m = {
    .data = layer_buffers[2]};
static __attribute__((section("persistent"))) /*__fram*/ mat_t *inter = &m;
static __attribute__((section("persistent"))) /*__fram*/ mat_t c_src, c_filter,
    c_dest, c_inter;
static __attribute__((section("persistent"))) /*__fram*/ mat_t *c_filter_ptr =
    &c_filter;
static __attribute__((section("persistent"))) /*__fram*/ mat_t *c_src_ptr =
    &c_src;
static __attribute__((section("persistent"))) /*__fram*/ mat_t *c_dest_ptr =
    &c_dest;
static __attribute__((section("persistent"))) /*__fram*/ mat_t *c_inter_ptr =
    &c_inter;

_immortal_function(pooling, mat_t *src, mat_t *dest) {
  _begin(pooling);
  _def uint16_t layers /*MAT_GET_DIM*/ /*(src, 0)*/;
  _WR(layers, (mat_get_dim(src, 0)));
  _def uint16_t rows /*MAT_GET_DIM*/ /*(src, 1)*/;
  _WR(rows, (mat_get_dim(src, 1)));
  _def uint16_t k;
  _def uint16_t j;
  {
    _def uint16_t i;
    _WR(i, 0);
    for (; i < layers;) {
      {
        _WR(j, 0);
        for (; j < rows;) {
          {
            _WR(k, 0);
            for (; k < rows;) {
              _def fixed max /*MAT_GET*/
                  /*(src, i, j, k)*/;
              _WR(max,
                  (((sizeof((uint16_t[]){i, j, k}) / sizeof(uint16_t)) == 1)
                       ? *(src->data + ((uint16_t[]){i, j, k})[0])
                       : ((sizeof((uint16_t[]){i, j, k}) / sizeof(uint16_t)) ==
                          2)
                             ? *(src->data +
                                 ((uint16_t[]){i, j, k})[0] * src->strides[0] +
                                 ((uint16_t[]){i, j, k})[1])
                             : ((sizeof((uint16_t[]){i, j, k}) /
                                 sizeof(uint16_t)) == 3)
                                   ? *(src->data +
                                       ((uint16_t[]){i, j, k})[0] *
                                           src->strides[0] +
                                       src->strides[1] *
                                           ((uint16_t[]){i, j, k})[1] +
                                       ((uint16_t[]){i, j, k})[2])
                                   : mat_get(src, ((uint16_t[]){i, j, k}),
                                             (sizeof((uint16_t[]){i, j, k}) /
                                              sizeof(uint16_t)))));
              {
                _def uint16_t l;
                _WR(l, 0);
                for (; l < params.size[1];) {
                  {
                    _def uint16_t m;
                    _WR(m, 0);
                    for (; m < params.size[2];) {
                      _def fixed val /*MAT_GET*/
                          /*(src, i, j + l, k + m)*/;
                      _WR(val,
                          (((sizeof((uint16_t[]){i, j + l, k + m}) /
                             sizeof(uint16_t)) == 1)
                               ? *(src->data +
                                   ((uint16_t[]){i, j + l, k + m})[0])
                               : ((sizeof((uint16_t[]){i, j + l, k + m}) /
                                   sizeof(uint16_t)) == 2)
                                     ? *(src->data +
                                         ((uint16_t[]){i, j + l, k + m})[0] *
                                             src->strides[0] +
                                         ((uint16_t[]){i, j + l, k + m})[1])
                                     : ((sizeof((uint16_t[]){i, j + l, k + m}) /
                                         sizeof(uint16_t)) == 3)
                                           ? *(src->data +
                                               ((uint16_t[]){i, j + l,
                                                             k + m})[0] *
                                                   src->strides[0] +
                                               src->strides[1] *
                                                   ((uint16_t[]){i, j + l,
                                                                 k + m})[1] +
                                               ((uint16_t[]){i, j + l,
                                                             k + m})[2])
                                           : mat_get(src,
                                                     ((uint16_t[]){i, j + l,
                                                                   k + m}),
                                                     (sizeof((uint16_t[]){
                                                          i, j + l, k + m}) /
                                                      sizeof(uint16_t)))));
                      if (max < val /*F_LT*/ /*(max, val)*/) {
                        _WR(max, val);
                      }
                      _WR_SELF(uint16_t, m, m + 1);
                    }
                  }
                  _WR_SELF(uint16_t, l, l + 1);
                }
              }
              if (((sizeof((uint16_t[]){i, j / params.stride[1],
                                        k / params.stride[2]}) /
                    sizeof(uint16_t)) == 1)) {
                _WR(*(dest->data + ((uint16_t[]){i, j / params.stride[1],
                                                 k / params.stride[2]})[0]),
                    max);
              } else {
                if (((sizeof((uint16_t[]){i, j / params.stride[1],
                                          k / params.stride[2]}) /
                      sizeof(uint16_t)) == 3)) {
                  _WR(*(dest->data +
                        ((uint16_t[]){i, j / params.stride[1],
                                      k / params.stride[2]})[0] *
                            dest->strides[0] +
                        dest->strides[1] *
                            ((uint16_t[]){i, j / params.stride[1],
                                          k / params.stride[2]})[1] +
                        ((uint16_t[]){i, j / params.stride[1],
                                      k / params.stride[2]})[2]),
                      max);
                } else {
                  mat_set(dest, max,
                          ((uint16_t[]){i, j / params.stride[1],
                                        k / params.stride[2]}),
                          (sizeof((uint16_t[]){i, j / params.stride[1],
                                               k / params.stride[2]}) /
                           sizeof(uint16_t)));
                }
              } /*MAT_SET*/
              /*(dest, max, i, j / params.stride[1], k / params.stride[2])*/;
              _WR_SELF(uint16_t, k, k + params.stride[2]);
            }
          }
          _WR(k, 0);
          _WR_SELF(uint16_t, j, j + params.stride[1]);
        }
      }
      _WR(j, 0);
      _WR_SELF(uint16_t, i, i + 1);
    }
  }
  _end(pooling);
}

_immortal_function(relu, mat_t *src, mat_t *dest) {
  _begin(relu);

  _def uint16_t total_elements /*MAT_GET_DIM*/ /*(src, 1)*/;
  _WR(total_elements, (mat_get_dim(src, 0)) /*MAT_GET_DIM*/ /*(src, 0)*/ *
                          (mat_get_dim(src, 1)));
  if (src->len_dims == 3) {
    _WR_SELF(uint16_t, total_elements, total_elements * (mat_get_dim(src, 2)))
    /*MAT_GET_DIM*/ /*(src, 2)*/;
  }
  _def fixed max /*F_LIT*/ /*(0.0)*/;
  _WR(max, (fixed)(0.0 * (1 << 5)));
  {
    _def uint16_t i;
    _WR(i, 0);
    for (; i < total_elements;) {
      _WR(max, *(src->data + i));
      _WR(*(dest->data + i),
          (max < (fixed)(0.0 * (1 << 5)) /*F_LT*/ /*(max, F_LIT(0.0))*/)
              ? (fixed)(0.0 * (1 << 5)) /*F_LIT*/ /*(0.0)*/
              : max);
      _WR_SELF(uint16_t, i, i + 1);
    }
  }
  _end(relu);
}

_immortal_function_with_retval(prediction, uint16_t, mat_t *src) {
  _begin(prediction);
  _def uint16_t predict;
  _def fixed max;
  _WR(max, 0);

  {
    _def uint16_t i;
    _WR(i, 0);
    for (; i < 10;) {
      _def fixed v /*MAT_GET*/ /*(src, i, 0)*/;
      _WR(v,
          (((sizeof((uint16_t[]){i, 0}) / sizeof(uint16_t)) == 1)
               ? *(src->data + ((uint16_t[]){i, 0})[0])
               : ((sizeof((uint16_t[]){i, 0}) / sizeof(uint16_t)) == 2)
                     ? *(src->data + ((uint16_t[]){i, 0})[0] * src->strides[0] +
                         ((uint16_t[]){i, 0})[1])
                     : ((sizeof((uint16_t[]){i, 0}) / sizeof(uint16_t)) == 3)
                           ? *(src->data +
                               ((uint16_t[]){i, 0})[0] * src->strides[0] +
                               src->strides[1] * ((uint16_t[]){i, 0})[1] +
                               ((uint16_t[]){i, 0})[2])
                           : mat_get(src, ((uint16_t[]){i, 0}),
                                     (sizeof((uint16_t[]){i, 0}) /
                                      sizeof(uint16_t)))));
      if (v > max) {
        _WR(predict, i);
        _WR(max, v);
      }
      _WR_SELF(uint16_t, i, i + 1);
    }
  }
  _return(prediction, predict);
  _end(prediction);
}

_immortal_function(convol, mat_t *src, mat_t *dest, mat_t *w, mat_t *b) {
  _begin(convol);

  mat_reshape(inter, dest->dims, dest->len_dims);
  _def uint16_t filters;
  _WR(filters, w->sparse.dims[0]);

  _def uint16_t running_size;
  _WR(running_size, 0);

  {
    _def uint16_t i;
    _WR(i, 0);
    for (; i < filters;) {
      if (w->sparse.sizes[i] > 0) {

        _WR(c_filter, (mat_constrain(w, ((uint16_t[]){running_size}),
                                     (sizeof((uint16_t[]){running_size}) /
                                      sizeof(uint16_t))))) /*MAT_CONSTRAIN*/
        /*(w, running_size)*/;                             // Selecting
                                                           // filter
        _WR(c_filter.dims[0], w->sparse.sizes[i]);
        _WR(c_filter.sparse.len_dims, w->sparse.len_dims - 1);
        _WR(c_inter,
            (b == ((void *)0) /*NULL*/)
                ? (mat_constrain(
                      dest, ((uint16_t[]){i}),
                      (sizeof((uint16_t[]){i}) /
                       sizeof(uint16_t)))) /*MAT_CONSTRAIN*/ /*(dest, i)*/
                : (mat_constrain(inter, ((uint16_t[]){i}),
                                 (sizeof((uint16_t[]){i}) / sizeof(uint16_t)))))
        /*MAT_CONSTRAIN*/ /*(*/
        /*inter, i)*/;    // destination of selected filter's feature map

        _WR_SELF(uint16_t, running_size,
                 running_size +
                     w->sparse.sizes[i]); // Filter element size positio
        _call(con_mul, src, c_inter_ptr, c_filter_ptr); // convolving
      } else {

        _WR(c_inter,
            (b == ((void *)0) /*NULL*/)
                ? (mat_constrain(
                      dest, ((uint16_t[]){i}),
                      (sizeof((uint16_t[]){i}) /
                       sizeof(uint16_t)))) /*MAT_CONSTRAIN*/ /*(dest, i)*/
                : (mat_constrain(inter, ((uint16_t[]){i}),
                                 (sizeof((uint16_t[]){i}) / sizeof(uint16_t)))))
        /*MAT_CONSTRAIN*/ /*(inter, i)*/;
        _call(ds_zero, src, c_inter_ptr);
      }
      _WR_SELF(uint16_t, i, i + 1);
    }
  }
  if (b != ((void *)0) /*NULL*/) {
    {
      _def uint16_t i;
      _WR(i, 0);
      for (; i < filters;) {

        // Assumes filter, dest, src in that order
        _WR(c_inter,
            (mat_constrain(inter, ((uint16_t[]){i}),
                           (sizeof((uint16_t[]){i}) / sizeof(uint16_t)))))
        /*MAT_CONSTRAIN*/ /*(inter, i)*/;
        _WR(c_filter,
            (mat_constrain(b, ((uint16_t[]){i}),
                           (sizeof((uint16_t[]){i}) / sizeof(uint16_t)))))
        /*MAT_CONSTRAIN*/ /*(b, i)*/;
        _WR(c_dest,
            (mat_constrain(dest, ((uint16_t[]){i}),
                           (sizeof((uint16_t[]){i}) / sizeof(uint16_t)))))
        /*MAT_CONSTRAIN*/ /*(dest, i)*/;

        _call(ds_add, c_inter_ptr, c_dest_ptr, c_filter_ptr);
        _WR_SELF(uint16_t, i, i + 1);
      }
    }
  }
  _end(convol);
}

_immortal_function(convol_depth, mat_t *src, mat_t *dest, mat_t *w, mat_t *b) {
  _begin(convol_depth);

  mat_reshape(inter, dest->dims, dest->len_dims);
  _def uint16_t filters;
  _WR(filters, w->sparse.dims[0]);
  _def uint16_t running_size;
  _WR(running_size, 0);

  {
    _def uint16_t i;
    _WR(i, 0);
    for (; i < filters;) {
      if (w->sparse.sizes[i] > 0) {

        _WR(c_filter, (mat_constrain(w, ((uint16_t[]){running_size}),
                                     (sizeof((uint16_t[]){running_size}) /
                                      sizeof(uint16_t))))) /*MAT_CONSTRAIN*/
        /*(w, running_size)*/;                             // Selecting
                                                           // filter
        _WR(c_filter.dims[0], w->sparse.sizes[i]);
        _WR(c_filter.sparse.len_dims, w->sparse.len_dims - 1);
        _WR(c_inter,
            (b == ((void *)0) /*NULL*/)
                ? (mat_constrain(
                      dest, ((uint16_t[]){i}),
                      (sizeof((uint16_t[]){i}) /
                       sizeof(uint16_t)))) /*MAT_CONSTRAIN*/ /*(dest, i)*/
                : (mat_constrain(inter, ((uint16_t[]){i}),
                                 (sizeof((uint16_t[]){i}) / sizeof(uint16_t)))))
        /*MAT_CONSTRAIN*/ /*(*/
        /*inter, i)*/;    // destination of selected filter's feature map
        _WR(c_src,
            (mat_constrain(src, ((uint16_t[]){i}),
                           (sizeof((uint16_t[]){i}) / sizeof(uint16_t)))))
        /*MAT_CONSTRAIN*/ /*(src, i)*/;
        (mat_reshape(
            c_src_ptr,
            ((uint16_t[]){1, (mat_get_dim(src, 1)), (mat_get_dim(src, 2))}),
            (sizeof((uint16_t[]){1, (mat_get_dim(src, 1)),
                                 (mat_get_dim(src, 2))}) /
             sizeof(uint16_t)))) /*MAT_RESHAPE*/
            /*(c_src_ptr, 1, MAT_GET_DIM(src, 1), MAT_GET_DIM(src, 2))*/;
        _WR_SELF(uint16_t, running_size,
                 running_size +
                     w->sparse.sizes[i]); // Filter element size position

        _call(con_mul, c_src_ptr, c_inter_ptr, c_filter_ptr); // convolving
      }
      _WR_SELF(uint16_t, i, i + 1);
    }
  }
  if (b != ((void *)0) /*NULL*/) {
    {
      _def uint16_t i;
      _WR(i, 0);
      for (; i < filters;) {

        // Assumes filter, dest, src in that order
        _WR(c_inter,
            (mat_constrain(inter, ((uint16_t[]){i}),
                           (sizeof((uint16_t[]){i}) / sizeof(uint16_t)))))
        /*MAT_CONSTRAIN*/ /*(inter, i)*/;
        _WR(c_filter,
            (mat_constrain(b, ((uint16_t[]){i}),
                           (sizeof((uint16_t[]){i}) / sizeof(uint16_t)))))
        /*MAT_CONSTRAIN*/ /*(b, i)*/;
        _WR(c_dest,
            (mat_constrain(dest, ((uint16_t[]){i}),
                           (sizeof((uint16_t[]){i}) / sizeof(uint16_t)))))
        /*MAT_CONSTRAIN*/ /*(dest, i)*/;

        _call(ds_add, c_inter_ptr, c_dest_ptr, c_filter_ptr);
        _WR_SELF(uint16_t, i, i + 1);
      }
    }
  }
  _end(convol_depth);
}
/*
void convol_biasing (mat_t *src, mat_t *dest,  mat_t *w, mat_t *bias){



   //Biasing

    uint16_t b_rows=dest->dims[1];
    uint16_t b_cols=dest->dims[2];
    uint16_t total_elements= b_rows*b_cols;
    uint16_t filters = w->sparse.dims[0];
    uint16_t i,j;


    for(i=0;i<filters;i++){
        for(j=0;j<total_elements;j++){

            if(bias == NULL) {
                fixed sw = MAT_GET(src, total_elements*i + j);
                MAT_SET(dest, sw, i,j);
              }else {
                fixed sw = F_ADD(MAT_GET(src, total_elements*i +
j),MAT_GET(bias, i)); MAT_SET(dest, sw, i,j);
              }
        }
    }


}
*/
_immortal_function(fc_sparse, mat_t *src, mat_t *dest, mat_t *w, mat_t *b) {
  _begin(fc_sparse);

  mat_reshape(inter, dest->dims, dest->len_dims);

  // Sparse mat mul

  _call(fc_sparse_mul, src, (b == ((void *)0) /*NULL*/) ? dest : inter,
        w); // Sparse mat mul

  if (b != ((void *)0) /*NULL*/) {
    // Bias
    _call(dm_add, inter, dest, b);
  }
  _end(fc_sparse);
}

_immortal_function(fc_dense, mat_t *src, mat_t *dest, mat_t *w, mat_t *b) {
  _begin(fc_dense);
  mat_reshape(inter, dest->dims, dest->len_dims);

  // Dense mat mul
  _call(fc_dense_mul, src, (b == ((void *)0) /*NULL*/) ? dest : inter, w);
  if (b != ((void *)0) /*NULL*/) {
    // Bias
    _call(dm_add, inter, dest, b);
  }
  _end(fc_dense);
}

/*
void fc_biasing (mat_t *src, mat_t *dest, mat_t *bias){

    // Bias

        uint16_t rows = MAT_GET_DIM(src, 0);
        uint16_t cols = MAT_GET_DIM(src, 1);

        for(uint16_t i = 0; i < rows; i++ ) {
            for(uint16_t j = 0; j < cols; j++) {

                if(bias == NULL) {
                    fixed w = MAT_GET(src, i, j);
                    MAT_SET(dest, w, i, j);
                }else{
                    fixed w = F_ADD(MAT_GET(src, i, j), MAT_GET(bias, i, j));
                    MAT_SET(dest, w, i, j);
                }

            }

        }

}

*/

