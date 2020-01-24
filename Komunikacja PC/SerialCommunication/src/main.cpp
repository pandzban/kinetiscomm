///////////////////////////////////////////////////////
//  Projekt TMP Dudzik Gawlik 
///////////////////////////////////////////////////////


#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <cstdint>
#include <fstream>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include "SerialPort.h"
#include "Buffer.h"
#include <windows.h>

using namespace std;

char* portName = (char*)"\\\\.\\COM6";

#define MAX_DATA_LENGTH 257
#define MAX_FILE_NAME_LENGTH 257
#define MAX_STREAM 256
#define MAX_BUFFER_SIZE 32*MAX_STREAM
#define MAX_DEVICES 32

class Buffer;
vector <Buffer> Devices;

map <int, string> Sensors = { {0 , "ADC"}, {1 , "Term"}, {2 , "Pit"}, {3 , "NotSpecified"}, {4 , "NotSpecified"}, {5 , "NotSpecified"}, {6 , "NotSpecified"}, {7 , "NotSpecified"}, };
map <string, int> commands = { { "show", 0}, { "disable", 1}, { "enable", 2}, { "A", 3}, { "AA", 4}, { "AAA", 5}, { "AAAA", 6}, { "AAAAA", 7}, { "AAAAAA", 8} };

SerialPort* kl46z;

namespace association {
	void divulgation(void) {   // divulgation of PC activity for two-way transmission( during divulgation time MC is obligated to set up hierarchy, and routing paths among other MCs and get ready to service PCs UART0 )
		uint8_t PC = 170, word = 0;
		do {
			kl46z->WriteData(PC);
			Sleep(500); // wait 10ms
			kl46z->readSerialPortUint(&word, 1);
			cout << "otrzymano: " << static_cast<int> (word) << endl;
		} while (word != 170); // received ready
		cout << "finished" << endl;
	}

	class Information {
	public:
		static uint8_t m_Number;
		static int Entries;
		uint8_t m_Tag;
		uint8_t m_Format;
		bool m_SEND;

		Information(uint8_t Tag, uint8_t Format) : m_Tag(Tag), m_Format(Format), m_SEND(false) {}
	};

	vector <Information> Members;
	uint8_t Information::m_Number = 0;
	int Information::Entries = 0;

	void interrogation(void) {   // interrogation of MCs for their devices parameters
		uint8_t receivedInfo[3], receivedSTag = 0, Number{}, Tag{}, Format{}; // StreamTag != Tag
		bool Error = false;
		bool exists = false;
		int number{};
		int exit = 0;
		while (true) {
			do {
				do {
					kl46z->readSerialPortUint(&receivedSTag, 1);
					if (receivedSTag == 206) {
						exit++;
					}
					if (exit == 3) {
						break;
					}
				} while (receivedSTag != 204);  // wait for 204
				if (exit == 3) {
					break;
				}
				kl46z->readSerialPortUint(receivedInfo, 3);  // read 3 bytes containing Tag, Number and Format
				if (Number == 0) {
					Number = receivedInfo[1];
				}
				Tag = receivedInfo[0];
				Format = receivedInfo[2];
				exists = false;
				number = 0;
				if ((Information::m_Number != 0) && (Number != Information::m_Number)) {
					Error = true;
				}
				else {
					Information::m_Number = Number;
				}
				if (Information::Entries < Information::m_Number) {
					Members.push_back(Information(Tag, Format));
					Information::Entries++;
					if (!(Members[Information::Entries - 1].m_SEND)) {
						kl46z->WriteData(Tag);  // send confirmation of data for accept
						kl46z->WriteData(Number);
						kl46z->WriteData(Format);
						Members[Information::Entries - 1].m_SEND = true;
					}
				}
				if (Error && (Number == Information::m_Number)) {  // clear class' vector
					for (auto p : Members) {
					}
					Information::m_Number = 0;
					Information::Entries = 0;
					for (int i = 0; i < Members.size(); i++) {
						Members.erase(Members.begin() + i);
					}
				}
				//Sleep(30);
			} while (Error != false);
			if (Information::Entries != Information::m_Number) {
				continue;
			}
			else if (exit == 3) {
				for (int k = 0; k < Members.size(); k++) {
					for (int i = 0; i < Devices.size(); i++) {
						if (Devices[i].get_Tag() == Members[k].m_Tag) {
							exists = true;
							number = i;
							break;
						}
					}
					if (!exists) {   // create missing object
						number = Devices.size();
						Devices.push_back(Buffer(Members[k].m_Tag, number));
					}
					Devices[number].set_Format(Members[k].m_Format);
				}
				break;    // finish interrogatioon
			}
		}
	}
}

queue <uint8_t*> kolejka;
mutex writing_main_mutex;
condition_variable writing_main_cvar;
thread writing_thread;
thread reading_thread;
//thread control_thread;

namespace control
{
	bool process_command(string line) {
		int size = line.length();
		string command;
		vector <string> objects;
		vector <string> data;
		for (int i = 0; i < 5; i++) {
			objects.push_back("");
		}
		for (int i = 0; i < 5; i++) {
			data.push_back("");
		}
		bool breaks = false;
		bool end = false;
		int idx = 0;
		std::cout << line << endl;
		for (int i = 0; i < size; i++) {
			if (line[i] != ' ') { // if not space
				command += line[i];
			}
			else {
				idx = i;
				break;
			}
		}
		if (idx == 0) {
			cout << "idx pierwsze: " << idx << endl;
			end = true;
		}
		cout << "jeden" << endl;
		for (int k = 0; k < 5; k++) {   // objects count to address
			if (end) {
				break;
			}
			for (int i = idx; i < idx + 5; i++) {
				if (line[i] == ':') {
					breaks = true;
					idx = i + 1;
					break;
				}
				if (line[i] != ' ') {
					idx = i;
					break;
				}
			}
			cout << "dwa" << endl;
			for (int i = idx; i < size; i++) {
				cout << i << endl;
				if (line[i] == ':') {
					breaks = true;
					idx = i + 1;
					break;
				}
				if (line[i] != ' ') {
					objects[k] += line[i];
				}
				else {
					idx = i;
					break;

				}
			}

			if (breaks) {
				break;
			}
		}
		cout << "trzy idx:" << idx << endl;
		cout << "trzy" << endl;
		for (int k = 0; k < 5; k++) {   // data count to apply
			if (end) {
				break;
			}
			for (int i = idx; i < idx + 5; i++) {
				cout << "cztery i :" << i << endl;
				if (line[i] == ':') {
					breaks = true;
					idx = i + 1;
					break;
				}
				if (line[i] != ' ') {
					idx = i;
					cout << "cztery idx" << idx << endl;
					break;
				}
				idx = i;
			}
			cout << "piec" << endl;
			cout << idx << endl;
			for (int i = idx; i < size; i++) {
				cout << i << endl;
				if (line[i] == ':') {
					breaks = true;
					idx = i + 1;
					break;
				}
				if (line[i] != ' ') {
					data[k] += line[i];
				}
				else {
					idx = i;
					break;

				}
			}

			if (breaks) {
				break;
			}
		}

		std::cout << "command: " << command << endl;
		std::cout << "obj: ";
		for (auto p : objects) {
			std::cout << p;
		}
		std::cout << endl;
		std::cout << "data: ";
		for (auto p : data) {
			std::cout << p;
		}
		std::cout << endl;
		return true;
	}
} // unused

namespace reading
{
	int to_read(const uint8_t(&bytes)[2]) {
		return ((bytes[1] & 0b1111) << 8) + bytes[0];
	}

	int bytes_to_blocks(int bytes) {
		return bytes / 256;
	}

	void clear_received(uint8_t(&received)[257]) {
		for (int i = 0; i < 257; i++) {
			received[i] = 0;
		}
	}

	void processFormatInfo(void) {
		uint8_t receivedInt[MAX_DEVICES * 3];
		bool exists = false;
		bool error = false;
		int idx;
		uint8_t Number{ MAX_DEVICES }; uint8_t Tag{}; uint8_t Format{};
		kl46z->readSerialPortFull((char*)receivedInt, MAX_DEVICES * 3);
		Number = receivedInt[1];
		for (int k = 0; k < Number; k++) {
			Tag = receivedInt[k];
			Format = receivedInt[k + 2];
			if (Number != receivedInt[k + 1]) {
				break;  /// error         // edit: create buffer to hold values and check if no error occurs, and then after for loop, uses Buffer class to save it
			}
			exists = false;
			for (int i = 0; i < Devices.size(); i++) {
				if (Devices[i].get_Tag() == Tag) {
					exists = true;
					idx = i;
					break;
				}
			}
			if (!exists) {   // create missing object
				idx = Devices.size();
				Devices.push_back(Buffer(Tag, idx));
			}
			Devices[idx].set_Format(Format);
		}
		// confirm eventually...
	}

	void write_full_buffers(void) { // write all data held in buffers to files
		for (auto p : Devices) {
			if (p.BufferFull()) {
				p.Buffer_Write_to_File(MAX_BUFFER_SIZE);
			}
		}
	}
}

void writing_thread_entry() {      // unused
	while (true) {
		uint8_t* x;
		{
			unique_lock<mutex> lock(writing_main_mutex);
			writing_main_cvar.wait(lock, [] { return !kolejka.empty(); });
			x = kolejka.front();
			kolejka.pop();
		}
		cout << x << endl;// send data x...
	}
}

void reading_thread_entry() {
	int bytes_to_read{}, blocks_to_read{}, remainder_bytes{}, idx{}, error{};
	bool repeat{false};
	uint8_t received{};
	uint8_t receivedTwoInt[2];
	uint8_t receivedInt[MAX_DATA_LENGTH];
	cout << "divulgation began" << endl;
	association::divulgation();
	cout << "divulgation done" << endl;
	association::interrogation();  ////////// answer to detect_PC
	cout << "interrogation done" << endl;
	cout << "Tagi" << endl;
	for (auto p : Devices) {
		cout << static_cast<int>(p.get_Tag()) << endl;
	}
	cout << "Format:" << endl;
	for (auto p : Devices) {
		cout << static_cast<int>(p.get_Format()) << endl;
	}
	while (true) {
		do {
			reading::clear_received(receivedInt);
			error = 0;
			repeat = false;
			do {
				kl46z->WriteData(107); // interrupt mc for data transmission
				reading::write_full_buffers();    // instead of : //Sleep(5);
				if (repeat) {
					Sleep(10); //5 //10
				}
				else {
					Sleep(10); // 50
				}
				kl46z->readSerialPortUint(&received, 1);
				if (received == 204) {
					reading::processFormatInfo(); // special read
				}
				repeat = true;
			} while (received != 117); // conf
			//cout << "zapisywanie" << endl;
			Sleep(3); // 1 // 3
			kl46z->readSerialPortUint(&received, 1);
			bytes_to_read = received;
			Sleep(23); // 15 // 23
			if (bytes_to_read != 0) {
				kl46z->readSerialPortFull((char*)receivedInt, bytes_to_read + 1);
				Sleep(3); // 2 // 3
				kl46z->readSerialPortUint(&received, 1);
				if (received == 255) {
					idx = Buffer::BufferEnqueueRawCustom(receivedInt, bytes_to_read);// + 1);
					Devices[idx].Buffer_Write_to_File_Format(256);
				}
				else {
					cout << "error" << endl;
					kl46z->WriteData(99); // error
				}
			}
		} while (received != 255);  // end of frame

	}

}



void zakolejkuj_dane(uint8_t* x) { // unused
	{
		unique_lock<mutex> lock(writing_main_mutex);
		kolejka.push(x);
	}
	writing_main_cvar.notify_one();
}

void Create_Writing_Thread() {       // unused
	writing_thread = std::thread(writing_thread_entry);
}

void Create_Reading_Thread() {
	reading_thread = std::thread(reading_thread_entry);
}





int main()
{
	std::cout << "Good day!" << std::endl;
	kl46z = new SerialPort(portName);
	//Create_Writing_Thread();  // writing through separated thread not yet implemented
	Create_Reading_Thread();

	string command;
	cout << "Dzien dobry!" << endl;
	bool write_to_queue = false;
	do {                       // pass commands in cmd
		getline(cin, command);
		if (command != "exit") {
			control::process_command(command);
		}
	} while (command != "exit");
	cout << "Dowidzenia!" << endl;
	//watek_piszacy.join();
	reading_thread.join();

	return 0;
}
