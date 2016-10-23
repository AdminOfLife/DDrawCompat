#include <cstring>
#include <deque>

#include "Config.h"
#include "DDraw/CompatPrimarySurface.h"
#include "DDraw/DirectDrawPalette.h"
#include "DDraw/RealPrimarySurface.h"
#include "Time.h"

namespace DDraw
{
	void DirectDrawPalette::setCompatVtable(IDirectDrawPaletteVtbl& vtable)
	{
		vtable.SetEntries = &SetEntries;
	}

	HRESULT STDMETHODCALLTYPE DirectDrawPalette::SetEntries(
		IDirectDrawPalette* This,
		DWORD dwFlags,
		DWORD dwStartingEntry,
		DWORD dwCount,
		LPPALETTEENTRY lpEntries)
	{
		if (This == CompatPrimarySurface::g_palette)
		{
			waitForNextUpdate();
			if (lpEntries && dwStartingEntry + dwCount <= 256 &&
				0 == std::memcmp(&CompatPrimarySurface::g_paletteEntries[dwStartingEntry],
					lpEntries, dwCount * sizeof(PALETTEENTRY)))
			{
				return DD_OK;
			}
		}

		HRESULT result = s_origVtable.SetEntries(This, dwFlags, dwStartingEntry, dwCount, lpEntries);
		if (This == CompatPrimarySurface::g_palette && SUCCEEDED(result))
		{
			std::memcpy(&CompatPrimarySurface::g_paletteEntries[dwStartingEntry], lpEntries,
				dwCount * sizeof(PALETTEENTRY));
			RealPrimarySurface::updatePalette(dwStartingEntry, dwCount);
		}
		return result;
	}

	void DirectDrawPalette::waitForNextUpdate()
	{
		static std::deque<long long> updatesInLastMs;

		const long long qpcNow = Time::queryPerformanceCounter();
		const long long qpcLastMsBegin = qpcNow - Time::g_qpcFrequency / 1000;
		while (!updatesInLastMs.empty() && qpcLastMsBegin - updatesInLastMs.front() > 0)
		{
			updatesInLastMs.pop_front();
		}

		if (updatesInLastMs.size() >= Config::maxPaletteUpdatesPerMs)
		{
			Sleep(1);
			updatesInLastMs.clear();
		}

		updatesInLastMs.push_back(Time::queryPerformanceCounter());
	}
}
