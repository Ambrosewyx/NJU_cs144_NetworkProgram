### 1. Program Structure and Design
1. `webget`程序：

   ​	在`webget`程序中，只需建立TCP连接，并将`http`报文内容写入，并不断接收并输出内容直到`EOF`即可。

2. An in-memory reliable byte stream程序：
	
	- 首先要确定的是buffer的数据类型。本人首先考虑的是`string`，但对于`string`类型，`pop`操作的复杂度为O(n)，效率较低，而用`queue`来作为`buffer`，`pop`操作的复杂度为O(1)，因此本人最终选择了`queue`作为`buffer`的数据类型。
	- 除此之外，`bytestream`类还要求记录剩余空间(`available_capacity`)和占用空间(`byte_buffered`)，而可通过总容量(`capacity`)与二者之一作差得到另一个，因此只需要记录一个变量即可。由于在定义类时，占用空间已知为0方便初始化，而剩余空间需要知道总容量(`capacity`)，因此最终选择记录占用空间(`bytes_buffered_num`)。
	- `Read::pop(uint64_t len)`的设计：由于本人使用`queue`作为`buffer`的数据类型，因此当删除字符与对列首元素`queue.front()`长度不一样时，不能完全删除首元素，因此本人使用额外的变量`removedBytes_`来记录已经被删掉的字符，在`pop()`时需要综合考虑`len`与首元素长度`queue.front()-removedBytes_`之间的大小关系来删除元素。

### 2. Implementation Challenges

1. 在`pop()`函数中，需要比较`len`与首元素剩余长度`size`的大小关系，忽略了`len==size`的情况，导致测试样例`byte_stream_basics.cc`fail，通过clion的调试功能发现并解决了bug。
1. 在`cmake --build build --target check0`运行测试程序时，总会报`AddressSanitizer:DEADLYSIGNAL`错误，在qq群的FAQ文件中找到了解决方法：终端输入`echo 0 | sudo tee /proc/sys/kernel/randomize_va_space`，禁止linux地址空间布局随机化ASLR。

### 3. Remaining Bugs

1. 不太清楚的点：不清楚`peek()`函数的功能是提前看多少个字符，在`return`的时候，返回`string_view {buffer_.front()}.substr(removedBytes)`可以pass，但返回`string_view {buffer_.front().substr(removedBytes_)}`时，`Address Sanitizer`报`stack-use-after-return`错误，可能原因与`string_view`有关，需要进一步研究。

### 4. Experimental results and performance

1. 手动获取报文

   ![image-20240927191614175](/home/wangyuxi/.config/Typora/typora-user-images/image-20240927191614175.png)

2. 测试截图：	![image-20240927190010124](/home/wangyuxi/.config/Typora/typora-user-images/image-20240927190010124.png)
