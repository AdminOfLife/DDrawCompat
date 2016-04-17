#pragma once

typedef unsigned long DWORD;

namespace Config
{
	const int maxPrimaryUpdateRate = 60;
	const int primaryUpdateDelayAfterFlip = 100;
	const DWORD preallocatedGdiDcCount = 4;
}
