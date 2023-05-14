#pragma once

#include <cfloat>
#include <type_traits>

namespace YaGE {
namespace Numbers {

template <typename Floating>
static constexpr const Floating Pi = Floating(3.1415926535897932384626433832795);

template <typename Floating>
static constexpr const Floating SqrtPi = Floating(1.7724538509055160272981674833411);

template <typename Floating>
static constexpr const Floating InvPi = Floating(0.31830988618379067153776752674503);

template <typename Floating>
static constexpr const Floating InvSqrtPi = Floating(0.56418958354775628694807945156077);

template <typename Floating>
static constexpr const Floating E = Floating(2.7182818284590452353602874713527);

template <typename Floating>
static constexpr const Floating Log2E = Floating(1.4426950408889634073599246810019);

template <typename Floating>
static constexpr const Floating Log10E = Floating(0.43429448190325182765112891891661);

template <typename Floating>
static constexpr const Floating Ln2 = Floating(0.69314718055994530941723212145818);

template <typename Floating>
static constexpr const Floating Ln10 = Floating(2.3025850929940456840179914546844);

template <typename Floating>
static constexpr const Floating Sqrt2 = Floating(1.4142135623730950488016887242097);

template <typename Floating>
static constexpr const Floating InvSqrt2 = Floating(0.70710678118654752440084436210485);

template <typename Floating>
static constexpr const Floating Sqrt3 = Floating(1.7320508075688772935274463415059);

template <typename Floating>
static constexpr const Floating InvSqrt3 = Floating(0.57735026918962576450914878050196);

template <typename Floating>
static const typename std::enable_if<false, Floating>::type Epsilon;

template <>
static constexpr const float Epsilon<float> = FLT_EPSILON;

template <>
static constexpr const double Epsilon<double> = DBL_EPSILON;

template <>
static constexpr const long double Epsilon<long double> = LDBL_EPSILON;

} // namespace Numbers
} // namespace YaGE
