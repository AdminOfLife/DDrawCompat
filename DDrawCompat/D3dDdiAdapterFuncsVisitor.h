#pragma once

#define CINTERFACE

#include <d3d.h>
#include <d3dumddi.h>

#include "DDrawVtableVisitor.h"

struct D3dDdiAdapterFuncsIntf
{
	D3DDDI_ADAPTERFUNCS* lpVtbl;
};

template <>
struct DDrawVtableForEach<D3DDDI_ADAPTERFUNCS>
{
	template <typename Vtable, typename Visitor>
	static void forEach(Visitor& visitor)
	{
		DD_VISIT(pfnGetCaps);
		DD_VISIT(pfnCreateDevice);
		DD_VISIT(pfnCloseAdapter);
	}
};
