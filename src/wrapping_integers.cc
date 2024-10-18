#include "wrapping_integers.hh"
// #include <cmath>
using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return Wrap32 { static_cast<uint32_t>( zero_point.raw_value_ + n ) };
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  int32_t diff = this->raw_value_ -  wrap(checkpoint, zero_point).raw_value_;
  int64_t res = checkpoint + diff;
  if(res<0)
    res += 1ul<<32;
  return res;
}
