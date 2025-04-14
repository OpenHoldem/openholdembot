#pragma once
#include <cstdint>
#include <Windows.h>
#include <CommCtrl.h>
#include <winrt/base.h>

namespace robmikh::common::desktop::controls
{
    enum class ControlType
    {
        Label,
        ComboBox,
        Button,
        CheckBox,
        Edit,
    };

    struct StackPanel
    {
        StackPanel(HWND parentWindow, HINSTANCE instance, uint32_t marginX, uint32_t marginY, uint32_t stepAmount, uint32_t width, uint32_t height)
        {
            m_parentWindow = parentWindow;
            m_instance = instance;
            m_offsetX = marginX;
            m_width = width;
            m_height = height;
            m_currentOffsetY = Stepper{ marginY, stepAmount };
        }

        HWND CreateControl(ControlType controlType, LPCTSTR windowName)
        {
            return CreateControl(controlType, windowName, 0);
        };

        HWND CreateControl(ControlType controlType, LPCTSTR windowName, DWORD style)
        {
            return CreateControlWindow(GetControlClassName(controlType), windowName,
                style | GetControlStyle(controlType),
                GetOffsetY(controlType, m_currentOffsetY));
        };

    private:
        struct Stepper
        {
            uint32_t Value;
            uint32_t StepAmount;
            uint32_t Step()
            {
                return StepCustom(StepAmount);
            }
            uint32_t StepCustom(uint32_t stepAmount)
            {
                auto oldValue = Value;
                Value += stepAmount;
                return oldValue;
            }
        };

        HWND CreateControlWindow(LPCWSTR className, LPCTSTR windowName, DWORD style, uint32_t offsetY)
        {
            return winrt::check_pointer(CreateWindowExW(0,className, (LPCWSTR)windowName,
                style | m_commonStyle,
                m_offsetX, offsetY, m_width, m_height, m_parentWindow, nullptr, m_instance, nullptr));
        };

        DWORD GetControlStyle(ControlType controlType)
        {
            switch (controlType)
            {
            case ControlType::Label:
                return 0;
            case ControlType::ComboBox:
                return WS_TABSTOP | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_VSCROLL;
            case ControlType::Button:
                return WS_TABSTOP | BS_DEFPUSHBUTTON;
            case ControlType::CheckBox:
                return WS_TABSTOP | BS_AUTOCHECKBOX;
            case ControlType::Edit:
                return WS_TABSTOP | WS_BORDER | ES_LEFT;
            default:
                return 0;
            }
        };

        LPCWSTR GetControlClassName(ControlType controlType)
        {
            switch (controlType)
            {
            case ControlType::Label:
                return (LPCWSTR)WC_STATIC;
            case ControlType::ComboBox:
                return (LPCWSTR)WC_COMBOBOX;
            case ControlType::Button:
                return (LPCWSTR)WC_BUTTON;
            case ControlType::CheckBox:
                return (LPCWSTR)WC_BUTTON;
            case ControlType::Edit:
                return L"EDIT";
            default:
                return 0;
            }
        };

        uint32_t GetOffsetY(ControlType controlType, Stepper& currentOffsetY)
        {
            switch (controlType)
            {
            case ControlType::Label:
                return currentOffsetY.StepCustom(20);
            case ControlType::ComboBox:
                return currentOffsetY.Step();
            case ControlType::Button:
                return currentOffsetY.Step();
            case ControlType::CheckBox:
                return currentOffsetY.Step();
            case ControlType::Edit:
                return currentOffsetY.Step();
            default:
                return 0;
            }
        };

    private:
        HWND m_parentWindow = nullptr;
        HINSTANCE m_instance = nullptr;
        Stepper m_currentOffsetY;
        DWORD m_commonStyle = WS_CHILD | WS_OVERLAPPED | WS_VISIBLE;
        uint32_t m_offsetX = 10;
        uint32_t m_width = 200;
        uint32_t m_height = 30;
    };

    inline HGDIOBJ SetBackgroundColorToWindowColor(HDC staticColorHdc)
    {
        auto color = static_cast<COLORREF>(GetSysColor(COLOR_WINDOW));
        SetBkColor(staticColorHdc, color);
        SetDCBrushColor(staticColorHdc, color);
        return GetStockObject(DC_BRUSH);
    }

    inline LRESULT StaticControlColorMessageHandler(WPARAM const wparam, LPARAM const)
    {
        return reinterpret_cast<LRESULT>(robmikh::common::desktop::controls::SetBackgroundColorToWindowColor(reinterpret_cast<HDC>(wparam)));
    }
}
