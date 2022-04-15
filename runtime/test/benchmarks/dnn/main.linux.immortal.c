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
#include "includes/fixedlib/fixed.h"
#include "includes/matlib/mat.h"
#include <stdio.h>
#include <stdlib.h>

#include "headers/conv1.h"
#include "headers/conv2.h"
#include "headers/fc1.h"
#include "headers/fc2.h"
#include "headers/input.h"

//#define PORT_CONTROL

#ifdef PF_SIMULATION
/*__fram int timer_count = 500;*/
/*__fram int timer_array[30] = {450, 600, 500, 480, 620, 570, 630, 450, 524,
 * 320,*/
/*658, 620, 630, 555, 587, 487, 496, 500, 400, 350,*/
/*390, 310, 615, 310, 596, 400, 409, 381, 636, 583};*/
/*__fram int count_t = 0;*/
/*immortalc_fn_ignore void Reset() { PMMCTL0 = 0x0008; }*/
#endif

__attribute__((section(".rodata"))) /*__ro_fram*/ mat_t mat_conv1_wd = {
    .dims = {20 /*CONV1_WMD_LEN*/},
    .len_dims = 1,
    .strides = {1},
    .data = conv1_wmd,
    .sparse =
        {
            .dims = {20, 1, 1, 1},
            .len_dims = 4,
            .sizes = conv1_wmd_sizes,
            .offsets = conv1_wmd_offsets,
        },
};

__attribute__((section(".rodata"))) /*__ro_fram*/ mat_t mat_conv1_wv = {
    .dims = {100 /*CONV1_WMV_LEN*/},
    .len_dims = 1,
    .strides = {1},
    .data = conv1_wmv,
    .sparse =
        {
            .dims = {20, 1, 5, 1},
            .len_dims = 4,
            .sizes = conv1_wmv_sizes,
            .offsets = conv1_wmv_offsets,
        },
};

__attribute__((section(".rodata"))) /*__ro_fram*/ mat_t mat_conv1_wh = {
    .dims = {100 /*CONV1_WMH_LEN*/},
    .len_dims = 1,
    .strides = {1},
    .data = conv1_wmh,
    .sparse =
        {
            .dims = {20, 1, 1, 5},
            .len_dims = 4,
            .sizes = conv1_wmh_sizes,
            .offsets = conv1_wmh_offsets,
        },
};

__attribute__((section(".rodata"))) /*__ro_fram*/ mat_t mat_conv1_b = {
    .dims = {20},
    .len_dims = 1,
    .strides = {1},
    .data = conv1_b,
};

__attribute__((section(".rodata"))) /*__ro_fram*/ mat_t mat_conv2_w = {
    .dims = {931 /*CONV2_WM_LEN*/},
    .len_dims = 1,
    .strides = {1},
    .data = conv2_wm,
    .sparse =
        {
            .dims = {100, 20, 5, 5},
            .len_dims = 4,
            .sizes = conv2_wm_sizes,
            .offsets = conv2_wm_offsets,
        },
};

__attribute__((section(".rodata"))) /*__ro_fram*/ mat_t mat_conv2_b = {
    .dims = {100},
    .strides = {1},
    .len_dims = 1,
    .data = conv2_b,
};

__attribute__((section(".rodata"))) /*__ro_fram*/ mat_t mat_fc1_wh = {
    .dims = {5456 /*FC1_WMH_LEN*/},
    .len_dims = 1,
    .strides = {1},
    .data = fc1_wmh,
    .sparse =
        {
            .dims = {100, 1600},
            .len_dims = 2,
            .offsets = fc1_wmh_offsets,
            .sizes = fc1_wmh_sizes,
        },
};

__attribute__((section(".rodata"))) /*__ro_fram*/ mat_t mat_fc1_wv = {
    .dims = {1892 /*FC1_WMV_LEN*/},
    .len_dims = 1,
    .strides = {1},
    .data = fc1_wmv,
    .sparse =
        {
            .dims = {500, 100},
            .len_dims = 2,
            .offsets = fc1_wmv_offsets,
            .sizes = fc1_wmv_sizes,
        },
};

__attribute__((section(".rodata"))) /*__ro_fram*/ mat_t mat_fc1_b = {
    .dims = {500, 1},
    .strides = {1, 1},
    .len_dims = 2,
    .data = fc1_b,
};

__attribute__((section(".rodata"))) /*__ro_fram*/ mat_t mat_fc2_w = {
    .dims = {10, 500},
    .strides = {500, 1},
    .len_dims = 2,
    .data = fc2_w,
};

__attribute__((section(".rodata"))) /*__ro_fram*/ mat_t mat_fc2_b = {
    .dims = {10, 1},
    .strides = {1, 1},
    .len_dims = 2,
    .data = fc2_b,
};

__attribute__((section(".rodata"))) /*__ro_fram*/ mat_t mat_input = {
    .dims = {1, 28, 28},
    .strides = {784, 28, 1},
    .len_dims = 3,
    .data = input,
};

__attribute__((section("persistent"))) /*__fram*/ mat_t buf1 = {
    .data = layer_buffers[0]};
__attribute__((section("persistent"))) /*__fram*/ mat_t buf2 = {
    .data = layer_buffers[1]};
__attribute__((section("persistent"))) /*__fram*/ mat_t *b1 = &buf1;
__attribute__((section("persistent"))) /*__fram*/ mat_t *b2 = &buf2;

_immortal_function(dnn_main, void *args) {
  _begin(dnn_main);
  while (1) {
    _WR(params.same_padding, 0) /*false*/;
    _WR(params.size[0], 1);
    _WR(params.size[1], 2);
    _WR(params.size[2], 2);
    _WR(params.stride[0], 1);
    _WR(params.stride[1], 1);
    _WR(params.stride[2], 1);

    //// INPUT //////
    (mat_reshape(b2, ((uint16_t[]){1, 28, 28}),
                 (sizeof((uint16_t[]){1, 28, 28}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b2, 1, 28, 28)*/;
    _def mat_t *mat_input_ptr;
    _WR(mat_input_ptr, &mat_input);
    {
      _def uint16_t i;
      _WR(i, 0);
      for (; i < 28;) {
        {
          _def uint16_t j;
          _WR(j, 0);
          for (; j < 28;) {
            _def fixed w /*MAT_GET*/
                /*(mat_input_ptr, 0, i, j)*/;
            _WR(w,
                (((sizeof((uint16_t[]){0, i, j}) / sizeof(uint16_t)) == 1)
                     ? *(mat_input_ptr->data + ((uint16_t[]){0, i, j})[0])
                     : ((sizeof((uint16_t[]){0, i, j}) / sizeof(uint16_t)) == 2)
                           ? *(mat_input_ptr->data +
                               ((uint16_t[]){0, i, j})[0] *
                                   mat_input_ptr->strides[0] +
                               ((uint16_t[]){0, i, j})[1])
                           : ((sizeof((uint16_t[]){0, i, j}) /
                               sizeof(uint16_t)) == 3)
                                 ? *(mat_input_ptr->data +
                                     ((uint16_t[]){0, i, j})[0] *
                                         mat_input_ptr->strides[0] +
                                     mat_input_ptr->strides[1] *
                                         ((uint16_t[]){0, i, j})[1] +
                                     ((uint16_t[]){0, i, j})[2])
                                 : mat_get(mat_input_ptr,
                                           ((uint16_t[]){0, i, j}),
                                           (sizeof((uint16_t[]){0, i, j}) /
                                            sizeof(uint16_t)))));
            if (((sizeof((uint16_t[]){0, i, j}) / sizeof(uint16_t)) == 1)) {
              _WR(*(b2->data + ((uint16_t[]){0, i, j})[0]), w);
            } else {
              if (((sizeof((uint16_t[]){0, i, j}) / sizeof(uint16_t)) == 3)) {
                _WR(*(b2->data + ((uint16_t[]){0, i, j})[0] * b2->strides[0] +
                      b2->strides[1] * ((uint16_t[]){0, i, j})[1] +
                      ((uint16_t[]){0, i, j})[2]),
                    w);
              } else {
                mat_set(b2, w, ((uint16_t[]){0, i, j}),
                        (sizeof((uint16_t[]){0, i, j}) / sizeof(uint16_t)));
              }
            } /*MAT_SET*/
            /*(b2, w, 0, i, j)*/;
            _WR_SELF(uint16_t, j, j + 1);
          }
        }
        _WR_SELF(uint16_t, i, i + 1);
      }
    }

    /////////////CONVOL 1_D//////////////
    (mat_reshape(b1, ((uint16_t[]){20, 28, 28}),
                 (sizeof((uint16_t[]){20, 28, 28}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b1, 20, 28, 28)*/;
    _def mat_t *w_ptr;
    _WR(w_ptr, &mat_conv1_wd);
    _def mat_t *b_ptr /*NULL*/;
    _WR(b_ptr, ((void *)0));
    _call(convol, b2, b1, w_ptr, b_ptr);

    /////////CONVOL 1_H//////////////////
    _WR(w_ptr, &mat_conv1_wh);
    (mat_reshape(b2, ((uint16_t[]){20, 28, 24}),
                 (sizeof((uint16_t[]){20, 28, 24}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b2, 20, 28, 24)*/;
    _call(convol_depth, b1, b2, w_ptr, b_ptr);

    ////////CONVOL 1_W///////////////////
    _WR(b_ptr, &mat_conv1_b);
    _WR(w_ptr, &mat_conv1_wv);
    (mat_reshape(b1, ((uint16_t[]){20, 24, 24}),
                 (sizeof((uint16_t[]){20, 24, 24}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b1, 20, 24, 24)*/;
    _call(convol_depth, b2, b1, w_ptr, b_ptr);

    /////////  RELU  ///////////////////
    (mat_reshape(b2, ((uint16_t[]){20, 24, 24}),
                 (sizeof((uint16_t[]){20, 24, 24}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b2, 20, 24, 24)*/;
    _call(relu, b1, b2);

    ////////  POOLING  ///////////////////
    (mat_reshape(b1, ((uint16_t[]){20, 12, 12}),
                 (sizeof((uint16_t[]){20, 12, 12}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b1, 20, 12, 12)*/;
    _WR(params.stride[1], 2);
    _WR(params.stride[2], 2);
    _call(pooling, b2, b1);

    /////// CONVOL 2 /////////////////////
    (mat_reshape(b2, ((uint16_t[]){100, 8, 8}),
                 (sizeof((uint16_t[]){100, 8, 8}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b2, 100, 8, 8)*/;
    _WR(params.stride[1], 1);
    _WR(params.stride[2], 1);
    _WR(b_ptr, &mat_conv2_b);
    _WR(w_ptr, &mat_conv2_w);
    _call(convol, b1, b2, w_ptr, b_ptr);

    /////// RELU /////////////////////
    (mat_reshape(b1, ((uint16_t[]){100, 8, 8}),
                 (sizeof((uint16_t[]){100, 8, 8}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b1, 100, 8, 8)*/;
    _call(relu, b2, b1);

    ////////  POOLING  ///////////////////
    (mat_reshape(b2, ((uint16_t[]){100, 4, 4}),
                 (sizeof((uint16_t[]){100, 4, 4}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b2, 100, 4, 4)*/;
    _WR(params.stride[1], 2);
    _WR(params.stride[2], 2);
    _call(pooling, b1, b2);

    ////////     FC1_H    ///////////////////
    (mat_reshape(b2, ((uint16_t[]){1600, 1}),
                 (sizeof((uint16_t[]){1600, 1}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b2, 1600, 1)*/;
    (mat_reshape(b1, ((uint16_t[]){100, 1}),
                 (sizeof((uint16_t[]){100, 1}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b1, 100, 1)*/;
    _WR(w_ptr, &mat_fc1_wh);
    _WR(b_ptr, ((void *)0)) /*NULL*/;
    _call(fc_sparse, b2, b1, w_ptr, b_ptr);

    ////////     FC1_W    ///////////////////
    (mat_reshape(b2, ((uint16_t[]){500, 1}),
                 (sizeof((uint16_t[]){500, 1}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b2, 500, 1)*/;
    _WR(w_ptr, &mat_fc1_wv);
    _WR(b_ptr, &mat_fc1_b);
    _call(fc_sparse, b1, b2, w_ptr, b_ptr);

    /////////     RELU   /////////////////////
    (mat_reshape(b1, ((uint16_t[]){500, 1}),
                 (sizeof((uint16_t[]){500, 1}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b1, 500, 1)*/;
    _call(relu, b2, b1);

    /////////    FC 2    ////////////////////
    (mat_reshape(b2, ((uint16_t[]){10, 1}),
                 (sizeof((uint16_t[]){10, 1}) /
                  sizeof(uint16_t)))) /*MAT_RESHAPE*/ /*(b2, 10, 1)*/;
    _WR(w_ptr, &mat_fc2_w);
    _WR(b_ptr, &mat_fc2_b);
    _call(fc_dense, b1, b2, w_ptr, b_ptr);

    _def uint16_t predict;
    _call(prediction, &predict, b2);
#ifdef IMMORTALITY_PORT_LINUX
    printf("%u\n", predict);
#endif
    if (predict == 3) {
#ifdef __MSP430__
#if defined(PORT_CONTROL)
      /*P3OUT ^= 0x02;*/
      /*P3OUT ^= 0x02;*/
#else
      /*P1OUT = 0x02;*/
      /*_BIC_SR(GIE);*/
      /*while (1)*/
      /*;*/
#endif
#endif
    } else {
#ifdef __MSP430__
#if defined(PORT_CONTROL)
      /*P3OUT ^= 0x02;*/
      /*P3OUT ^= 0x02;*/
#else
      /*P1OUT = 0x01;*/
      /*_BIC_SR(GIE);*/
      /*while (1)*/
      /*;*/
#endif
#endif
    }
  }
  _end(dnn_main);
}

_gdef static immortal_thread_t threads[1];

int main(int argc, char *argv[]) {
#ifdef IMMORTALITY_PORT_LINUX
  im_port_linux_init(argc, argv);
#endif

#ifdef __MSP430__
  /*WDTCTL = WDTPW | WDTHOLD;*/ // stop watchdog timer
  /*PM5CTL0 &= ~LOCKLPM5;*/ // Disable the GPIO power-on default high-impedance
                            // mode
#if defined(PORT_CONTROL)
  /*P3DIR = 0xFF;*/
#else
  /*P1OUT = 0x00;*/
  /*P1DIR = 0x03;*/
#endif
#endif
  _imt_thread_init_macro(&threads[0], dnn_main, ((void *)0) /*NULL*/);
  imt_run(threads, 1, 0 /*false*/);
  return 0;
}

#ifdef PF_SIMULATION

/*immortalc_fn_ignore void Timer_A1_set() {*/
/*TA1CCR0 = timer_count;*/        // max 65535
/*TA1CTL = TASSEL__ACLK + MC_1;*/ // set the max period for 16bit timer
                                  // operation
/*TA1CCTL0 = CCIE;*/              // enable compare reg 0
//    _BIS_SR( GIE); //ENABLE GLOBAL INTERRRUPTS
/*}*/

/*immortalc_fn_ignore void RebootHandler() {*/
/*Timer_A1_set();*/ // Power failure simulation
                    /*}*/

// Timer A0 interrupt service routine
/*void __attribute__((interrupt(TIMER1_A0_VECTOR))) TimerA1(void) {*/
/*if (count_t == 30)*/
/*count_t = 0;*/
/*timer_count = timer_array[count_t];*/
/*count_t++;*/
#ifdef PORT_CONTROL
/*P3OUT ^= 0x01;*/
/*P3OUT ^= 0x01;*/
#endif
/*Reset();*/
/*}*/

#endif
