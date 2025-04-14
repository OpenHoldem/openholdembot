#pragma once
#include <winrt/Windows.Graphics.h>
#include <windows.graphics.interop.h>
#include <d2d1_1.h>

namespace robmikh::common::uwp
{
    struct GeometrySource : winrt::implements<
        GeometrySource, 
        winrt::Windows::Graphics::IGeometrySource2D, 
        ABI::Windows::Graphics::IGeometrySource2DInterop>
    {
        GeometrySource(winrt::com_ptr<ID2D1Geometry> const& geometry) :
            m_geometry(geometry)
        { }

        IFACEMETHODIMP GetGeometry(ID2D1Geometry** value) override
        {
            m_geometry.copy_to(value);
            return S_OK;
        }

        IFACEMETHODIMP TryGetGeometryUsingFactory(ID2D1Factory*, ID2D1Geometry** result) override
        {
            *result = nullptr;
            return E_NOTIMPL;
        }

    private:
        winrt::com_ptr<ID2D1Geometry> m_geometry;
    };
}