#include "ArduinoReader.h"

HANDLE hPort = 0;
wchar_t* comPort = AR_DEFAULT_COM_PORT;
int connected = 0;
COMSTAT status = {0};
DWORD error = 0;

int connect_current_port(const char* access, AR_BAUD_RATE baud_rate){
	if(connected){
		printf("ERROR: Already connected.");
		return -1;
	}
	if(!comPort){
		printf("ERROR: No COM Port selected.");
		return -2;
	}
	if(access != "r" && access != "w" && access != "rw"){
		printf("ERROR: Invalid access rights.");
		return -3;
	}

	DCB dcb;
	hPort = CreateFile(
		comPort,
		access == "r" ? GENERIC_READ : access == "w" ? GENERIC_WRITE : access == "rw" ? (GENERIC_READ | GENERIC_WRITE) : 0,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
	);

	if(!GetCommState(hPort, &dcb))
		return GetLastError();

	dcb.BaudRate = baud_rate;
	dcb.ByteSize = 8;												// 8 data bits
	dcb.Parity = NOPARITY;											// no parity
	dcb.StopBits = ONESTOPBIT;										// 1 stop

	if(!SetCommState(hPort, &dcb))
		return GetLastError();

	connected = 1;
	printf("Connected with Port %ls.\n", comPort);

	return 0;
}

int connect_port(wchar_t* port, const char* access, AR_BAUD_RATE baud_rate){
	// Can't connect to another port, if we are already connected one
	if(connected) return -1;
	comPort = port;
	return connect_current_port(access, baud_rate);
}

int close_port(){
	connected = 0;
	printf("Disconnecting from Port %ls.\n", comPort);

	return CloseHandle(hPort);
}

int write_on_port(const BYTE* data, unsigned int bytes){
	if(!connected) return 0;

	DWORD bytes_written;

	int retVal = WriteFile(hPort, data, bytes, &bytes_written, NULL);
	printf("%lu bytes written on port %ls\n", bytes_written, comPort);
	
	return retVal;
}

BYTE* read_from_port(DWORD min_byte_count, DWORD max_byte_count, DWORD* bytes_read){
	*bytes_read = 0;
	if(!connected){
		printf("ERROR: Trying to read from unconnected port.\n");
		return NULL;
	}
	if(!(max_byte_count > 0)){
		printf("ERROR: Max. byte count must be bigger than 0.\n");
		return NULL;
	}
	if(max_byte_count < min_byte_count){
		printf("ERROR: Max. byte count must be bigger than min. byte count.\n");
		return NULL;
	}

	// Read buffer
	BYTE* buffer = (BYTE*) malloc(max_byte_count);
	DWORD bytes_transferred;

	if(!buffer){
		*bytes_read = 0;
		printf("ERROR: Couldn't allocate memory for read buffer\n");
		return NULL;
	}

	
	ClearCommError(hPort, &error, &status);			// Get status info on the Serial port
	DWORD queue_count = status.cbInQue;				// Save the # of bytes in queue at current point and use it further on, since "status.cbInQue" is async
	printf("%lu bytes in queue\n", queue_count);

	// There must be at least the minimum amount of bytes in the queue
	if(queue_count < min_byte_count){
		printf("Nr. of bytes in queue (%lu) is currently smaller than target min. # of bytes (%d).\n", queue_count, min_byte_count);	// NOT an error!
		free(buffer);
		*bytes_read = 0;
		return NULL;
	}

	// Nr. of bytes we want or can read from the buffer
	DWORD read_count = max_byte_count > queue_count ? queue_count : max_byte_count;
	if(!ReadFile(hPort, buffer, read_count, &bytes_transferred, NULL)){
		printf("ERROR: Couldn't read port (Error %lu)\n", GetLastError());
	}

	printf("%lu bytes transfered\n", bytes_transferred);

	// Reduce buffer to actual read size
	if(!bytes_transferred){
		free(buffer);
		buffer = NULL;
	}
	else{
		BYTE* tmp = buffer;
		buffer = realloc(buffer, bytes_transferred);
		if(!buffer) buffer = tmp;
	}

	*bytes_read = bytes_transferred;

	// Didn't receive enough bytes somehow
	// Return the bytes, that have been read, but output a warning
	if(bytes_transferred < min_byte_count){
		printf("WARNING: Transferred byte count (%lu) lower than target minimum (%lu).\n", bytes_transferred, min_byte_count);
		return buffer;
	}

	return buffer;
}
