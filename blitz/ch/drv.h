
#include "check.h"
#include "Func.h"
#include "xor.h"
#include <TlHelp32.h>
#include <winternl.h>
#include <intrin.h>
uintptr_t virtualaddy;
uintptr_t base_address;
int process_id;
#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)


#define code_rw CTL_CODE(FILE_DEVICE_UNKNOWN, 0x31, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_ba CTL_CODE(FILE_DEVICE_UNKNOWN, 0x32, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_move CTL_CODE(FILE_DEVICE_UNKNOWN, 0x34, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_spoof CTL_CODE(FILE_DEVICE_UNKNOWN, 0x35, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_unlock CTL_CODE(FILE_DEVICE_UNKNOWN, 0x36, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_move_lclick_down CTL_CODE(FILE_DEVICE_UNKNOWN, 0x37, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_move_lclick_up CTL_CODE(FILE_DEVICE_UNKNOWN, 0x38, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_spoofer CTL_CODE(FILE_DEVICE_UNKNOWN, 0x39, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_security 0x72

typedef struct _readwrite {
	INT32 security;
	INT32 process_id;
	ULONGLONG address;
	ULONGLONG buffer;
	ULONGLONG size;
	BOOLEAN write;
} rw, * prw;

typedef struct _MOUSE_REQUEST
{
	BOOLEAN click;
	BOOLEAN status;
	LONG dx;
	LONG dy;

} MOUSE_REQUEST, * PMOUSE_REQUEST;
typedef struct _ba {
	INT32 security;
	INT32 process_id;
	ULONGLONG* address;
} ba, * pba;
struct comms_t {
	std::uint32_t key;

	struct {
		void* handle;
	}window;
};
typedef struct _hide {
	comms_t* a;
} hide, * hidea;
typedef struct _ga {
	INT32 security;
	ULONGLONG* address;
} ga, * pga;
typedef struct _mu {
	float y;
	float x;
} mu, * mua;
typedef struct _spoof {
	float y;
	float x;
} spoof, * spoofa;
typedef struct _SYSTEM_BIGPOOL_ENTRY
{
	union {
		PVOID VirtualAddress;
		ULONG_PTR NonPaged : 1;
	};
	ULONG_PTR SizeInBytes;
	union {
		UCHAR Tag[4];
		ULONG TagUlong;
	};
} SYSTEM_BIGPOOL_ENTRY, * PSYSTEM_BIGPOOL_ENTRY;

typedef struct _SYSTEM_BIGPOOL_INFORMATION {
	ULONG Count;
	SYSTEM_BIGPOOL_ENTRY AllocatedInfo[ANYSIZE_ARRAY];
} SYSTEM_BIGPOOL_INFORMATION, * PSYSTEM_BIGPOOL_INFORMATION;




namespace mem {
	HANDLE driver_handle;
	INT32 process_id;

	bool find_driver() {


		driver_handle = CreateFileW(L"\\\\.\\\ASUSTeK", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

		if (!driver_handle || (driver_handle == INVALID_HANDLE_VALUE))
			return false;

		return true;
	}

	void read_physical(PVOID address, PVOID buffer, DWORD size) {
		_readwrite arguments = { 0 };

		arguments.security = code_security;
		arguments.address = (ULONGLONG)address - 11;
		arguments.buffer = (ULONGLONG)buffer - 11;
		arguments.size = size - 11;
		arguments.process_id = process_id - 11;
		arguments.write = FALSE;

		DeviceIoControl(driver_handle, code_rw, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}

	void write_physical(PVOID address, PVOID buffer, DWORD size) {
		_readwrite arguments = { 0 };

		arguments.security = code_security;
		arguments.address = (ULONGLONG)address - 11;
		arguments.buffer = (ULONGLONG)buffer - 11;
		arguments.size = size - 11;
		arguments.process_id = process_id - 11;
		arguments.write = TRUE;

		DeviceIoControl(driver_handle, code_rw, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
	}
	void lclick_down() {
		uintptr_t image_address = { NULL };
		_mu arguments = { NULL };



		DeviceIoControl(driver_handle, code_move_lclick_down, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);


	}
	void lclick_up() {
		uintptr_t image_address = { NULL };
		_mu arguments = { NULL };



		DeviceIoControl(driver_handle, code_move_lclick_up, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);


	}

	void move(float x, float y) {
		uintptr_t image_address = { NULL };
		_mu arguments = { NULL };


		arguments.x = x - 11;
		arguments.y = y - 11;

		DeviceIoControl(driver_handle, code_move, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);


	}
	uintptr_t find_image() {
		uintptr_t image_address = { NULL };
		_ba arguments = { NULL };

		arguments.security = code_security;
		arguments.process_id = process_id - 11;
		arguments.address = (ULONGLONG*)&image_address;

		DeviceIoControl(driver_handle, code_ba, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

		return image_address;
	}
	uintptr_t spoofer() {
		uintptr_t image_address = { NULL };
		_ba arguments = { NULL };



		DeviceIoControl(driver_handle, code_spoofer, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

		return image_address;
	}

	INT32 find_process(LPCTSTR process_name) {
		PROCESSENTRY32 pt;
		HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pt.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hsnap, &pt)) {
			do {
				if (!lstrcmpi(pt.szExeFile, process_name)) {
					CloseHandle(hsnap);
					process_id = pt.th32ProcessID;
					return pt.th32ProcessID;
				}
			} while (Process32Next(hsnap, &pt));
		}
		CloseHandle(hsnap);
		return process_id;
	}

}

uintptr_t guard;

bool zero_check(uintptr_t pointer)
{
	constexpr uintptr_t filter = 0xFFFF0000FFFFFF00;
	uintptr_t result = pointer & filter;
	return result == 0x0000000000000000;
}

bool extras_check(uintptr_t pointer)
{
	constexpr uintptr_t filter = 0xFFFF000000000000;
	uintptr_t result = pointer & filter;
	return result == 0x0000000000000000;
}

bool is_valid(uintptr_t pointer)
{
	if (!pointer)
		return false;

	if (zero_check(pointer))
		return false;


	return true;
}

bool is_guarded_2(uintptr_t pointer)
{

	if (zero_check(pointer))
	{
		return false;
	}

	constexpr uintptr_t filter = 0xFFFFFFFFFF000000;
	uintptr_t result = pointer & filter;
	return result == 0x0000000000000000;
}

bool is_guarded1(uintptr_t pointer)
{
	constexpr uintptr_t filter = 0xFFFFFFFF00000000;
	uintptr_t result = pointer & filter;
	return result == 0x8000000000 || result == 0x10000000000;
}

uint64_t validate_pointer(uint64_t address)
{
	if (is_guarded1(address))
		return guard + (address & 0xFFFFFF);
	else
		return address;
}

template <typename T>
T read2(uint64_t address) {
	T buffer{ };
	mem::read_physical((PVOID)address, &buffer, sizeof(T));
	if (is_guarded1(buffer))
	{
		buffer = validate_pointer(buffer);
	}

	return buffer;
}




template <typename T>
T read(uint64_t address) {
	T buffer{ };
	mem::read_physical((PVOID)address, &buffer, sizeof(T));

	return buffer;
}

template <typename T>
T write(uint64_t address, T buffer) {

	//mem::write_physical((PVOID)address, &buffer, sizeof(T));
	return buffer;
}

__forceinline auto query_bigpools() -> PSYSTEM_BIGPOOL_INFORMATION
{

	DWORD length = 0;
	DWORD size = 0;
	LPVOID heap = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 0);
	heap = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, heap, 0xFF);
	NTSTATUS ntLastStatus = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)0x42, heap, 0x30, &length);
	heap = HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, heap, length + 0x1F);
	size = length;
	ntLastStatus = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)0x42, heap, size, &length);

	return reinterpret_cast<PSYSTEM_BIGPOOL_INFORMATION>(heap);
}

__forceinline auto retrieve_guarded() -> uintptr_t
{
	auto pool_information = query_bigpools();
	uintptr_t guarded = 0;

	if (pool_information)
	{
		auto count = pool_information->Count;
		for (auto i = 0ul; i < count; i++)
		{
			SYSTEM_BIGPOOL_ENTRY* allocation_entry = &pool_information->AllocatedInfo[i];
			const auto virtual_address = (PVOID)((uintptr_t)allocation_entry->VirtualAddress & ~1ull);

			if (allocation_entry->TagUlong == 'TnoC') {
				auto world = read<uintptr_t>(reinterpret_cast<uintptr_t>(virtual_address) + 0x60);
				if (world) {
					auto world1 = reinterpret_cast<uintptr_t>(virtual_address) + (world & 0xFFFFFF);
					if (allocation_entry->NonPaged && allocation_entry->SizeInBytes == 0x200000) {
						guarded = reinterpret_cast<uintptr_t>(virtual_address);
						printf("DRV bas minne %p\n", world1 - reinterpret_cast<uintptr_t>(virtual_address));
					}

						
					if (world1) {
						if (world1 - reinterpret_cast<uintptr_t>(virtual_address) == 0x1BBC0) {
							guarded = reinterpret_cast<uintptr_t>(virtual_address);

						}
					}

				}
			}


		}
	}

	return guarded;
}
