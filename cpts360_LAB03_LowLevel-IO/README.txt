Lab 03: Low Level IO~~~~~~~~~~~~~~Raul Martinez

I plotted my data using R since it is what is ingraned in my brain after successfuly having passed and taken STATS 360.

The `raw_copy` program is a custom file that uses raw I/O system calls `read` and `write`. 
The program dynamically allocates a buffer based on a user-specified sizes, allowing for experimentation with different buffer sizes to evaluate their impact on performance. 
In this lab, we tested buffer sizes ranging from 1 byte to 16384 bytes and analyzed the time required to copy a 100 MB file.

The results showed that smaller buffer sizes, such as 1 and 2 bytes, resulted in significantly longer execution times likely due to the excessive number of `read` and `write` system calls. 
For example, using a 1-byte buffer took over 200 seconds to complete the file copy. The high system time during these tests indicates the kernel's overhead in handling frequent I/O operations. 
As the buffer size increased, the performance improved drastically. With a buffer size of 64 bytes, the real time dropped to 2.86 seconds, and by 2048 bytes, the time was reduced to 0.10 seconds. 
These improvements reflect the reduced frequency of system calls as larger buffers allowed more data to be processed per operation.

Beyond 4096 bytes, the performance gains slowed down. For buffer sizes of 4096 bytes and above, the real time plateaued at around 0.05 seconds. 
This demonstrates that increasing the buffer size beyond a certain point provides minimal additional benefit. For this experiment, a buffer size of 4096 bytes seems
to be teh best choice, balancing performance with memory usage on this ARM64 based Mac.