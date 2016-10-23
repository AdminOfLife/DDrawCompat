#pragma once

#include "Common/CompatVtable.h"
#include "D3dDdiDeviceFuncsVisitor.h"

std::ostream& operator<<(std::ostream& os, const D3DDDI_RATIONAL& val);
std::ostream& operator<<(std::ostream& os, const D3DDDI_SURFACEINFO& val);
std::ostream& operator<<(std::ostream& os, const D3DDDIARG_CREATERESOURCE& val);
std::ostream& operator<<(std::ostream& os, const D3DDDIARG_CREATERESOURCE2& val);
std::ostream& operator<<(std::ostream& os, const D3DDDIARG_LOCK& val);
std::ostream& operator<<(std::ostream& os, const D3DDDIARG_OPENRESOURCE& val);
std::ostream& operator<<(std::ostream& os, const D3DDDIARG_UNLOCK& val);
std::ostream& operator<<(std::ostream& os, const D3DDDIBOX& val);

namespace D3dDdi
{
	class DeviceFuncs : public CompatVtable<DeviceFuncs, D3dDdiDeviceFuncsIntf>
	{
	public:
		static void setCompatVtable(D3DDDI_DEVICEFUNCS& vtable);
	};
}
