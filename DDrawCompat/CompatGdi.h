#pragma once

#define WIN32_LEAN_AND_MEAN

#include <unordered_map>

#include <Windows.h>

#include "DDrawLog.h"

#define CALL_ORIG_GDI(func) CompatGdi::getOrigFuncPtr<decltype(&func), &func>()

#define HOOK_GDI_FUNCTION(module, func, newFunc) \
	CompatGdi::hookGdiFunction<decltype(&func), &func>(#module, #func, &newFunc)

namespace CompatGdi
{
	class GdiScopedThreadLock
	{
	public:
		GdiScopedThreadLock();
		~GdiScopedThreadLock();
		void unlock();
	private:
		bool m_isLocked;
	};

	extern CRITICAL_SECTION g_gdiCriticalSection;
	extern std::unordered_map<void*, const char*> g_funcNames;

	bool beginGdiRendering();
	void endGdiRendering();

	template <typename OrigFuncPtr, OrigFuncPtr origFunc>
	OrigFuncPtr& getOrigFuncPtr()
	{
		static OrigFuncPtr origFuncPtr = origFunc;
		return origFuncPtr;
	}

	void hookGdiFunction(const char* moduleName, const char* funcName, void*& origFuncPtr, void* newFuncPtr);

	template <typename OrigFuncPtr, OrigFuncPtr origFunc>
	void hookGdiFunction(const char* moduleName, const char* funcName, OrigFuncPtr newFuncPtr)
	{
		hookGdiFunction(moduleName, funcName,
			reinterpret_cast<void*&>(getOrigFuncPtr<OrigFuncPtr, origFunc>()), newFuncPtr);
	}

	void installHooks();
	void invalidate(const RECT* rect);
	void updatePalette();
};
