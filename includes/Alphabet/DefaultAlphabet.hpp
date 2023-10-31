#pragma once

#include "Alphabet/IAlphabet.hpp"

#include <limits>
#include <algorithm>
#include <iterator>

namespace xk7
{
    template<typename StringT>
    class DefaultAlphabet : public IAlphabet<typename StringT::value_type>
    {
    private:
        using base_type = IAlphabet<typename StringT::value_type>;

    public:
		using string_type = StringT;
        using typename base_type::char_type;
        using typename base_type::size_type;
		using typename base_type::difference_type;

    public:
		DefaultAlphabet() = delete;

		DefaultAlphabet(char_type first_symbol, char_type last_symbol, string_type other_symbols)
            :	first_symbol_{ first_symbol },
				last_symbol_{ last_symbol },
				size_symbols_{ static_cast<size_type>(last_symbol_.unicode() - first_symbol_.unicode()) + 1 },
				other_symbols_{ other_symbols },
				size_{ size_symbols_ + other_symbols_.size() }
		{ }

		DefaultAlphabet(char_type first_symbol, char_type second_symbol)
			:	DefaultAlphabet{ first_symbol, second_symbol, string_type{ } }
		{ }

		DefaultAlphabet(char_type first_symbol, char_type second_symbol, char_type space)
			:	DefaultAlphabet{ first_symbol, second_symbol, string_type{ space } }
		{ }

        ~DefaultAlphabet() override = default;

    public:
        size_type index(char_type symbol) const noexcept override
        {
            if (first_symbol_ <= symbol && symbol <= last_symbol_)
                return static_cast<size_type>(symbol.unicode() - first_symbol_.unicode());
            else if (auto it = std::find(other_symbols_.begin(), other_symbols_.end(), symbol); it != other_symbols_.end())
                return size_symbols_ + std::distance(other_symbols_.begin(), it);
            return std::numeric_limits<size_type>::max();
        }

        size_type size() const noexcept override
        {
            return size_;
        }

        char_type getSymbol(size_type index) const override
        {
            if (index < size_symbols_)
                return char_type(static_cast<char16_t>(first_symbol_.unicode() + index));
            else if (index < size_)
                return other_symbols_[index - size_symbols_];
			return { '\0' };
        }

        bool include(char_type symbol) const noexcept override
        {
            return (first_symbol_ <= symbol && symbol <= last_symbol_) ||
				std::find(other_symbols_.begin(), other_symbols_.end(), symbol) != other_symbols_.end();
        }

    private:
        char_type first_symbol_;
        char_type last_symbol_;
		size_type size_symbols_;
        string_type other_symbols_;
        size_type size_;
    };
} // namespace xk7