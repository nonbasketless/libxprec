/* Tests
 *
 * Copyright (C) 2023 Markus Wallerberger and others
 * SPDX-License-Identifier: MIT
 */
#include "xprec/ddouble.h"
#include <catch2/catch_test_macros.hpp>

using xprec::DDouble;

TEST_CASE("int", "[convert]")
{
    // XXX BROKEN
    // int64_t large = (1UL << 63) - 1;
    // REQUIRE(DDouble(-large).as<int64_t>() == -large);

    REQUIRE(DDouble(1.0f) + DDouble(3) == 4.0);
}
