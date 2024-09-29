#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  return close_;
}

void Writer::push( string data )
{
  if ( available_capacity() == 0 || data.empty() )
    return;
  const uint64_t size = min( data.length(), available_capacity() );
  data = data.substr( 0, size );
  buffer_.emplace( move( data ) );

  bytes_buffered_num_ += size;
  bytes_pushed_num_ += size;
}

void Writer::close()
{
  close_ = true;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - bytes_buffered_num_;
}

uint64_t Writer::bytes_pushed() const
{
  return bytes_pushed_num_;
}

bool Reader::is_finished() const
{
  return close_ && bytes_buffered() == 0;
}

uint64_t Reader::bytes_popped() const
{
  return bytes_popped_num_;
}

string_view Reader::peek() const
{
  if ( buffer_.empty() )
    return {};
  return string_view { buffer_.front() }.substr( removedBytes_ );
  // return string_view{buffer_.front().substr( removedBytes_ )}会报错，可能和string_view的构造有关系？
}

void Reader::pop( uint64_t len )
{
  if ( buffer_.empty() || len == 0 )
    return;

  len = len > bytes_buffered_num_ ? bytes_buffered_num_ : len; // 最大情况：清空队列
  bytes_popped_num_ += len;
  bytes_buffered_num_ -= len;

  while ( len != 0 ) {
    const uint64_t size = buffer_.front().length() - removedBytes_;
    if ( len >= size ) {
      buffer_.pop();
      len -= size;
      removedBytes_ = 0;
    } else {
      removedBytes_ += len;
      len = 0;
    }
  }
}

uint64_t Reader::bytes_buffered() const
{
  return bytes_buffered_num_;
}
