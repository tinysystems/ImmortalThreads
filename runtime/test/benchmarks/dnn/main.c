#include "immortality.h"

#include "includes/dnnlib/dnn.h"
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
__fram int timer_count = 500;
__fram int timer_array[30] = {450, 600, 500, 480, 620, 570, 630, 450, 524, 320,
                              658, 620, 630, 555, 587, 487, 496, 500, 400, 350,
                              390, 310, 615, 310, 596, 400, 409, 381, 636, 583};
__fram int count_t = 0;
immortalc_fn_ignore void Reset() { PMMCTL0 = 0x0008; }
#endif

__ro_fram mat_t mat_conv1_wd = {
    .dims = {CONV1_WMD_LEN},
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

__ro_fram mat_t mat_conv1_wv = {
    .dims = {CONV1_WMV_LEN},
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

__ro_fram mat_t mat_conv1_wh = {
    .dims = {CONV1_WMH_LEN},
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

__ro_fram mat_t mat_conv1_b = {
    .dims = {20},
    .len_dims = 1,
    .strides = {1},
    .data = conv1_b,
};

__ro_fram mat_t mat_conv2_w = {
    .dims = {CONV2_WM_LEN},
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

__ro_fram mat_t mat_conv2_b = {
    .dims = {100},
    .strides = {1},
    .len_dims = 1,
    .data = conv2_b,
};

__ro_fram mat_t mat_fc1_wh = {
    .dims = {FC1_WMH_LEN},
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

__ro_fram mat_t mat_fc1_wv = {
    .dims = {FC1_WMV_LEN},
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

__ro_fram mat_t mat_fc1_b = {
    .dims = {500, 1},
    .strides = {1, 1},
    .len_dims = 2,
    .data = fc1_b,
};

__ro_fram mat_t mat_fc2_w = {
    .dims = {10, 500},
    .strides = {500, 1},
    .len_dims = 2,
    .data = fc2_w,
};

__ro_fram mat_t mat_fc2_b = {
    .dims = {10, 1},
    .strides = {1, 1},
    .len_dims = 2,
    .data = fc2_b,
};

__ro_fram mat_t mat_input = {
    .dims = {1, 28, 28},
    .strides = {784, 28, 1},
    .len_dims = 3,
    .data = input,
};

__fram mat_t buf1 = {.data = layer_buffers[0]};
__fram mat_t buf2 = {.data = layer_buffers[1]};
__fram mat_t *b1 = &buf1;
__fram mat_t *b2 = &buf2;

immortal_thread(dnn_main, args) {
  while (1) {
    params.same_padding = false;
    params.size[0] = 1;
    params.size[1] = 2;
    params.size[2] = 2;
    params.stride[0] = 1;
    params.stride[1] = 1;
    params.stride[2] = 1;

    //// INPUT //////
    MAT_RESHAPE(b2, 1, 28, 28);
    mat_t *mat_input_ptr = &mat_input;
    for (uint16_t i = 0; i < 28; i++) {
      for (uint16_t j = 0; j < 28; j++) {
        fixed w = MAT_GET(mat_input_ptr, 0, i, j);
        MAT_SET(b2, w, 0, i, j);
      }
    }

    /////////////CONVOL 1_D//////////////
    MAT_RESHAPE(b1, 20, 28, 28);
    mat_t *w_ptr = &mat_conv1_wd;
    mat_t *b_ptr = NULL;
    convol(b2, b1, w_ptr, b_ptr);

    /////////CONVOL 1_H//////////////////
    w_ptr = &mat_conv1_wh;
    MAT_RESHAPE(b2, 20, 28, 24);
    convol_depth(b1, b2, w_ptr, b_ptr);

    ////////CONVOL 1_W///////////////////
    b_ptr = &mat_conv1_b;
    w_ptr = &mat_conv1_wv;
    MAT_RESHAPE(b1, 20, 24, 24);
    convol_depth(b2, b1, w_ptr, b_ptr);

    /////////  RELU  ///////////////////
    MAT_RESHAPE(b2, 20, 24, 24);
    relu(b1, b2);

    ////////  POOLING  ///////////////////
    MAT_RESHAPE(b1, 20, 12, 12);
    params.stride[1] = 2;
    params.stride[2] = 2;
    pooling(b2, b1);

    /////// CONVOL 2 /////////////////////
    MAT_RESHAPE(b2, 100, 8, 8);
    params.stride[1] = 1;
    params.stride[2] = 1;
    b_ptr = &mat_conv2_b;
    w_ptr = &mat_conv2_w;
    convol(b1, b2, w_ptr, b_ptr);

    /////// RELU /////////////////////
    MAT_RESHAPE(b1, 100, 8, 8);
    relu(b2, b1);

    ////////  POOLING  ///////////////////
    MAT_RESHAPE(b2, 100, 4, 4);
    params.stride[1] = 2;
    params.stride[2] = 2;
    pooling(b1, b2);

    ////////     FC1_H    ///////////////////
    MAT_RESHAPE(b2, 1600, 1);
    MAT_RESHAPE(b1, 100, 1);
    w_ptr = &mat_fc1_wh;
    b_ptr = NULL;
    fc_sparse(b2, b1, w_ptr, b_ptr);

    ////////     FC1_W    ///////////////////
    MAT_RESHAPE(b2, 500, 1);
    w_ptr = &mat_fc1_wv;
    b_ptr = &mat_fc1_b;
    fc_sparse(b1, b2, w_ptr, b_ptr);

    /////////     RELU   /////////////////////
    MAT_RESHAPE(b1, 500, 1);
    relu(b2, b1);

    /////////    FC 2    ////////////////////
    MAT_RESHAPE(b2, 10, 1);
    w_ptr = &mat_fc2_w;
    b_ptr = &mat_fc2_b;
    fc_dense(b1, b2, w_ptr, b_ptr);

    uint16_t predict = prediction(b2);
#ifdef IMMORTALITY_PORT_LINUX
    printf("%u\n", predict);
#endif
    if (predict == 3) {
#ifdef __MSP430__
#if defined(PORT_CONTROL)
      P3OUT ^= 0x02;
      P3OUT ^= 0x02;
#else
      P1OUT = 0x02;
      _BIC_SR(GIE);
      while (1)
        ;
#endif
#endif
    } else {
#ifdef __MSP430__
#if defined(PORT_CONTROL)
      P3OUT ^= 0x02;
      P3OUT ^= 0x02;
#else
      P1OUT = 0x01;
      _BIC_SR(GIE);
      while (1)
        ;
#endif
#endif
    }
  }
}

static immortal_thread_t threads[1];

int main(int argc, char *argv[]) {
#ifdef IMMORTALITY_PORT_LINUX
  im_port_linux_init(argc, argv);
#endif

#ifdef __MSP430__
  WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
  PM5CTL0 &= ~LOCKLPM5; // Disable the GPIO power-on default high-impedance mode
#if defined(PORT_CONTROL)
  P3DIR = 0xFF;
#else
  P1OUT = 0x00;
  P1DIR = 0x03;
#endif
#endif
  imt_thread_init(&threads[0], dnn_main, NULL);
  imt_run(threads, 1, false);
  return 0;
}

#ifdef PF_SIMULATION

immortalc_fn_ignore void Timer_A1_set() {
  TA1CCR0 = timer_count;        // max 65535
  TA1CTL = TASSEL__ACLK + MC_1; // set the max period for 16bit timer
                                // operation
  TA1CCTL0 = CCIE;              // enable compare reg 0
  //    _BIS_SR( GIE); //ENABLE GLOBAL INTERRRUPTS
}

immortalc_fn_ignore void RebootHandler() {
  Timer_A1_set(); // Power failure simulation
}

// Timer A0 interrupt service routine
void __attribute__((interrupt(TIMER1_A0_VECTOR))) TimerA1(void) {
  if (count_t == 30)
    count_t = 0;
  timer_count = timer_array[count_t];
  count_t++;
#ifdef PORT_CONTROL
  P3OUT ^= 0x01;
  P3OUT ^= 0x01;
#endif
  Reset();
}

#endif
