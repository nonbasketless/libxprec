/* Tests
 *
 * Copyright (C) 2023 Markus Wallerberger and others
 * SPDX-License-Identifier: MIT
 */
#include <numeric>
#include <vector>
#include <catch2/catch_test_macros.hpp>
#include "xprec/ddouble.h"
#include "catch2-addons.h"


TEST_CASE("leg-weights", "[gauss]")
{
    std::vector<DDouble> x(5), w(5);
    gauss_legendre(5, x.data(), w.data());

    REQUIRE_THAT(std::accumulate(w.begin(), w.end(), DDouble(0.0)),
                 WithinRel(DDouble(2.0), 5e-32));
}

TEST_CASE("leg-cmp-7", "[gauss]")
{
    const static DDouble x_ref[7] = {
        {-0.9491079123427585, -3.82579658786657e-17},
        {-0.7415311855993945, 2.0220134774069897e-17},
        {-0.4058451513773972, 1.72492754475471e-17},
        {0.0, 0.0},
        {0.4058451513773972, -1.72492754475471e-17},
        {0.7415311855993945, -2.0220134774069897e-17},
        {0.9491079123427585, 3.82579658786657e-17}
        };
    const static DDouble w_ref[7] = {
        {0.1294849661688697, -9.625448970284404e-18},
        {0.27970539148927664, 2.3267180221717138e-17},
        {0.3818300505051189, 2.1862747923824822e-17},
        {0.4179591836734694, -1.5497807119257288e-17},
        {0.3818300505051189, 2.1862747923824822e-17},
        {0.27970539148927664, 2.3267180221717138e-17},
        {0.1294849661688697, -9.625448970284404e-18}
        };

    std::vector<DDouble> x(7), w(7);
    gauss_legendre(7, x.data(), w.data());
    for (int i = 0; i < 7; ++i) {
        REQUIRE_THAT(x[i], WithinAbs(x_ref[i], 5e-32));
        REQUIRE_THAT(w[i], WithinAbs(w_ref[i], 0.4 * 5e-32));
    }
}

TEST_CASE("leg-cmp-16", "[gauss]")
{
    const static DDouble x_ref[16] = {
        {-0.9894009349916499, 5.914095566469922e-18},
        {-0.9445750230732326, 2.4190068142444825e-17},
        {-0.8656312023878318, 1.1315677979849837e-17},
        {-0.755404408355003, -3.5241085894430354e-17},
        {-0.6178762444026438, 2.2123521973463665e-17},
        {-0.45801677765722737, -1.6662404170959257e-17},
        {-0.2816035507792589, 2.1958791252592132e-18},
        {-0.09501250983763744, 3.275947755433097e-19},
        {0.09501250983763744, -3.275947755433097e-19},
        {0.2816035507792589, -2.1958791252592132e-18},
        {0.45801677765722737, 1.6662404170959257e-17},
        {0.6178762444026438, -2.2123521973463665e-17},
        {0.755404408355003, 3.5241085894430354e-17},
        {0.8656312023878318, -1.1315677979849837e-17},
        {0.9445750230732326, -2.4190068142444825e-17},
        {0.9894009349916499, -5.914095566469922e-18}
        };
    const static DDouble w_ref[16] = {
        {0.027152459411754096, -1.56154670271636e-18},
        {0.062253523938647894, -7.690264522605704e-19},
        {0.09515851168249279, -8.783003597087393e-19},
        {0.12462897125553388, -4.841529802320495e-18},
        {0.14959598881657674, -3.887619883741701e-18},
        {0.16915651939500254, 2.323299329564479e-18},
        {0.18260341504492358, 5.090226510905207e-18},
        {0.1894506104550685, -5.883843495582664e-18},
        {0.1894506104550685, -5.883843495582664e-18},
        {0.18260341504492358, 5.090226510905207e-18},
        {0.16915651939500254, 2.323299329564479e-18},
        {0.14959598881657674, -3.887619883741701e-18},
        {0.12462897125553388, -4.841529802320495e-18},
        {0.09515851168249279, -8.783003597087393e-19},
        {0.062253523938647894, -7.690264522605704e-19},
        {0.027152459411754096, -1.56154670271636e-18}
        };

    std::vector<DDouble> x(16), w(16);
    gauss_legendre(16, x.data(), w.data());
    for (int i = 0; i < 16; ++i) {
        REQUIRE_THAT(x[i], WithinAbs(x_ref[i], 5e-32));
        REQUIRE_THAT(w[i], WithinAbs(w_ref[i], 0.3 * 5e-32));
    }
}
