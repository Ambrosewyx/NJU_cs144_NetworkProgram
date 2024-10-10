#include "reassembler.hh"
using namespace std;

//  [----]      [-----------------] [ ---   ---   ---  ]  [............]
//  poped       in the bytestream   in the reassembler   unacceptable
#define first_unassembled_index output_.writer().bytes_pushed()
// #define available_capacity output_.writer().available_capacity()
#define first_unacceptable_index output_.writer().bytes_pushed()+output_.writer().available_capacity()

void Reassembler::closeCheck()
{
  if(endingIdx_ == first_unassembled_index)
    output_.writer().close();
}


void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  if(is_last_substring)
    endingIdx_ = first_index + data.length();
  closeCheck();
  // 无用片段
  if(data.length()==0 || first_index + data.length()<first_unassembled_index || first_index>=first_unacceptable_index) {
    return;
  }
  // 找到data插入Reassemble_buf_的index
  data = data.substr(0,min(data.length(), first_unacceptable_index-first_index));
  pair<uint64_t, string> substring{first_index, data};
  uint64_t idx = 0;
  while(idx < Reassemble_buf_.size() && substring.first > Reassemble_buf_[idx].first)
    idx++;
  // 插入之后需要消除overlap
  if(idx < Reassemble_buf_.size() && substring.first == Reassemble_buf_[idx].first){
    if(substring.second.length() > Reassemble_buf_[idx].second.length()) {
      Reassemble_buf_.erase( Reassemble_buf_.begin()+idx);
      Reassemble_buf_.insert(Reassemble_buf_.begin()+idx, substring);
    }
  }else
    Reassemble_buf_.insert(Reassemble_buf_.begin()+idx, substring);
  // 开始写入ByteStream
  while(!Reassemble_buf_.empty() && Reassemble_buf_[0].first<=first_unassembled_index) {
    if(Reassemble_buf_[0].first+Reassemble_buf_[0].second.length()>first_unassembled_index){
      output_.writer().push( Reassemble_buf_[0].second.substr( first_unassembled_index-Reassemble_buf_[0].first));
    }
    Reassemble_buf_.erase(Reassemble_buf_.begin());
  }

  closeCheck();
}

uint64_t Reassembler::bytes_pending() const
{
  uint64_t sum = 0;
  for(auto& it: Reassemble_buf_)
    sum += it.second.length();
  return sum;
}

// 废弃code
// if(first_index <= first_unassembled_index) {
//   // 与bytestream完全重复:扔掉
//   if(first_index + data.length() <= first_unassembled_index) {
//     return;
//   }// 与bytestream部分重复:取剩余
//   uint64_t size = min(first_index+data.length()-first_unassembled_index,available_capacity );
//   output_.writer().push( data.substr( 0, size ) );
//
//   pair<uint64_t, string> substring = Reassemble_buf_[0];
//   while(substring.first + substring.second.length() > first_unassembled_index) {
//     ;
//   }

// 将有效片段放入Reassemble_buf_
// if(first_index<first_unassembled_index && first_index+data.length()>first_unassembled_index) {
//   uint64_t size = min(first_index+data.length()-first_unassembled_index,available_capacity );
//   substring = pair<uint64_t, string>{first_unassembled_index, data.substr(first_unassembled_index-first_index, size)};
// }else
//   substring = pair<uint64_t, string>{first_index, data};


