// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "resource.h"
#include "resource1.h"

typedef int(*__stdcall MoveCards)(HWND);

void handle_5();

HACCEL FreeMenu;
HHOOK hkb;

LRESULT CALLBACK Callback(int nCode, WPARAM wParam, LPARAM lParam)
{
	// convert contents of message in lParam
	LPMSG lMsg = (LPMSG)lParam;
	// hWnd of freecell
	HWND hWnd = *(HWND*)0x01008374;
	// using new accelerator table - translate message
	if (TranslateAccelerator(hWnd, FreeMenu, lMsg) != 0)
	{
		// success
		if (lMsg->lParam == 0x3C0001) {
			// execute method to win game
			handle_5();
			return (LRESULT)0;
		}
	}
	else {
		// failed
		TranslateMessage(lMsg);
		DispatchMessage(lMsg);
	}
	// call next hook in queue
	return CallNextHookEx(hkb, nCode, wParam, lParam);
}

void handle_1() {
	DWORD old;
	// base address of program
	DWORD base = (DWORD)GetModuleHandle(NULL);
	// starting address of string in binary
	DWORD targetVirtual = 0xD404;
	// raw ofset of .rsrc data section
	DWORD rawRsrc = 0x5800;
	// virtual address of .rsrc data section
	DWORD virtualRsrc = 0x9000;
	// calculated raw address
	DWORD targetRawAddress = base + targetVirtual + virtualRsrc - rawRsrc;
	char* ptr = (char*)targetRawAddress;

	if (VirtualProtect(ptr, 0x31, 0x40, &old) == 0) {
		MessageBoxA(0, "Failed To change memory protection", "Fail", 1);
	}
	else {
		char msg[] = "Not in this game.";
		for (int x = 0; x < sizeof(msg) / sizeof(msg[0]); x++)
			ptr[x * 2] = msg[x];
	}
}

void handle_2() {
	// store key in freecell mem
	HKEY* _hkey = (HKEY*)0x10079A4;
	// pointer to registry path
	WCHAR* _pszRegPath = (WCHAR*)0x1001230;
	// pointer to pszWon variable
	WCHAR* _pszWon = (WCHAR*)0x10012A8;
	// create registry key - store in _hkey
	RegCreateKeyW((HKEY)2147483649, _pszRegPath, _hkey);
	// set registry at _pszWon to data
	DWORD data = 1000;
	RegSetValueExW(*_hkey, _pszWon, 0, 3, (LPBYTE)&data, 4);
	// close registry key
	RegCloseKey(*_hkey);
}

void handle_3() {
	// cheat = 2  next move will result in win
	*(unsigned int *)0x1007130 = 2;
}

void handle_4_5(HMODULE hModule)
{
	// load custom accelerator table
	FreeMenu = LoadAccelerators(hModule, L"FreeMenu");
	
	HWND hWnd = *(HWND *)0x01008374;
	DWORD dwProcID = GetCurrentProcessId();
	DWORD dwThreadID = GetWindowThreadProcessId(hWnd, &dwProcID);
	// hook all messages for freecell thread --> execute callback 
	hkb = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)Callback, NULL, dwThreadID);
	// continually wait for messages -- causes infinite loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {};
}

void handle_5() {
	HWND hWnd = *(HWND*)0x01008374;
	//To make sure that moves continue to happen
	*(unsigned int *)0x01007864 = 1;
	//To make sure that the cards are 0 so that you can win.
	//If cards are not zero, that means that there is still cards left in play
	*(unsigned int *)0x01007800 = 0;
	//move the rest of the cards to win
	MoveCards move_cards = (MoveCards)0x01004FC7;
	move_cards(hWnd);
}

BOOL APIENTRY DllMain(	HMODULE hModule,
						DWORD  ul_reason_for_call,
						LPVOID lpReserved
) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		Sleep(100);
		MessageBoxA(0, "Injected", "Hello", 1);
		handle_1();
		handle_2();
		handle_3();
		handle_4_5(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}



