#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include <vector>
#include <string>
#include <filesystem>
#include <psapi.h>

DWORD get_parent_pid(DWORD _current = 0) {
	DWORD pid = 0;
	if (_current == 0) {
		_current = GetCurrentProcessId();
	}
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return pid;
	}
	PROCESSENTRY32 pe32{ sizeof(PROCESSENTRY32) };
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return pid;
	}
	DWORD  crtpid = GetCurrentProcessId();
	do {
		if (crtpid == pe32.th32ProcessID) {
			pid = pe32.th32ParentProcessID;
			break;
		}
	} while (pid == 0 && Process32Next(hProcessSnap, &pe32));
	CloseHandle(hProcessSnap);
	return pid;
}

std::vector<DWORD> get_executable_pid(const std::string& _executable) {
	namespace fs = std::filesystem;
	std::vector<DWORD> pids;
	const auto exec = fs::path(_executable);
	if (!exec.has_filename()) {
		return pids;
	}
	const auto executableName = exec.filename();
	HANDLE hProcess{ nullptr };
	PROCESSENTRY32 pe32{ sizeof(PROCESSENTRY32) };
	memset(pe32.szExeFile, 0, sizeof(pe32.szExeFile));
	DWORD dwPriorityClass{ 0 };

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return pids;
	}

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return pids;
	}
	DWORD pid = 0;
	DWORD  crtpid = GetCurrentProcessId();
	do {
		if (std::string(pe32.szExeFile) == executableName) {
			pids.push_back(pe32.th32ProcessID);
		}
	} while (pid == 0 && Process32Next(hProcessSnap, &pe32));
	CloseHandle(hProcessSnap);
	return pids;
}

HMODULE get_module_handle(DWORD _pid, const std::string& _module)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, _pid);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return 0;
	}
	namespace fs = std::filesystem;
	const auto dll = fs::path(_module);
	if (!dll.has_filename()) {
		return 0;
	}
	const auto dllName = dll.filename().string();
	MODULEENTRY32 me32{ sizeof(MODULEENTRY32) };
	if (!Module32First(hProcessSnap, &me32))
	{
		CloseHandle(hProcessSnap);
		return 0;
	}
	do {
		if (std::string(me32.szModule) == dllName) {
			return me32.hModule;
		}
	} while (Module32Next(hProcessSnap, &me32));
	CloseHandle(hProcessSnap);
	return 0;
}
int main(int argc, char* argv[]) {
	if (argc < 4) {
		printf("Usage: process_injector <attach|detach> <executable_name> <dll_name>");
		return EXIT_SUCCESS;
	}

	const std::string mode = std::string(argv[1]);
	const std::string exe = std::string(argv[2]);
	const std::string dll = std::string(argv[3]);
	auto pids = get_executable_pid(exe);
	if (mode == "attach" || mode == "detach") {
		if (pids.empty()) {
			printf("no exe is running\n");
			return EXIT_FAILURE;
		}
		if (pids.size() > 1) {
			printf("multiple process found\n");
			return EXIT_FAILURE;
		}
	}
	else {
		printf("unknown mode\n");
		return EXIT_FAILURE;
	}
	// PROCESS_QUERY_INFORMATION - required for read loaded dlls
	HANDLE hProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_CREATE_THREAD, 0, pids[0]);
	if (hProcess == NULL) {
		printf("cannot obtain pid handle\n");
		return EXIT_FAILURE;
	}
	if (mode == "attach") {

		size_t bufferSize = 1 << 12;
		void* buffer = VirtualAllocEx(hProcess, NULL, 1 << 12, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (buffer == NULL) {
			printf("Failed during virtual alloc\n");
			return EXIT_FAILURE;
		}
		std::vector<char> emptyData(bufferSize, 0);
		if (!WriteProcessMemory(hProcess, buffer, emptyData.data(), emptyData.size(), NULL)) {
			printf("cannot clear memory\n");
			return EXIT_FAILURE;
		}
		if (!WriteProcessMemory(hProcess, buffer, dll.data(), dll.size(), NULL)) {
			printf("cannot set memory\n");
			return EXIT_FAILURE;
		}
		HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
			(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(L"kernel32"), "LoadLibraryA"),
			buffer, 0, NULL);
		if (!hThread) {
			printf("Cannot run thread\n");
			return EXIT_FAILURE;
		}
		printf("SUCCESS!");
	}
	else if (mode == "detach") {
		HMODULE hmodule = get_module_handle(pids[0], dll);
		if (hmodule == 0) {
			printf("moodule not loaded\n");
			return EXIT_FAILURE;
		}

		HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
			(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleW(L"kernel32"), "FreeLibrary"),
			hmodule, 0, NULL);
		if (!hThread) {
			printf("Cannot run thread\n");
			return EXIT_FAILURE;
		}
		printf("SUCCESS!");
	}
	return 0;

}