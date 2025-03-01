/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <cmath>

#include <executorch/kernels/portable/cpu/util/functional_util.h>
#include <executorch/runtime/kernel/kernel_includes.h>
#include <executorch/runtime/platform/assert.h>

namespace torch {
namespace executor {
namespace native {

using Tensor = exec_aten::Tensor;
using ScalarType = exec_aten::ScalarType;

Tensor& relu_out(RuntimeContext& ctx, const Tensor& in, Tensor& out) {
  (void)ctx;

  Error err = resize_tensor(out, in.sizes());
  ET_CHECK_MSG(err == Error::Ok, "Could not resize output");

  ET_CHECK_SAME_SHAPE_AND_DTYPE2(in, out);

  ET_SWITCH_REAL_TYPES(in.scalar_type(), ctx, "relu.out", CTYPE, [&]() {
    apply_unary_map_fn(
        [](const CTYPE val_in) {
          return (std::isnan(val_in) || val_in >= CTYPE(0)) ? val_in : CTYPE(0);
        },
        in.const_data_ptr<CTYPE>(),
        out.mutable_data_ptr<CTYPE>(),
        in.numel());
  });

  return out;
}

} // namespace native
} // namespace executor
} // namespace torch
