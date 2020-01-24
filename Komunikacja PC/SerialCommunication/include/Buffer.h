#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <cstdint>
#include <fstream>
#include <map>


#define MAX_DATA_LENGTH 257
#define MAX_FILE_NAME_LENGTH 257
#define MAX_STREAM 256
#define MAX_BUFFER_SIZE 32*MAX_STREAM

using namespace std;

class Buffer {
private:
	int m_Number;   // allows for flexible class' object indexing with methods etc.
	uint8_t m_Tag;  // allows for fast Tag access
	string m_DeviceName;  // holds device name for fast access
	bool m_Time; // time format enabled
	int m_Format;
	uint8_t m_Data[MAX_BUFFER_SIZE];      // exact data (product of MAX_STREAM and integer)
	int m_Head; // oldest data element pointer
	int m_Tail; // next free space 
	int m_Size;
public:
	Buffer(uint8_t Tag, int Number);
	~Buffer();

	bool BufferEmpty();
	bool BufferFull();
	int get_Number();
	uint8_t get_Tag();
	string get_DeviceName();
	int  get_Head();
	int get_Tail();
	int get_Size();
	int get_Data(int n); // TEST
	int get_Format();
	void set_Format(uint8_t format);  // sets time and data format
	bool BufferEnqueue(const uint8_t(&data)[MAX_STREAM]);  // enqueue buffer with STREAM sized uint8 array // returns buffor object's index
	bool BufferEnqueueFast(const uint8_t(&data)[MAX_STREAM + 1]); // enqueue for use in static BufferEnqueueRaw method // returns buffor object's index
	bool BufferEnqueueFastCustom(const uint8_t(&data)[MAX_STREAM + 1], int size); // returns buffor object's index
	static int BufferEnqueueRaw(const uint8_t(&data)[MAX_STREAM + 1]); // static method for enqueue buffer with STREAM sized uint8 array with Tag translation and creation of missing Device 
	static int BufferEnqueueRawCustom(const uint8_t(&data)[MAX_STREAM + 1], int size);
	bool BufferDequeue(uint8_t(&data)[MAX_STREAM]);
	bool Buffer_Write_to_File(int size);
	bool Buffer_Write_to_File_Format(int size);
	void BufferShift(void);  // buffer moves left so Head equals 0 ///// better to use separately
};

string File_Names_Decode(const char input);
void File_Clear(const string& FileName);
void File_Write(const string& FileName, const string& word);
