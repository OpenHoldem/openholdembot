#pragma once
#include <windows.h>
#include <dwmapi.h>
#include <vector>
#include <string>

// Make sure to link dwmapi.lib!

namespace robmikh::common::desktop
{
    struct WindowInfo
    {
        WindowInfo(HWND windowHandle)
        {
            WindowHandle = windowHandle;
            auto titleLength = GetWindowTextLengthW(WindowHandle);
            if (titleLength > 0)
            {
                titleLength++;
            }
            std::wstring title(titleLength, 0);
            GetWindowTextW(WindowHandle, title.data(), titleLength);
            Title = title;
            auto classNameLength = 256;
            std::wstring className(classNameLength, 0);
            GetClassNameW(WindowHandle, className.data(), classNameLength);
            ClassName = className;
            RECT rect = {};
            winrt::check_bool(GetWindowRect(WindowHandle, &rect));
            Position = { (float)rect.left, (float)rect.top };
            Size = { (float)(rect.right - rect.left), (float)(rect.bottom - rect.top) };
        }

        HWND WindowHandle;
        std::wstring Title;
        std::wstring ClassName;
        winrt::Windows::Foundation::Numerics::float2 Position;
        winrt::Windows::Foundation::Numerics::float2 Size;

        bool operator==(const WindowInfo& info) { return WindowHandle == info.WindowHandle; }
        bool operator!=(const WindowInfo& info) { return !(*this == info); }
    };

    namespace impl
    {
        inline bool MatchTitleAndClassName(WindowInfo const& window, std::wstring const& title, std::wstring const& className)
        {
            return wcscmp(window.Title.c_str(), title.c_str()) == 0 &&
                wcscmp(window.ClassName.c_str(), className.c_str()) == 0;
        }

        inline bool IsKnownBlockedWindow(WindowInfo const& window)
        {
            return
                // Task View
                MatchTitleAndClassName(window, L"Task View", L"Windows.UI.Core.CoreWindow") ||
                // XAML Islands
                MatchTitleAndClassName(window, L"DesktopWindowXamlSource", L"Windows.UI.Core.CoreWindow") ||
                // XAML Popups
                MatchTitleAndClassName(window, L"PopupHost", L"Xaml_WindowedPopupClass");
        }
    }

    inline std::vector<WindowInfo> FindTopLevelWindowsByTitle(std::wstring const& query)
    {
        struct EnumState
        {
            std::wstring WindowQuery;
            HWND CurrentWindow;
            std::vector<WindowInfo> Windows;
        };

        // TODO: This doesn't work for terminal, replace this with changing the
        //       window title to a GUID or something and then changing it back.
        auto currentWindow = GetConsoleWindow();

        auto enumState = EnumState{ query, currentWindow, std::vector<WindowInfo>() };
        EnumWindows([](HWND hwnd, LPARAM lParam)
            {
                if (GetWindowTextLengthW(hwnd) > 0)
                {
                    auto& enumState = *reinterpret_cast<EnumState*>(lParam);

                    if (enumState.CurrentWindow != nullptr && hwnd == enumState.CurrentWindow)
                    {
                        return TRUE;
                    }

                    if (GetAncestor(hwnd, GA_ROOT) != hwnd || hwnd == GetShellWindow() || !IsWindowVisible(hwnd))
                    {
                        return TRUE;
                    }

                    auto exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
                    if (exStyle & WS_EX_TOOLWINDOW)    // No tooltips
                    {
                        return TRUE;
                    }

                    auto window = WindowInfo(hwnd);

                    auto index = window.Title.find(enumState.WindowQuery.c_str());
                    if (index == std::wstring::npos)
                    {
                        return TRUE;
                    }

                    // Check to see if the window is cloaked if it's a UWP
                    if (wcscmp(window.ClassName.c_str(), L"Windows.UI.Core.CoreWindow") == 0 ||
                        wcscmp(window.ClassName.c_str(), L"ApplicationFrameWindow") == 0)
                    {
                        DWORD cloaked = FALSE;
                        if (SUCCEEDED(DwmGetWindowAttribute(window.WindowHandle, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && (cloaked == DWM_CLOAKED_SHELL))
                        {
                            return TRUE;
                        }
                    }

                    // Unfortunate work-around. Not sure how to avoid this.
                    if (impl::IsKnownBlockedWindow(window))
                    {
                        return TRUE;
                    }

                    enumState.Windows.push_back(window);
                }

                return TRUE;
            }, reinterpret_cast<LPARAM>(&enumState));

        return enumState.Windows;
    }
}
