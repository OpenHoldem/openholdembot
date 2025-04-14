#include <winrt/base.h>

#include <mfobjects.h>
#include <mferror.h>
#include <mfapi.h>

#include <wil/resource.h>
#include <wil/com.h>

#include <string>
#include <optional>

namespace robmikh::common::uwp
{
    struct MediaBufferGuard
    {
        struct BufferInfo
        {
            byte* Bits;
            DWORD MaxLength;
            DWORD CurrentLength;
        };

        MediaBufferGuard(winrt::com_ptr<IMFMediaBuffer> const& buffer)
        {
            m_buffer = buffer;
            winrt::check_hresult(buffer->Lock(&m_info.Bits, &m_info.MaxLength, &m_info.CurrentLength));
        }

        ~MediaBufferGuard()
        {
            winrt::check_hresult(m_buffer->Unlock());
        }

        BufferInfo const& Info() { return m_info; }

    private:
        winrt::com_ptr<IMFMediaBuffer> m_buffer;
        BufferInfo m_info = {};
    };

    inline std::optional<std::wstring> GetStringAttribute(winrt::com_ptr<IMFAttributes> const& attributes, GUID const& attributeGuid)
    {
        uint32_t resultLength = 0;
        HRESULT hr = attributes->GetStringLength(attributeGuid, &resultLength);
        if (SUCCEEDED(hr))
        {
            std::wstring result((size_t)resultLength + 1, L' ');
            winrt::check_hresult(attributes->GetString(attributeGuid, result.data(), (uint32_t)result.size(), &resultLength));
            result.resize(resultLength);
            return std::optional(std::move(result));
        }
        else if (hr != MF_E_ATTRIBUTENOTFOUND)
        {
            throw winrt::hresult_error(hr);
        }
        return std::nullopt;
    }

    inline auto EnumerateMFTs(GUID const& category, uint32_t const& flags, MFT_REGISTER_TYPE_INFO const* inputType, MFT_REGISTER_TYPE_INFO const* outputType)
    {
        std::vector<winrt::com_ptr<IMFActivate>> transformSources;
        {
            wil::unique_cotaskmem_array_ptr<wil::com_ptr<IMFActivate>> activateArray;
            winrt::check_hresult(MFTEnumEx(
                category,
                flags,
                inputType,
                outputType,
                activateArray.put(),
                reinterpret_cast<uint32_t*>(activateArray.size_address())));

            if (activateArray.size() > 0)
            {
                for (auto&& activate : activateArray)
                {
                    winrt::com_ptr<IMFActivate> transformSource;
                    transformSource.copy_from(activate);
                    transformSources.push_back(transformSource);
                }
            }
        }
        return transformSources;
    }
}
