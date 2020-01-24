#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <cstdint>
#include <fstream>
#include <map>
#include "SerialPort.h"
#include "Buffer.h"

using namespace std;

extern map <int, string> Sensors;
extern vector <Buffer> Devices;


Buffer::Buffer(uint8_t Tag = 0, int Number = 0) : m_Number(Number), m_Tag(Tag), m_DeviceName("N/A"), m_Head(0), m_Tail(0), m_Size(0), m_Time(false), m_Format(0) {
	int tag, board, sensor, number;
	tag = static_cast<int> (Tag);
	board = (224 & tag) >> 5;    
	sensor = (28 & tag) >> 2;
	number = (3 & tag); // od zera
	m_DeviceName = "Board" + to_string(board) + "_" + Sensors[sensor] + to_string(number);
	for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
		m_Data[i] = 0;
	}
}
Buffer::~Buffer() {}
bool Buffer::BufferEmpty() {
	return m_Size == 0;
}
bool Buffer::BufferFull() {
	return m_Size == MAX_BUFFER_SIZE;
}
int Buffer::get_Number() {
	return m_Number;
}
uint8_t Buffer::get_Tag() {
	return m_Tag;
}
string Buffer::get_DeviceName() {
	return m_DeviceName;
}
int  Buffer::get_Head() {
	return m_Head;
}
int Buffer::get_Tail() {
	return m_Tail;
}
int Buffer::get_Size() {
	return m_Size;
}

int Buffer::get_Format() {
	return m_Format;
}

void Buffer::set_Format(uint8_t format) {  // temporaly disabled, because of no service for Format and Time
	//m_Time = format & 0b01100000;
	//m_Format = format & 0b10000000;
	m_Format = format;
}

int Buffer::get_Data(int n) {
	return static_cast<int> (m_Data[n]);
}

bool Buffer::BufferEnqueue(const uint8_t(&data)[MAX_STREAM]) {  // enqueue buffer with STREAM sized uint8 array
	if ((MAX_BUFFER_SIZE - MAX_STREAM > m_Size)) {
		for (int i = 0; i < MAX_STREAM; i++) {
			m_Data[m_Tail + i] = data[i];
		}
		m_Tail += MAX_STREAM;
		m_Size += MAX_STREAM;
		return true;
	}
	else {
		return false; // error
	}
}

bool Buffer::BufferEnqueueFast(const uint8_t(&data)[MAX_STREAM + 1]) {  // enqueue for use in static BufferEnqueueRaw method
	for (int i = 0; i < MAX_STREAM; i++) {
		m_Data[m_Tail + i] = data[i + 1];
	}
	m_Tail += MAX_STREAM;
	m_Size += MAX_STREAM;
	return true;
}

bool Buffer::BufferEnqueueFastCustom(const uint8_t(&data)[MAX_STREAM + 1], int size) {  // enqueue for use in static BufferEnqueueRaw method
	for (int i = 0; i < size; i++) {
		m_Data[m_Tail + i] = data[i + 1];
	}
	m_Tail += size;
	m_Size += size;
	return true;
}

int Buffer::BufferEnqueueRawCustom(const uint8_t(&data)[MAX_STREAM + 1], int size) {  // static method for enqueue buffer with STREAM sized uint8 array with Tag translation and creation of missing Device
	bool exists = false;
	int number{};
	for (int i = 0; i < Devices.size(); i++) {
		if (Devices[i].get_Tag() == data[0]) {
			exists = true;
			number = i;
			break;
		}
	}
	if (!exists) {   // create missing object
		number = Devices.size();
		Devices.push_back(Buffer(data[0], number));
	}
	if ((MAX_BUFFER_SIZE - size > Devices[number].get_Size())) {
		Devices[number].BufferEnqueueFastCustom(data, size);  // enqueue
		return number;
	}
	else {
		return 222; // error when data to big to enqueue
	}
}

int Buffer::BufferEnqueueRaw(const uint8_t(&data)[MAX_STREAM + 1]) {  // static method for enqueue buffer with STREAM sized uint8 array with Tag translation and creation of missing Device
	bool exists = false;
	int number{};
	for (int i = 0; i < Devices.size(); i++) {
		if (Devices[i].get_Tag() == data[0]) {
			exists = true;
			number = i;
			break;
		}
	}
	if (!exists) {   // create missing object
		number = Devices.size();
		Devices.push_back(Buffer(data[0], number));
	}
	if ((MAX_BUFFER_SIZE - MAX_STREAM > Devices[number].get_Size())) { // is there enough space for STREAM sized data
		Devices[number].BufferEnqueueFast(data);  // enqueue
		return number;
	}
	else {
		return 222; // error when data to big to enqueue
	}
}

bool  Buffer::BufferDequeue(uint8_t(&data)[MAX_STREAM]) {
	if (m_Size <= MAX_STREAM) {
		for (int i = 0; i < MAX_STREAM; i++) {
			data[i] = m_Data[m_Head + i];
			m_Data[m_Head + i] = 0;
		}
		m_Head += MAX_STREAM;
		m_Size -= MAX_STREAM;
		m_Head %= MAX_BUFFER_SIZE;
		if (m_Head == 0) {
			m_Size = 0;
			m_Tail = 0;
		}
		return true;
	}
	else {
		return false; // error
	}
}

bool  Buffer::Buffer_Write_to_File(int size) {
	if ((size > m_Size) && (size < MAX_BUFFER_SIZE)) {
		size = m_Size;
	}
	if ((size <= m_Size) && (!(this->BufferEmpty()))) {
		char name[MAX_FILE_NAME_LENGTH];
		string name_str = m_DeviceName;
		name_str.append(".txt");
		strcpy(name, name_str.c_str());
		ofstream ostrm(name, ios_base::app); // aby dopisywaæ
		while (!(ostrm.is_open())) {

		}
		if (ostrm.is_open()) {
			//cout << "size: " << size << endl;
			for (int i = 0; i < size; i++) {
				ostrm << static_cast<int>(m_Data[m_Head + i]) << " "; //endl;
				//cout << "zapisano: " << static_cast<int>(m_Data[m_Head + i]) << " dla indeksu: " << i << endl;
				m_Data[m_Head + i] = 0;

			}
			cout << " zapisano do " << name_str << endl;
			ostrm << endl;
			ostrm.close();
			ostrm.clear();
			m_Head += size;
			m_Size -= size;
			m_Head %= MAX_BUFFER_SIZE;
			if (m_Head == 0) {
				m_Size = 0;
				m_Tail = 0;
			}
			else {
				this->BufferShift(); // move when non-empty
			}
			return true;
		}
	}
	else {
		return false; // error
	}
}

bool  Buffer::Buffer_Write_to_File_Format(int size) {
	int data{}; int time{};
	if ((size > m_Size) && (size < MAX_BUFFER_SIZE)) {
		size = m_Size;
	}
	if ((size <= m_Size) && (!(this->BufferEmpty()))) { // dałem < zamist <=
		char name[MAX_FILE_NAME_LENGTH];
		string name_str = m_DeviceName;
		name_str.append(".txt");
		strcpy(name, name_str.c_str());
		ofstream ostrm(name, ios_base::app); // aby dopisywac
		while (!(ostrm.is_open())) {

		}
		if (ostrm.is_open()) {
			this->BufferShift(); // move when non-empty
			for (int i = 0; i < size; i += 6) {
				data = 0; time = 0;
				data = (m_Data[m_Head + i]) << 8;    // used instead of specialized method using Format and Time fields // temporal
				data += (m_Data[m_Head + i + 1]);
				time = (m_Data[m_Head + i + 2]) << 24;
				time += (m_Data[m_Head + i + 3]) << 16;
				time += (m_Data[m_Head + i + 4]) << 8;
				time += (m_Data[m_Head + i + 5]);
				ostrm << data << " "; 
				ostrm << time << endl;
				m_Data[m_Head + i] = 0;
			}
			cout << " zapisano do " << name_str << endl;
			ostrm.close();
			ostrm.clear();
			m_Head += size;
			m_Size -= size;
			m_Head %= MAX_BUFFER_SIZE;
			if (m_Head == 0) {
				m_Size = 0;
				m_Tail = 0;
			}
			else {
				this->BufferShift(); // move when non-empty
			}
			return true;
		}
	}
	else {
		return false; // error
	}
}

void  Buffer::BufferShift(void) {  // buffer moves left so Head equals 0 ///// better to use separately
	if ((m_Size > 0) && (m_Head > 0)) {
		for (int i = 0; i < m_Size; i++) {
			m_Data[i] = m_Data[m_Head + i];
			m_Data[m_Head + i] = 0;
		}
		m_Tail -= m_Head;
		m_Head = 0;
	}
}

string File_Names_Decode(const char input) { // translates device TAG to file name
	int tag, board, sensor, number;
	string name;
	tag = static_cast<int> (input);
	board = (224 & tag) >> 5;
	sensor = (28 & tag) >> 2;
	number = (3 & tag) + 1;
	name = "Board" + to_string(board) + "_" + Sensors[sensor] + to_string(number);
	return name;
}

void File_Clear(const string& FileName) {    // function deleting .txt file contents
	char name[MAX_FILE_NAME_LENGTH];
	string name_str = FileName;
	name_str.append(".txt");
	strcpy(name, name_str.c_str());
	ofstream ostrm(name, ios_base::app);
	if (ostrm.is_open()) {
		ostrm.close();
		ostrm.clear();
	}
}

void File_Write(const string& FileName, const string& word) {    // function with one string input
	char name[MAX_FILE_NAME_LENGTH];
	string name_str = FileName;
	name_str.append(".txt");
	strcpy(name, name_str.c_str());
	ofstream ostrm(name, ios_base::app);
	if (ostrm.is_open()) {
		ostrm << word << endl;
		ostrm.close();
		ostrm.clear();
	}

}

