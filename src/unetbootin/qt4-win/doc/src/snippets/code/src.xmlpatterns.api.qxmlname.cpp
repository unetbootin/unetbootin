// Fills the bits from begin to end with 1s and leaves the rest as 0.

template<typename IntegralT>
inline IntegralT bitmask(IntegralT begin, IntegralT end)
{
   IntegralT filled_bits = (1 << (end - begin + 1)) - 1;
   return filled_bits << begin;
}
