#include "DllLoader.h"
#include <Windows.h>

int main()
{
	DllLoader<
		NamedArgument<"MessageBoxW", int(*)(HWND, LPCWSTR, LPCWSTR, UINT)>
	> dll_loader{ L"user32.dll" };

	if (not dll_loader.Valid())
	{
		return -1;
	}

	dll_loader.Invoke<"MessageBoxW">(nullptr, L"Hello, DllLoader!", L"Test MessageBox", MB_OK);
	dll_loader.Invoke<"MessageBoxW">(nullptr, L"Hello, DllLoader using cache!", L"Test MessageBox", MB_OK);	// Cached
}
