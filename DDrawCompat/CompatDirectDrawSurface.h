#pragma once

#include <type_traits>

#include "CompatRef.h"
#include "CompatVtable.h"
#include "DDrawTypes.h"
#include "DirectDrawSurfaceVtblVisitor.h"

template <typename TSurface>
class CompatDirectDrawSurface : public CompatVtable<CompatDirectDrawSurface<TSurface>, TSurface>
{
public:
	typedef typename Types<TSurface>::TSurfaceDesc TSurfaceDesc;
	typedef typename Types<TSurface>::TDdsCaps TDdsCaps;
	typedef typename Types<TSurface>::TUnlockParam TUnlockParam;

	static void setCompatVtable(Vtable<TSurface>& vtable);

	template <typename TDirectDraw>
	static HRESULT createCompatPrimarySurface(
		CompatRef<TDirectDraw> dd,
		TSurfaceDesc compatDesc,
		TSurface*& compatSurface);

	static void fixSurfacePtrs(CompatRef<TSurface> surface);

	static HRESULT STDMETHODCALLTYPE Blt(
		TSurface* This,
		LPRECT lpDestRect,
		TSurface* lpDDSrcSurface,
		LPRECT lpSrcRect,
		DWORD dwFlags,
		LPDDBLTFX lpDDBltFx);

	static HRESULT STDMETHODCALLTYPE BltFast(
		TSurface* This,
		DWORD dwX,
		DWORD dwY,
		TSurface* lpDDSrcSurface,
		LPRECT lpSrcRect,
		DWORD dwTrans);

	static HRESULT STDMETHODCALLTYPE Flip(
		TSurface* This,
		TSurface* lpDDSurfaceTargetOverride,
		DWORD dwFlags);

	static HRESULT STDMETHODCALLTYPE GetCaps(TSurface* This, TDdsCaps* lpDDSCaps);
	static HRESULT STDMETHODCALLTYPE GetSurfaceDesc(TSurface* This, TSurfaceDesc* lpDDSurfaceDesc);
	static HRESULT STDMETHODCALLTYPE IsLost(TSurface* This);

	static HRESULT STDMETHODCALLTYPE Lock(
		TSurface* This,
		LPRECT lpDestRect,
		TSurfaceDesc* lpDDSurfaceDesc,
		DWORD dwFlags,
		HANDLE hEvent);
	
	static HRESULT STDMETHODCALLTYPE QueryInterface(TSurface* This, REFIID riid, LPVOID* obp);
	static HRESULT STDMETHODCALLTYPE ReleaseDC(TSurface* This, HDC hDC);
	static HRESULT STDMETHODCALLTYPE Restore(TSurface* This);
	static HRESULT STDMETHODCALLTYPE SetClipper(TSurface* This, LPDIRECTDRAWCLIPPER lpDDClipper);
	static HRESULT STDMETHODCALLTYPE SetPalette(TSurface* This, LPDIRECTDRAWPALETTE lpDDPalette);
	static HRESULT STDMETHODCALLTYPE Unlock(TSurface* This, TUnlockParam lpRect);

	static const IID& s_iid;

private:
	static void restorePrimaryCaps(TDdsCaps& caps);
};

namespace Compat
{
	template <typename Intf>
	struct IsDirectDrawSurfaceIntf : std::false_type {};

	template<> struct IsDirectDrawSurfaceIntf<IDirectDrawSurface> : std::true_type {};
	template<> struct IsDirectDrawSurfaceIntf<IDirectDrawSurface2> : std::true_type {};
	template<> struct IsDirectDrawSurfaceIntf<IDirectDrawSurface3> : std::true_type {};
	template<> struct IsDirectDrawSurfaceIntf<IDirectDrawSurface4> : std::true_type {};
	template<> struct IsDirectDrawSurfaceIntf<IDirectDrawSurface7> : std::true_type {};

	template <typename NewIntf, typename OrigIntf>
	std::enable_if_t<IsDirectDrawSurfaceIntf<NewIntf>::value && IsDirectDrawSurfaceIntf<OrigIntf>::value>
		queryInterface(OrigIntf& origIntf, NewIntf*& newIntf)
	{
		CompatDirectDrawSurface<OrigIntf>::s_origVtable.QueryInterface(
			&origIntf, CompatDirectDrawSurface<NewIntf>::s_iid, reinterpret_cast<void**>(&newIntf));
	}

	template <typename NewIntf>
	std::enable_if_t<IsDirectDrawSurfaceIntf<NewIntf>::value>
		queryInterface(IUnknown& origIntf, NewIntf*& newIntf)
	{
		CompatDirectDrawSurface<IDirectDrawSurface>::s_origVtable.QueryInterface(
			reinterpret_cast<IDirectDrawSurface*>(&origIntf),
			CompatDirectDrawSurface<NewIntf>::s_iid, reinterpret_cast<void**>(&newIntf));
	}

	template <typename OrigIntf>
	std::enable_if_t<IsDirectDrawSurfaceIntf<OrigIntf>::value>
		queryInterface(OrigIntf& origIntf, IUnknown*& newIntf)
	{
		CompatDirectDrawSurface<OrigIntf>::s_origVtable.QueryInterface(
			&origIntf, IID_IUnknown, reinterpret_cast<void**>(&newIntf));
	}
}
