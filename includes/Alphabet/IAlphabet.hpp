#pragma once

#include <cstddef>

namespace xk7
{
    template <typename CharT>
    class IAlphabet
    {
    public:
        using char_type = CharT;
        using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;

    public:
        virtual ~IAlphabet() = 0;

		[[nodiscard]] virtual size_type index(char_type) const noexcept = 0;
        [[nodiscard]] virtual size_type size() const noexcept = 0;
		[[nodiscard]] virtual char_type getSymbol(size_type) const = 0;
		[[nodiscard]] virtual bool include(char_type) const noexcept = 0;
    };

    template <typename CharT>
    IAlphabet<CharT>::~IAlphabet() = default;
}