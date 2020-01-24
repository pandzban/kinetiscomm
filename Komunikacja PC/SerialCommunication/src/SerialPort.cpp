#include "SerialPort.h"

SerialPort::SerialPort(char* portName)
{
	this->connected = false;

	this->handler = CreateFileA(static_cast<LPCSTR>(portName),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (this->handler == INVALID_HANDLE_VALUE) {
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			printf("ERROR: Handle was not attached, %s not available\n", portName);
		}
		else
		{
			printf("ERROR!!!");
		}
	}
	else {
		DCB dcbSerialParameters = { 0 };

		if (!GetCommState(this->handler, &dcbSerialParameters)) {
			printf("failed to get current serial parameters");
		}
		else {
			dcbSerialParameters.BaudRate = CBR_9600; //19200
			dcbSerialParameters.ByteSize = 8;
			dcbSerialParameters.StopBits = ONESTOPBIT;
			dcbSerialParameters.Parity = NOPARITY;
			dcbSerialParameters.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(handler, &dcbSerialParameters))
			{
				printf("ALERT: could not set Serial port parameters\n");
			}
			else {
				this->connected = true;
				PurgeComm(this->handler, PURGE_RXCLEAR | PURGE_TXCLEAR);
				Sleep(2000);
			}
		}
	}
}

SerialPort::~SerialPort()
{
	if (this->connected) {
		this->connected = false;
		CloseHandle(this->handler);
	}
}

int SerialPort::readSerialPort(char* buffer, unsigned int buf_size)
{
	DWORD bytesRead;
	unsigned int toRead = 0;

	ClearCommError(this->handler, &this->errors, &this->status);

	if (this->status.cbInQue > 0) {
		if (this->status.cbInQue > buf_size) {
			toRead = buf_size;
		}
		else toRead = this->status.cbInQue;
	}

	memset(buffer, 0, buf_size);

	if (ReadFile(this->handler, buffer, toRead, &bytesRead, NULL)) return bytesRead;
	return 0;
}

int SerialPort::readSerialPortUint(uint8_t* buffer, unsigned int buf_size)
{
	DWORD bytesRead;
	unsigned int toRead = 0;

	ClearCommError(this->handler, &this->errors, &this->status);

	if (this->status.cbInQue > 0) {
		if (this->status.cbInQue > buf_size) {
			toRead = buf_size;
		}
		else toRead = this->status.cbInQue;
	}

	memset(buffer, 0, buf_size);

	if (ReadFile(this->handler, buffer, toRead, &bytesRead, NULL)) return bytesRead;
	return 0;
}

int SerialPort::readSerialPort2(uint8_t* buffer, unsigned int buf_size)
{
	DWORD bytesRead;
	unsigned int toRead = 0;

	ClearCommError(this->handler, &this->errors, &this->status);

	if (this->status.cbInQue > 0) {
		if (this->status.cbInQue > buf_size) {
			toRead = buf_size;
		}
		else toRead = this->status.cbInQue;
	}

	memset(buffer, 0, buf_size);

	if (ReadFile(this->handler, buffer, toRead, &bytesRead, NULL)) return bytesRead;
	return 0;
}

bool SerialPort::readSerialPortFull(char* buffer, unsigned int buf_size) {
	while (buf_size > 0) {
		int hasRead = readSerialPort(buffer, buf_size);
		buffer += hasRead;
		buf_size -= hasRead; //nic
	}
		return true;
}


bool SerialPort::writeSerialPort(char* buffer, unsigned int buf_size)
{
	DWORD bytesSend;

	if (!WriteFile(this->handler, (void*)buffer, buf_size, &bytesSend, 0)) {
		ClearCommError(this->handler, &this->errors, &this->status);
		return false;
	}
	else return true;
}

bool SerialPort::writeSerialPort2(char* buffer, unsigned int buf_size)
{
	DWORD bytesSend;

	if (!WriteFile(this->handler, (void*)buffer, 1, &bytesSend, 0)) {
		ClearCommError(this->handler, &this->errors, &this->status);
		return false;
	}
	else return true;
}

bool SerialPort::writeSerialPort3(uint8_t* buffer, unsigned int buf_size)
{
	DWORD bytesSend;

	if (!WriteFile(this->handler, (void*)buffer, 1, &bytesSend, 0)) {
		ClearCommError(this->handler, &this->errors, &this->status);
		return false;
	}
	else return true;
}

bool SerialPort::writeSerialPortOne(uint8_t* buffer)
{
	DWORD bytesSend;

	if (!WriteFile(this->handler, (void*)buffer, 1, &bytesSend, 0)) {
		ClearCommError(this->handler, &this->errors, &this->status);
		return false;
	}
	else return true;
}

bool SerialPort::writeSerialPortDirect(uint8_t word)
{
	DWORD bytesSend;

	if (!WriteFile(this->handler, (void*)&word, 1, &bytesSend, 0)) {
		ClearCommError(this->handler, &this->errors, &this->status);
		return false;
	}
	else return true;
}

bool SerialPort::isConnected()
{
	if (!ClearCommError(this->handler, &this->errors, &this->status))
		this->connected = false;

	return this->connected;
}
// Close Connection
void SerialPort::closeSerial()
{
	CloseHandle(this->handler);
}

bool SerialPort::WriteData(uint8_t buffer)
{
	DWORD bytesSend;
	if (!WriteFile(this->handler, (void*)&buffer, sizeof(uint8_t), &bytesSend, 0))
	{
		ClearCommError(this->handler, &this->errors, &this->status);
		return false;
	}
	else
		return true;
}
