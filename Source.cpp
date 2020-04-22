#include "Header.h"

int main(int argc, char* argv[])
{

#ifdef _MSC_VER
	if (argc>1)
	{
		Child();
	}
	else
	{
		Parent(argv[0]);
	}

#else
	char *shm;
	key_t key;
	int pid;
	int choice;
	key = shmget(UNIQENUMBER, SHMSIZE, 0666 | IPC_CREAT);
	if ((shm = (char*)shmat(key, (void*)0, 0)) == (char*)(-1)) {
		printf("Can't attach shared memory\n");
		exit(-1);
	}
	char *s = (char *)shm;
	while (true) {
		fflush(stdin);
		string str;
		printf("Enter string: ");
		getline(cin, str);
		strcpy(s, str.c_str());
		pid = fork();
		switch (pid) {
		case 0: {
			client(shm, key);
			printf("Exit? 1/0\n");
			do {
				fflush(stdin);
				cin >> choice;
			} while (choice<0 || choice>1);
			if (choice == 1) {
				kill(pid, SIGTERM);
				exit(0);
			}
			kill(getpid(), SIGTERM);
			break;
		}
		case -1: {
			perror("ERROR");
			exit(0);
		}
		default: {
			wait(NULL);
			break;
		}
	  }
	}
#endif
	return 0;
}

#ifdef _MSC_VER
void Child()
{
	HANDLE Print = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Print");
	HANDLE Close = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "Close");
	HANDLE WINAPI FileProjection = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, "FileProjection");
	LPVOID uk;
	char* str;
	char bufstr[MAX];
	int j = 0;
	uk = MapViewOfFile(FileProjection, FILE_MAP_ALL_ACCESS, 0, 0, 20);
	str = (char*)uk;
	while (true)
	{
		WaitForSingleObject(Print, INFINITE);
		if (WaitForSingleObject(Close, 10) == WAIT_OBJECT_0)
		{
			CloseHandle(Print);
			CloseHandle(Close);
			UnmapViewOfFile(uk);
			CloseHandle(FileProjection);
			return;
		}
		int i = 0;
		for (i = 0; str[i] != '\0'; i++)
		{
			bufstr[j++] = str[i];
		}
		if (str[i] == '\0')
		{
			bufstr[j] = '\0';
			printf("%s", bufstr);
			j = 0;
		}
		ReleaseSemaphore(Print, 1, NULL);
	}
}
void Parent(char* prog)
{
	HANDLE Print = CreateSemaphore(NULL, 0, 1, "Print");
	HANDLE Close = CreateSemaphore(NULL, 0, 1, "Close");
	HANDLE WINAPI FileProjection = CreateFileMapping(INVALID_HANDLE_VALUE,
                                                     NULL,
                                                     PAGE_READWRITE, 
                                                     DW_MAXIMUM_SIZE_LOW, 
                                                     DW_MAXIMUM_SIZE_HIGH, 
                                                    "FileProjection");
	STARTUPINFO SI;
	ZeroMemory(&SI, sizeof(STARTUPINFO));
	SI.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION PI;
	if (!CreateProcess(prog, "child process", NULL, NULL, FALSE, NULL, NULL, NULL, &SI, &PI)){
		printf_s("Create Process failed %d\n", GetLastError());
	}
	LPVOID buf;
	buf = MapViewOfFile(FileProjection, FILE_MAP_ALL_ACCESS, 0, 0, 20);
	char *str = (char*)buf, bufstroka[MAX];
	while (true){
		printf_s("Input string...\n");
		fflush(stdin);
		gets_s(bufstroka);
		int j = 0, i = 0, k = 0;
		while (bufstroka[j++] != '\0');		//j = length
		while (i<j)	{
			k = 0;
			while (bufstroka[i] != '\0') {
				str[k++] = bufstroka[i++];
			}
			if (bufstroka[i] == '\0'){
				str[k] = '\0';
				i++;
			}
			ReleaseSemaphore(Print, 1, NULL);
			WaitForSingleObject(Print, INFINITE);
		}
		printf_s("\nExit (y/n)\n");
		fflush(stdin);
		if (getch() == 'y'){
			ReleaseSemaphore(Close, 1, NULL);
			ReleaseSemaphore(Print, 1, NULL);
			WaitForSingleObject(PI.hProcess, INFINITE);
			UnmapViewOfFile(buf);
			CloseHandle(PI.hProcess);
			CloseHandle(PI.hThread);
			CloseHandle(Close);
			CloseHandle(Print);
			CloseHandle(FileProjection);
			return;
		}
	}
}

#else
void client(char* sharedMemory, int key) {

	key = shmget(UNIQENUMBER, SHMSIZE, 0);
	sharedMemory = (char*)shmat(key, (void*)0, 0);
	printf("Child wrote: %s\n", sharedMemory);
	shmdt(sharedMemory);

}
#endif


