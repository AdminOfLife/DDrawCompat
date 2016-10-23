#pragma once

#include "Common/CompatVtable.h"
#include "D3dDdi/Visitors/AdapterFuncsVisitor.h"

namespace D3dDdi
{
	class AdapterFuncs : public CompatVtable<AdapterFuncs, AdapterFuncsIntf>
	{
	public:
		static void setCompatVtable(D3DDDI_ADAPTERFUNCS& vtable);
	};
}
