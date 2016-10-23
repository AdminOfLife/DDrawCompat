#pragma once

#include "CompatVtable.h"
#include "D3dDdiDeviceCallbacksVisitor.h"

std::ostream& operator<<(std::ostream& os, const D3DDDI_ALLOCATIONINFO& data);
std::ostream& operator<<(std::ostream& os, const D3DDDICB_ALLOCATE& data);
std::ostream& operator<<(std::ostream& os, const D3DDDICB_DEALLOCATE& data);
std::ostream& operator<<(std::ostream& os, const D3DDDICB_DEALLOCATE2& data);
std::ostream& operator<<(std::ostream& os, const D3DDDICB_LOCK& data);
std::ostream& operator<<(std::ostream& os, const D3DDDICB_LOCK2& data);
std::ostream& operator<<(std::ostream& os, const D3DDDICB_UNLOCK& data);
std::ostream& operator<<(std::ostream& os, const D3DDDICB_UNLOCK2& data);

namespace D3dDdi
{
	class DeviceCallbacks : public CompatVtable<DeviceCallbacks, D3dDdiDeviceCallbacksIntf>
	{
	public:
		static void setCompatVtable(D3DDDI_DEVICECALLBACKS& vtable);
	};
}
