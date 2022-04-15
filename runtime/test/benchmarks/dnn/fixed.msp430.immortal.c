#include "immortality.h"

#include "includes/fixedlib/fixed.h"
#include <stdint.h>

fixed f_sqrt(fixed a) {
  fixed tmp = f_mul((fixed)(0.5 * (1 << 5)), a) /*F_MUL*/ /*(F_LIT(0.5), a)*/;
#ifdef CONFIG_FIXED_PRECISE
  /*for (uint16_t i = 0; i < 8; i++) {*/
#else
  for (uint16_t i = 0; i < 4; i++) {
#endif
  tmp = f_mul(
      (fixed)(0.5 * (1 << 5)),
      tmp +
          f_div(a, tmp)) /*F_MUL*/ /*(F_LIT(0.5), F_ADD(tmp, F_DIV(a, tmp)))*/;
  }
  return tmp;
}

fixed f_cos(fixed a) {
  fixed tmp = a;
  // Scale
  if ((fixed)(6.2831853 * (1 << 5)) < tmp /*F_LT*/ /*(F_TWO_PI, tmp)*/) {
    fixed close =
        f_mul((fixed)(6.2831853 * (1 << 5)),
              -f_round(f_div(tmp, (fixed)(6.2831853 * (1 << 5))))) /*F_MUL*/
        /*(F_TWO_PI, -F_ROUND(F_DIV(tmp, F_TWO_PI)))*/;
    tmp = tmp + close /*F_ADD*/ /*(tmp, close)*/;
  } else if (tmp <
             -(fixed)(6.2831853 * (1 << 5)) /*F_LT*/ /*(tmp, -F_TWO_PI)*/) {
    fixed close =
        f_mul((fixed)(6.2831853 * (1 << 5)),
              f_round(f_div(-tmp, (fixed)(6.2831853 * (1 << 5))))) /*F_MUL*/
        /*(F_TWO_PI, F_ROUND(F_DIV(-tmp, F_TWO_PI)))*/;
    tmp = tmp + close /*F_ADD*/ /*(tmp, close)*/;
  }

  // Center around -F_PI and F_PI
  if (tmp < -(fixed)(3.1415926 * (1 << 5)) /*F_LT*/ /*(tmp, -F_PI)*/) {
    tmp = (fixed)(6.2831853 * (1 << 5)) + tmp /*F_ADD*/ /*(F_TWO_PI, tmp)*/;
  } else if ((fixed)(3.1415926 * (1 << 5)) < tmp /*F_LT*/ /*(F_PI, tmp)*/) {
    tmp = -(fixed)(6.2831853 * (1 << 5)) + tmp /*F_ADD*/ /*(-F_TWO_PI, tmp)*/;
  }

  // Shift
  tmp = tmp + (fixed)(1.5707963 * (1 << 5)) /*F_ADD*/ /*(tmp, F_HALF_PI)*/;
  if ((fixed)(3.1415926 * (1 << 5)) < tmp /*F_LT*/ /*(F_PI, tmp)*/) {
    tmp = -(fixed)(6.2831853 * (1 << 5)) + tmp /*F_ADD*/ /*(-F_TWO_PI, tmp)*/;
  }

  // Apply cos/sin
  fixed first_term = f_mul((fixed)(1.27323954 * (1 << 5)),
                           tmp) /*F_MUL*/ /*(F_LIT(1.27323954), tmp)*/;
  fixed second_term =
      f_mul(f_mul((fixed)(0.405284735 * (1 << 5)), tmp),
            tmp) /*F_MUL*/ /*(F_MUL(F_LIT(0.405284735), tmp), tmp)*/;
  if (tmp < (fixed)(0 * (1 << 5)) /*F_LT*/ /*(tmp, F_LIT(0))*/) {
    tmp = first_term + second_term;
#ifdef CONFIG_FIXED_PRECISE
    /*if (F_LT(tmp, F_LIT(0))) {*/
    /*return F_ADD(F_MUL(F_LIT(0.225), F_ADD(F_MUL(tmp, -tmp), -tmp)), tmp);*/
    /*}*/
    /*return F_ADD(F_MUL(F_LIT(0.225), F_ADD(F_MUL(tmp, tmp), -tmp)), tmp);*/
#else
    return tmp;
#endif
  }
  tmp = first_term - second_term;
#ifdef CONFIG_FIXED_PRECISE
  /*if (F_LT(tmp, F_LIT(0))) {*/
  /*return F_ADD(F_MUL(F_LIT(0.225), F_ADD(F_MUL(tmp, -tmp), -tmp)), tmp);*/
  /*}*/
  /*return F_ADD(F_MUL(F_LIT(0.225), F_ADD(F_MUL(tmp, tmp), -tmp)), tmp);*/
#else
  return tmp;
#endif
}

fixed f_sin(fixed a) {
  fixed tmp = a;
  // Scale
  if ((fixed)(6.2831853 * (1 << 5)) < tmp /*F_LT*/ /*(F_TWO_PI, tmp)*/) {
    fixed close =
        f_mul((fixed)(6.2831853 * (1 << 5)),
              -f_round(f_div(tmp, (fixed)(6.2831853 * (1 << 5))))) /*F_MUL*/
        /*(F_TWO_PI, -F_ROUND(F_DIV(tmp, F_TWO_PI)))*/;
    tmp = tmp + close /*F_ADD*/ /*(tmp, close)*/;
  } else if (tmp <
             -(fixed)(6.2831853 * (1 << 5)) /*F_LT*/ /*(tmp, -F_TWO_PI)*/) {
    fixed close =
        f_mul((fixed)(6.2831853 * (1 << 5)),
              f_round(f_div(-tmp, (fixed)(6.2831853 * (1 << 5))))) /*F_MUL*/
        /*(F_TWO_PI, F_ROUND(F_DIV(-tmp, F_TWO_PI)))*/;
    tmp = tmp + close /*F_ADD*/ /*(tmp, close)*/;
  }

  // Center around -F_PI and F_PI
  if (tmp < -(fixed)(3.1415926 * (1 << 5)) /*F_LT*/ /*(tmp, -F_PI)*/) {
    tmp = (fixed)(6.2831853 * (1 << 5)) + tmp /*F_ADD*/ /*(F_TWO_PI, tmp)*/;
  } else if ((fixed)(3.1415926 * (1 << 5)) < tmp /*F_LT*/ /*(F_PI, tmp)*/) {
    tmp = -(fixed)(6.2831853 * (1 << 5)) + tmp /*F_ADD*/ /*(-F_TWO_PI, tmp)*/;
  }

  // Apply cos/sin
  fixed first_term = f_mul((fixed)(1.27323954 * (1 << 5)),
                           tmp) /*F_MUL*/ /*(F_LIT(1.27323954), tmp)*/;
  fixed second_term =
      f_mul(f_mul((fixed)(0.405284735 * (1 << 5)), tmp),
            tmp) /*F_MUL*/ /*(F_MUL(F_LIT(0.405284735), tmp), tmp)*/;
  if (tmp < (fixed)(0 * (1 << 5)) /*F_LT*/ /*(tmp, F_LIT(0))*/) {
    tmp = first_term + second_term;
#ifdef CONFIG_FIXED_PRECISE
    /*if (F_LT(tmp, F_LIT(0))) {*/
    /*return F_ADD(F_MUL(F_LIT(0.225), F_ADD(F_MUL(tmp, -tmp), -tmp)), tmp);*/
    /*}*/
    /*return F_ADD(F_MUL(F_LIT(0.225), F_ADD(F_MUL(tmp, tmp), -tmp)), tmp);*/
#else
    return tmp;
#endif
  }
  tmp = first_term - second_term;
#ifdef CONFIG_FIXED_PRECISE
  /*if (F_LT(tmp, F_LIT(0))) {*/
  /*return F_ADD(F_MUL(F_LIT(0.225), F_ADD(F_MUL(tmp, -tmp), -tmp)), tmp);*/
  /*}*/
  /*return F_ADD(F_MUL(F_LIT(0.225), F_ADD(F_MUL(tmp, tmp), -tmp)), tmp);*/
#else
  return tmp;
#endif
}
