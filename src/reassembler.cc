#include "reassembler.hh"
using namespace std;

//  [----]      [-----------------] [ ---   ---   ---  ]  [............]
//  poped       in the bytestream   in the reassembler   unacceptable
#define first_unassembled_index output_.writer().bytes_pushed()
#define first_unacceptable_index output_.writer().bytes_pushed()+output_.writer().available_capacity()
#define bufStrEnd Reassemble_buf_[idx].first+Reassemble_buf_[idx].second.length()
#define dataStrEnd dataFirst+data.length()


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
  // 除去capcity外的字符串
  data = data.substr(0,min(data.length(), first_unacceptable_index-first_index));
  pair<uint64_t, string> substring{first_index, data};
  // 找到data插入Reassemble_buf_的index
  uint64_t idx = 0;
  uint64_t dataFirst = first_index;
  bool insertFlag = true;
  while(idx < Reassemble_buf_.size()) {
    if(dataFirst > bufStrEnd) // 2
      idx++;
    else if(dataStrEnd<Reassemble_buf_[idx].first) // 1
      break;
    else if(dataFirst < Reassemble_buf_[idx].first && dataStrEnd >= Reassemble_buf_[idx].first && dataStrEnd<bufStrEnd){ // 3
      data += Reassemble_buf_[idx].second.substr(dataStrEnd - Reassemble_buf_[idx].first);
      Reassemble_buf_.erase(Reassemble_buf_.begin()+idx);
    }else if(dataFirst > Reassemble_buf_[idx].first&& dataFirst <= bufStrEnd && dataStrEnd > bufStrEnd ) { // 4
      data  = Reassemble_buf_[idx].second.substr( 0, dataFirst-Reassemble_buf_[idx].first) + data;
      dataFirst = Reassemble_buf_[idx].first;
      Reassemble_buf_.erase(Reassemble_buf_.begin()+idx);
    }else if(dataFirst <=Reassemble_buf_[idx].first && dataStrEnd>= bufStrEnd) { // 6
      Reassemble_buf_.erase(Reassemble_buf_.begin() + idx);
    }
    else { // 5
      insertFlag = false;
      break;
    }
  }
  if(insertFlag) {
    substring = pair<uint64_t, string>(dataFirst, data);
    Reassemble_buf_.insert(Reassemble_buf_.begin()+idx, substring);
  }

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

