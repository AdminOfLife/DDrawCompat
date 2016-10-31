#define WIN32_LEAN_AND_MEAN

#include <algorithm>
#include <map>
#include <utility>

#include <Windows.h>
#include <detours.h>

#include "Common/Hook.h"
#include "Common/Log.h"

namespace
{
	struct HookedFunctionInfo
	{
		HMODULE module;
		void* trampoline;
		void* newFunction;
	};

	std::map<void*, HookedFunctionInfo> g_hookedFunctions;

	std::map<void*, HookedFunctionInfo>::iterator findOrigFunc(void* origFunc)
	{
		return std::find_if(g_hookedFunctions.begin(), g_hookedFunctions.end(),
			[=](const auto& i) { return origFunc == i.first || origFunc == i.second.trampoline; });
	}

	FARPROC getProcAddress(HMODULE module, const char* procName)
	{
		if (!module || !procName)
		{
			return nullptr;
		}

		PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
		if (IMAGE_DOS_SIGNATURE != dosHeader->e_magic) {
			return nullptr;
		}
		char* moduleBase = reinterpret_cast<char*>(module);

		PIMAGE_NT_HEADERS ntHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
			reinterpret_cast<char*>(dosHeader) + dosHeader->e_lfanew);
		if (IMAGE_NT_SIGNATURE != ntHeader->Signature)
		{
			return nullptr;
		}

		PIMAGE_EXPORT_DIRECTORY exportDir = reinterpret_cast<PIMAGE_EXPORT_DIRECTORY>(
			moduleBase + ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

		DWORD* rvaOfNames = reinterpret_cast<DWORD*>(moduleBase + exportDir->AddressOfNames);

		for (DWORD i = 0; i < exportDir->NumberOfNames; ++i)
		{
			if (0 == strcmp(procName, moduleBase + rvaOfNames[i]))
			{
				WORD* nameOrds = reinterpret_cast<WORD*>(moduleBase + exportDir->AddressOfNameOrdinals);
				DWORD* rvaOfFunctions = reinterpret_cast<DWORD*>(moduleBase + exportDir->AddressOfFunctions);
				return reinterpret_cast<FARPROC>(moduleBase + rvaOfFunctions[nameOrds[i]]);
			}
		}

		return nullptr;
	}

	void hookFunction(const char* funcName, void*& origFuncPtr, void* newFuncPtr)
	{
		const auto it = findOrigFunc(origFuncPtr);
		if (it != g_hookedFunctions.end())
		{
			origFuncPtr = it->second.trampoline;
			return;
		}

		void* const hookedFuncPtr = origFuncPtr;

		DetourTransactionBegin();
		const bool attachSuccessful = NO_ERROR == DetourAttach(&origFuncPtr, newFuncPtr);
		const bool commitSuccessful = NO_ERROR == DetourTransactionCommit();
		if (!attachSuccessful || !commitSuccessful)
		{
			if (funcName)
			{
				Compat::Log() << "Failed to hook a function: " << funcName;
			}
			else
			{
				Compat::Log() << "Failed to hook a function: " << origFuncPtr;
			}
			return;
		}

		HMODULE module = nullptr;
		GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			reinterpret_cast<char*>(hookedFuncPtr), &module);
		g_hookedFunctions[hookedFuncPtr] = { module, origFuncPtr, newFuncPtr };
	}

	void unhookFunction(const std::map<void*, HookedFunctionInfo>::iterator& hookedFunc)
	{
		DetourTransactionBegin();
		DetourDetach(&hookedFunc->second.trampoline, hookedFunc->second.newFunction);
		DetourTransactionCommit();

		if (hookedFunc->second.module)
		{
			FreeLibrary(hookedFunc->second.module);
		}
		g_hookedFunctions.erase(hookedFunc);
	}
}

namespace Compat
{
	void hookFunction(void*& origFuncPtr, void* newFuncPtr)
	{
		::hookFunction(nullptr, origFuncPtr, newFuncPtr);
	}

	void hookFunction(HMODULE module, const char* funcName, void*& origFuncPtr, void* newFuncPtr)
	{
		FARPROC procAddr = getProcAddress(module, funcName);
		if (!procAddr)
		{
			Compat::LogDebug() << "Failed to load the address of a function: " << funcName;
			return;
		}

		origFuncPtr = procAddr;
		::hookFunction(funcName, origFuncPtr, newFuncPtr);
	}

	void hookFunction(const char* moduleName, const char* funcName, void*& origFuncPtr, void* newFuncPtr)
	{
		HMODULE module = LoadLibrary(moduleName);
		if (!module)
		{
			return;
		}
		hookFunction(module, funcName, origFuncPtr, newFuncPtr);
		FreeLibrary(module);
	}

	void unhookAllFunctions()
	{
		while (!g_hookedFunctions.empty())
		{
			::unhookFunction(g_hookedFunctions.begin());
		}
	}

	void unhookFunction(void* origFunc)
	{
		auto it = findOrigFunc(origFunc);
		if (it != g_hookedFunctions.end())
		{
			::unhookFunction(it);
		}
	}
}
