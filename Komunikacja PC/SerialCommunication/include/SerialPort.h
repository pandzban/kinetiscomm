#define WAIT_TIME 2000
#define MAX_DATA_LENGTH 257

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>

class SerialPort
{
private:
	HANDLE handler;
	bool connected;
	COMSTAT status;
	DWORD errors;
public:
	SerialPort(char* portName);
	~SerialPort();

	int readSerialPort(char* buffer, unsigned int buf_size);
	int readSerialPort2(uint8_t* buffer, unsigned int buf_size);
	bool writeSerialPort3(uint8_t* buffer, unsigned int buf_size);
	int readSerialPortUint(uint8_t* buffer, unsigned int buf_size);
	bool readSerialPortFull(char* buffer, unsigned int buf_size);
	bool writeSerialPort(char* buffer, unsigned int buf_size);
	bool writeSerialPort2(char* buffer, unsigned int buf_size);
	bool writeSerialPortOne(uint8_t* buffer);
	bool writeSerialPortDirect(uint8_t word);
	bool isConnected();
	void closeSerial();
	bool WriteData(uint8_t buffer);
};
