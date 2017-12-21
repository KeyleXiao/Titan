/*
 * Memory DLL loading code
 * Version 0.0.4.1
 *
 * Copyright (c) 2004-2014 by Joachim Bauch / mail@joachim-bauch.de
 * http://www.joachim-bauch.de
 *
 * Updated By RLib
 *   [1/23/2015 ASUS]
 *
 */

#ifndef __MEMORY_MODULE_HEADER
#define __MEMORY_MODULE_HEADER

namespace rkt {

#include <windows.h>
#include <winnt.h>
#include <tchar.h>

#include "Common.h"

typedef BOOL(WINAPI *DllEntryProc)(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved);
typedef int (WINAPI *ExeEntryProc)(void);

typedef void *HMEMORYRSRC;

typedef void *HCUSTOMMODULE;

typedef HCUSTOMMODULE(*CustomLoadLibraryFunc)(LPCSTR, void *);
typedef FARPROC(*CustomGetProcAddressFunc)(HCUSTOMMODULE, LPCSTR, void *);
typedef void(*CustomFreeLibraryFunc)(HCUSTOMMODULE, void *);

typedef struct {
	PIMAGE_NT_HEADERS headers;
	unsigned char *codeBase;
	HCUSTOMMODULE *modules;
	int numModules;
	int initialized;
	int isDLL;
	int isRelocated;
	CustomLoadLibraryFunc loadLibrary;
	CustomGetProcAddressFunc getProcAddress;
	CustomFreeLibraryFunc freeLibrary;
	void *userdata;
	ExeEntryProc exeEntry;
} MEMORYMODULE, *PMEMORYMODULE;

typedef MEMORYMODULE *HMEMORYMODULE;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Load EXE/DLL from memory location.
 *
 * All dependencies are resolved using default LoadLibrary/GetProcAddress
 * calls through the Windows API.
 */
RKT_API HMEMORYMODULE MemoryLoadLibrary(const void *);

/**
 * Load EXE/DLL from memory location using custom dependency resolvers.
 *
 * Dependencies will be resolved using passed callback methods.
 */
RKT_API HMEMORYMODULE MemoryLoadLibraryEx(const void *,
    CustomLoadLibraryFunc,
    CustomGetProcAddressFunc,
    CustomFreeLibraryFunc,
    void *);

/**
 * Get address of exported method.
 */
RKT_API FARPROC MemoryGetProcAddress(HMEMORYMODULE, LPCSTR);

/**
 * Free previously loaded EXE/DLL.
 */
RKT_API void MemoryFreeLibrary(HMEMORYMODULE);

/**
 * Execute entry point (EXE only). The entry point can only be executed
 * if the EXE has been loaded to the correct base address or it could
 * be relocated (i.e. relocation information have not been stripped by
 * the linker).
 *
 * Important: calling this function will not return, i.e. once the loaded
 * EXE finished running, the process will terminate.
 *
 * Returns a negative value if the entry point could not be executed.
 */
RKT_API int MemoryCallEntryPoint(HMEMORYMODULE);

/**
 * Find the location of a resource with the specified type and name.
 */
RKT_API HMEMORYRSRC MemoryFindResource(HMEMORYMODULE, LPCTSTR, LPCTSTR);

/**
 * Find the location of a resource with the specified type, name and language.
 */
RKT_API HMEMORYRSRC MemoryFindResourceEx(HMEMORYMODULE, LPCTSTR, LPCTSTR, WORD);

/**
 * Get the size of the resource in bytes.
 */
RKT_API DWORD MemorySizeofResource(HMEMORYMODULE, HMEMORYRSRC);

/**
 * Get a pointer to the contents of the resource.
 */
RKT_API LPVOID MemoryLoadResource(HMEMORYMODULE, HMEMORYRSRC);

/**
 * Load a string resource.
 */
RKT_API int MemoryLoadString(HMEMORYMODULE, UINT, LPTSTR, int);

/**
 * Load a string resource with a given language.
 */
RKT_API int MemoryLoadStringEx(HMEMORYMODULE, UINT, LPTSTR, int, WORD);

#ifdef __cplusplus
}
#endif

} // namespace rkt

#endif  // __MEMORY_MODULE_HEADER
