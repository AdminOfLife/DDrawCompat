#pragma once

#define CINTERFACE
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <Unknwnbase.h>

#include "CompatWeakPtr.h"

namespace CompatActivateAppHandler
{
	void installHooks();
	bool isActive();
	void setFullScreenCooperativeLevel(CompatWeakPtr<IUnknown> dd, HWND hwnd, DWORD flags);
	void uninstallHooks();
}
