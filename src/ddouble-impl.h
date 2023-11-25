/* Implementations.
 *
 * DO NOT INCLUDE THIS FILE DIRECTLY: Include ddouble.h instead.
 *
 * Most of the basic numerical algorithms are directly lifted from:
 * M. Joldes, et al., ACM Trans. Math. Softw. 44, 1-27 (2018)
 *
 * Copyright (C) 2022 Markus Wallerberger and others
 * SPDX-License-Identifier: MIT
 */
#pragma once
#include "ddouble.h"
#include <cassert>

#include <iostream>

/**
 * Return true if x is greater or equal in magnitude as y.
 *
 * Return true if the xponent of y does not exceed the exponent of x.  NaN
 * and Inf are considered maximum magnitude, 0 is considered minimum magnitude.
 */
inline bool greater_in_magnitude(double x, double y)
{
    static_assert(std::numeric_limits<double>::is_iec559);
    union {
        double number;
        uint64_t pattern;
    } x_u = {x}, y_u = {y};

    // Shift out sign bit
    return (x_u.pattern << 1) >= (y_u.pattern << 1);
}

inline bool greater_in_magnitude(DDouble x, DDouble y)
{
    return greater_in_magnitude(x.hi(), y.hi());
}

inline bool greater_in_magnitude(DDouble x, double y)
{
    return greater_in_magnitude(x.hi(), y);
}

inline bool greater_in_magnitude(double x, DDouble y)
{
    return greater_in_magnitude(x, y.hi());
}


// -------------------------------------------------------------------------
// PowerOfTwo

inline PowerOfTwo operator*(PowerOfTwo a, PowerOfTwo b)
{
    return PowerOfTwo(a._x * b._x);
}

inline PowerOfTwo operator/(PowerOfTwo a, PowerOfTwo b)
{
    return PowerOfTwo(a._x / b._x);
}

inline DDouble ExDouble::add_small(double b) const
{
    // M. Joldes, et al., ACM Trans. Math. Softw. 44, 1-27 (2018)
    // Algorithm 1: cost 3 flops
    //assert(_x == 0 || greater_in_magnitude(_x, b));
    double s = _x + b;
    double z = s - _x;
    double t = b - z;
    return DDouble(s, t);
}

// -------------------------------------------------------------------------
// ExDouble

inline DDouble operator+(ExDouble a, ExDouble b)
{
    // Algorithm 2: cost 6 flops
    double s = (double) a + (double) b;
    double aprime = s - (double) b;
    double bprime = s - aprime;
    double delta_a = (double) a - aprime;
    double delta_b = (double) b - bprime;
    double t = delta_a + delta_b;
    return DDouble(s, t);
}

// We must place this here rather than in-place in the class, because
// the type DDouble is still incomplete

inline DDouble operator+(double a, ExDouble b) { return ExDouble(a) + b; }
inline DDouble operator+(ExDouble a, double b) { return a + ExDouble(b); }

inline DDouble operator-(ExDouble a, ExDouble b) { return a + (-b); }
inline DDouble operator-(ExDouble a, double b) { return a + (-b); }
inline DDouble operator-(double a, ExDouble b) { return a + (-b); }

inline DDouble operator*(ExDouble a, ExDouble b)
{
    // Algorithm 3: cost 2 flops
    double pi = (double) a * (double) b;
    double rho = std::fma((double) a, (double) b, -pi);
    return DDouble(pi, rho);
}

inline DDouble operator*(double a, ExDouble b) { return ExDouble(a) * b; }
inline DDouble operator*(ExDouble a, double b) { return a * ExDouble(b); }

inline DDouble reciprocal(ExDouble y)
{
    // Part of Algorithm 18 for y_lo = 0
    double th = 1.0 / (double) y;
    double rh = std::fma(-(double) y, th, 1.0);
    DDouble delta = ExDouble(rh) * th;
    return delta + th;
}

inline DDouble operator/(ExDouble a, ExDouble b)
{
    // Algorithm 18 for this special case
    return reciprocal(a) * (double)b;
}

inline DDouble operator/(double a, ExDouble b) { return ExDouble(a) / b; }
inline DDouble operator/(ExDouble a, double b) { return a / ExDouble(b); }

// -------------------------------------------------------------------------
// DDouble arithmetic

inline DDouble operator+(DDouble x, double y)
{
    // Algorithm 4: cost 10 flops, error 2 u^2
    DDouble s = ExDouble(x._hi) + y;
    double v = x._lo + s._lo;
    return ExDouble(s._hi).add_small(v);
}

inline DDouble operator+(DDouble x, DDouble y)
{
    // Algorithm 6: cost 20 flops, error 3 u^2 + 13 u^3
    DDouble s = ExDouble(x._hi) + y._hi;
    DDouble t = ExDouble(x._lo) + y._lo;
    double c = s._lo + t._hi;
    DDouble v = ExDouble(s._hi).add_small(c);
    double w = t._lo + v._lo;
    return ExDouble(v._hi).add_small(w);
}

inline DDouble DDouble::add_small(double y)
{
    // Algorithm 4 modified: cost 7 flops, error 2 u^2
    DDouble s = ExDouble(_hi).add_small(y);
    double v = _lo + s._lo;
    return ExDouble(s._hi).add_small(v);
}

inline DDouble DDouble::add_small(DDouble y)
{
    // Algorithm 6: cost 17 flops, error 3 u^2 + 13 u^3
    DDouble s = ExDouble(_hi).add_small(y._hi);
    DDouble t = ExDouble(_lo) + y._lo;
    double c = s._lo + t._hi;
    DDouble v = ExDouble(s._hi).add_small(c);
    double w = t._lo + v._lo;
    return ExDouble(v._hi).add_small(w);
}

inline DDouble operator*(DDouble x, double y)
{
    // Algorithm 9: cost 6 flops, error 2 u^2
    DDouble c = ExDouble(x._hi) * y;
    double cl3 = std::fma(x._lo, y, c._lo);
    return ExDouble(c.hi()).add_small(cl3);
}

inline DDouble operator*(DDouble x, DDouble y)
{
    // Algorithm 12: cost 9 flops, error 4 u^2 (corrected)
    DDouble c = ExDouble(x._hi) * y._hi;
    double tl0 = x._lo * y._lo;
    double tl1 = std::fma(x._hi, y._lo, tl0);
    double cl2 = std::fma(x._lo, y._hi, tl1);
    double cl3 = c._lo + cl2;
    return ExDouble(c._hi).add_small(cl3);
}

inline DDouble operator/(DDouble x, double y)
{
    // Algorithm 15: cost 10 flops, error 3 u^2
    ExDouble th = x._hi / y;
    DDouble pi = th * y;
    double delta_h = x._hi - pi._hi;
    double delta_tee = delta_h - pi._lo;
    double delta = delta_tee + x._lo;
    double tl = delta / y;
    return th.add_small(tl);
}

inline DDouble reciprocal(DDouble y)
{
    // Part of Algorithm 18: cost 22 flops, error 2.3 u^2
    double th = 1.0 / y._hi;
    double rh = std::fma(-y._hi, th, 1.0);
    double rl = -y._lo * th;
    DDouble e = ExDouble(rh).add_small(rl);
    DDouble delta = e * th;
    return delta + th;
}

inline DDouble operator/(DDouble x, DDouble y)
{
    // Algorithm 18: cost 31 flops, error 10 u^2 (6 u^2 obs.)
    return x * reciprocal(y);
}

inline DDouble operator/(double x, DDouble y)
{
    // Algorithm 18: cost 28 flops
    return x * reciprocal(y);
}

inline DDouble operator*(DDouble x, PowerOfTwo y)
{
    return DDouble(x._hi * (double)y, x._lo * (double)y);
}

inline DDouble operator/(DDouble x, PowerOfTwo y)
{
    return DDouble(x._hi / (double)y, x._lo / (double)y);
}

// -------------------------------------------------------------------------
// DDouble relational operators

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

// -------------------------------------------------------------------------
// DDouble basic functions

inline void swap(DDouble &x, DDouble &y)
{
    std::swap(x._hi, y._hi);
    std::swap(x._lo, y._lo);
}

inline DDouble ldexp(DDouble a, int n)
{
    return DDouble(std::ldexp(a.hi(), n), std::ldexp(a.lo(), n));
}

inline bool signbit(DDouble a)
{
    return std::signbit(a.hi());
}

inline DDouble copysign(DDouble mag, double sgn)
{
    // The sign is determined by the hi part, however, the sign of hi and lo
    // need not be the same, so we cannot merely broadcast copysign to both
    // parts.
    return signbit(mag) != std::signbit(sgn) ? -mag : mag;
}

inline DDouble copysign(DDouble mag, DDouble sgn)
{
    return copysign(mag, sgn.hi());
}

inline DDouble copysign(double mag, DDouble sgn)
{
    return DDouble(std::copysign(mag, sgn.hi()));
}

inline DDouble abs(DDouble x)
{
    return fabs(x);
}

inline DDouble fabs(DDouble x)
{
    return signbit(x) ? -x : x;
}

inline DDouble trunc(DDouble x)
{
    // Truncation simply involves truncating both parts
    return DDouble(trunc(x.hi()), trunc(x.lo()));
}

inline DDouble ceil(DDouble x)
{
    // If hi was not an integer, we already found our answer
    double hi = ceil(x.hi());
    if (hi != x.hi())
        return hi;

    // hi is an integer, so modify lo instead.  This may actually increase the
    // magnitude above the limit, so let's renormalize to be safe.
    double lo = ceil(x.lo());
    return ExDouble(x.hi()).add_small(lo);
}

inline DDouble floor(DDouble x)
{
    // If hi was not an integer, we already found our answer
    double hi = floor(x.hi());
    if (hi != x.hi())
        return hi;

    // hi is an integer, so modify lo instead.  This may actually increase the
    // magnitude above the limit, so let's renormalize to be safe.
    double lo = floor(x.lo());
    return ExDouble(x.hi()).add_small(lo);
}

inline DDouble round(DDouble x)
{
    // trunc is usually encoded with two instructions, so it makes sense
    // to use this as a building block.
    double nudge = copysign(0.5, x.hi());
    return trunc(x + nudge);
}

// -------------------------------------------------------------------------
// DDouble inquiry functions

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

// -------------------------------------------------------------------------
// Numeric limits

constexpr DDouble std::numeric_limits<DDouble>::min() noexcept
{
    // Whereas the maximum exponent is the same for double and DDouble,
    // Denormalization in the low part means that the min exponent for
    // normalized values is lower.
    return DDouble(_double::min() / _double::epsilon());
}

constexpr DDouble std::numeric_limits<DDouble>::max() noexcept
{
    return DDouble(_double::max(),
                    _double::max() * _double::epsilon() / _double::radix);
}

constexpr DDouble std::numeric_limits<DDouble>::lowest() noexcept
{
    return DDouble(_double::lowest(),
                    _double::lowest() / _double::epsilon() / _double::radix);
}

constexpr DDouble std::numeric_limits<DDouble>::epsilon() noexcept
{
    return DDouble(_double::epsilon() * _double::epsilon() / _double::radix);
}

constexpr DDouble std::numeric_limits<DDouble>::round_error() noexcept
{
    return DDouble(_double::round_error());
}

constexpr DDouble std::numeric_limits<DDouble>::infinity() noexcept
{
    return DDouble(_double::infinity(), _double::infinity());
}

constexpr DDouble std::numeric_limits<DDouble>::quiet_NaN() noexcept
{
    return DDouble(_double::quiet_NaN(), _double::quiet_NaN());
}

constexpr DDouble std::numeric_limits<DDouble>::signaling_NaN() noexcept
{
    return DDouble(_double::signaling_NaN(), _double::signaling_NaN());
}

constexpr DDouble std::numeric_limits<DDouble>::denorm_min() noexcept
{
    return DDouble(_double::denorm_min());
}
