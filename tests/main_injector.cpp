#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <detours.h>

int main(int argc, char* argv[])
{
	if (argc != 3) {
		std::cout << "usage: command <executable> <hook_dll>" << std::endl; 
		return EXIT_SUCCESS;
	}
	std::string exe(argv[1]);
	std::string dll(argv[2]);

	namespace fs = std::filesystem;
	auto exepath = fs::absolute(exe);
	std::wstring executable = exepath.wstring();
	std::string hookdll = fs::absolute(dll).string();
	PROCESS_INFORMATION pi;
	STARTUPINFOW si;

	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	std::wcout << "executable: " << executable << std::endl; 
	std::cout << "hookdll: " << hookdll << std::endl;
#if 1
	BOOL created = DetourCreateProcessWithDllExW(
		executable.c_str(),
		NULL, // lpCommandLine,
		NULL, // lpProcessAttributes,
		NULL, // lpThreadAttributes,
		TRUE, // bInheritHandles,
		CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED, // dwCreationFlags,
		NULL, // lpEnvironment
		exepath.parent_path().c_str(), // lpCurrentDirectory,
		&si, // lpStartupInfo,
		&pi, // lpProcessInformation,

//		"C:\\Projects\\deterministic\\DetourCreateProcessWithDllEx-example\\x64\\Debug\\DetoursHook64.dll",
		hookdll.c_str(),
		NULL
	);
#else 

	BOOL created = CreateProcessW(
		executable.c_str(),
		NULL, // lpCommandLine,
		NULL, // lpProcessAttributes,
		NULL, // lpThreadAttributes,
		FALSE, // bInheritHandles,
		0, // dwCreationFlags,
		NULL, // lpEnvironment,
		NULL, // lpCurrentDirectory,
		&si, // lpStartupInfo,
		&pi // lpProcessInformation
	);
#endif 
	if (created) {

		ResumeThread(pi.hThread);
		printf("created process\n");
		DWORD exitCode = 0;
		do {
			if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
				printf("cannot get exit code\n");
				break;
			}
			if (WAIT_OBJECT_0 == WaitForSingleObject(pi.hProcess, 1000)) {
				printf("waiting\n");
				break;
			}
		} while (STILL_ACTIVE == exitCode);
		printf("process exited\n");
	}
	return EXIT_SUCCESS;
}