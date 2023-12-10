#include <Windows.h>
#include <detours.h>
#include <string>
#include <iostream>

#undef max

std::wstring module_path(HMODULE hModule) {

	DWORD length = 10ull;
	std::wstring path(length + 1ull, 0);
	DWORD result = 0; 
	while (GetModuleFileNameW(hModule, path.data(), length) == length) {
		length = length * 2ull;
		printf("loop resize %d\n", (int)length);
		path = std::wstring(length + 1ull, 0);
	}
	printf("final size %d: %d\n", (int)length, lstrlenW(path.data()));

	return path;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {

	if (DetourIsHelperProcess()) {
		return TRUE;
	}
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH: {
		std::wcout << "attach path: " << module_path(hModule) << std::endl;
		break;
	}
	case DLL_PROCESS_DETACH: {
		std::wcout << "datach module" << module_path(hModule) << std::endl; 
		break;
	}
	}
	return TRUE;
}

BOOL APIENTRY Detours_Func() {
	return TRUE;
}
