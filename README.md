# kinetiscomm

Undergraduate student project for Microprocessor Technology semester 2.
Data collection using UART communication between eval. boards: NXP kl46z and kl25z with Kinetis MC, using C language for Cortex-M0+ core.

Files specified for each MC allows for configuration of tree-type connection net of microcontrollers.
One of them needs to be connected to PC using USB(UART0 of MC), the rest needs to be connected using only UART1 and UART2 connected to pins PTE 0, 1, 16 and 17 on kl46z and pins PTC 3, 4 and PTD 2 and 3.
Flow control and stream direction configuration is being managed from main.c files.
The speed of transmission can be also increased by setting bigger baudrate of UART1 and UART2 in uart.c files.
Adding new sensor or file using our standard needs to be configured, just like cooperation between light sensor and PIT timer was implemented in our example. Each sensor needs it's own tag(more info. about encoding it can be found in .cpp files).

The PC communication part needs to be built and compiled using Cmake and MS Visual Studio.
It requires user to install Pemicro drivers to make USB port an serial COM port, which number must be specified in main.cpp file.

Other instructions are provided in .c, .cpp and .h files' comments.

Here is short video about our project in Polish language:
[![Projekt TMP](https://img.techpowerup.org/200124/screen.jpg)](https://www.youtube.com/watch?v=6QYDE7_mTg8&feature=youtu.be&fbclid=IwAR2bA27KMbAmxoTa_OSUUnOUhwgwgr7ZWWbiY911qVBp0MrfLn30jAh0xqw)
