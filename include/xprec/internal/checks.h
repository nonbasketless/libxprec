/* Implementations.
 *
 * DO NOT INCLUDE THIS FILE DIRECTLY: Include ddouble.h instead.
 *
 * Copyright (C) 2023 Markus Wallerberger and others
 * SPDX-License-Identifier: MIT
 */
#include "../ddouble.h"

inline bool operator==(DDouble x, DDouble y)
{
    return x.hi() == y.hi() && x.lo() == y.lo();
}

inline bool operator!=(DDouble x, DDouble y)
{
    return x.hi() != y.hi() || x.lo() != y.lo();
}

inline bool operator<=(DDouble x, DDouble y)
{
    return x.hi() < y.hi() || (x.hi() == y.hi() && x.lo() <= y.lo());
}

inline bool operator<(DDouble x, DDouble y)
{
    return x.hi() < y.hi() || (x.hi() == y.hi() && x.lo() < y.lo());
}

inline bool operator>=(DDouble x, DDouble y)
{
    return x.hi() > y.hi() || (x.hi() == y.hi() && x.lo() >= y.lo());
}

inline bool operator>(DDouble x, DDouble y)
{
    return x.hi() > y.hi() || (x.hi() == y.hi() && x.lo() > y.lo());
}

inline bool isfinite(DDouble x) { return std::isfinite(x.hi()); }

inline bool isinf(DDouble x) { return std::isinf(x.hi()); }

inline bool isnan(DDouble x) { return std::isnan(x.hi()); }

inline bool isnormal(DDouble x)
{
    // Here, we have to check the lo part too, since that is where the
    // denormalization shows up
    return std::isnormal(x.hi()) && std::isnormal(x.lo());
}

inline bool iszero(DDouble x) { return x.hi() == 0; }

inline int fpclassify(DDouble x)
{
    // This also works with zero, since that can be determined from the
    // hi part alone
    return std::fpclassify(x.hi());
}
