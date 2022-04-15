/**
 * Test case: multi instance function
 */
#include "immortality.h"

// annotation on the declaration
_immortal_function(foo) /*immortalc_fn_max_instances*/ /*(3)*/;
// the definition should be treated as multi-instance function
_immortal_function_metadata_def(, foo, 3);
_immortal_function(foo) {
  _begin_multi(foo, _id);

  _end_multi(foo);
}

// annotation on the definition
_immortal_function_metadata_def(, test, 3);
_immortal_function(test) {
  _begin_multi(test, _id);
  _def int a[3];
  _WR((a[_id]), 0);
  _WR_SELF(int, (a[_id]), (a[_id]) + 1);

  _def int aa[3];
  _WR((aa[_id]), 0);
  _WR_SELF(int, (aa[_id]), (aa[_id]) + 1);

  _def int aaaaa[3];
  _WR((aaaaa[_id]), 0);
  _WR_SELF(int, (aaaaa[_id]), (aaaaa[_id]) + 1);
  _end_multi(test);
}

