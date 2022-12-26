#ifndef __ARDUINOREADER_H__
#define __ARDUINOREADER_H__

#include <Windows.h>
#include <stdio.h>

#define AR_DEFAULT_COM_PORT L"COM3"		// Default COM Port

//////////////////////////////////////////////////////////////////////////////////
//																				//
//	ARDUINO READER: Read and write from and to an Arduino connected via USB		//
//																				//
//	Only one connection per program currently supported.						//
//																				//
//////////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
extern "C"
{
#endif

// Legal baud rates
typedef enum __AR_BAUD_RATE__ {
	AR_110 = 110,
	AR_300 = 300,
	AR_600 = 600,
	AR_1200 = 1200,
	AR_2400 = 2400,
	AR_4800 = 4800,
	AR_9600 = 9600,
	AR_14400 = 14400,
	AR_19200 = 19200,
	AR_38400 = 38400,
	AR_56000 = 56000,
	AR_57600 = 57600,
	AR_115200 = 115200,
	AR_128000 = 128000,
	AR_256000 = 256000
} AR_BAUD_RATE;

// TODO: Add "Port" struct and save an array of ports for possibility of connecting with multiple ports

extern HANDLE hPort;				// Currently connected port
extern wchar_t* comPort;			// Target COM Port
extern int connected;				// Connection status
extern COMSTAT status;				// COM Port status
extern DWORD error;					// Last error

// Connect with the currently set COM Port (default: COM3)
// Access: "r" for read-only, "w" for write-only, "rw" for both
int connect_current_port(const char* access, AR_BAUD_RATE baud_rate);
// Connects with the given COM Port
int connect_port(wchar_t* port, const char* access, AR_BAUD_RATE baud_rate);
// Disconnects from port
int close_port();

// Writes data on COM Port and returns 0 on success
int write_on_port(const BYTE* data, unsigned int bytes);
// Read bytes between the min. and max. count from the port and return a sequence of bytes
BYTE* read_from_port(DWORD min_byte_count, DWORD max_byte_count, DWORD* bytes_read);

#ifdef __cplusplus
}
#endif

#endif // !__ARDUINOREADER_H__
