#pragma once

#include <cstddef>

namespace xk7
{
    template<typename KeyT, class StringT, class StringViewT = const StringT&>
    class ICipher
    {
    public:
        using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using key_type = KeyT;
        using string_type = StringT;
        using string_view_type = StringViewT;
        using char_type = typename string_type::value_type;

    public:
        virtual ~ICipher() = 0;

        virtual string_type encrypt(string_view_type) const = 0;
        virtual string_type decrypt(string_view_type) const = 0;
		virtual const key_type& getKey() const = 0;
		virtual void setKey(const key_type&) = 0;
    }; // class ICipher

	// ===== DEFAULT DEFINITION FOR DESTRUCTOR ===== //
    template <typename TKey, class TString, class TStringView>
    ICipher<TKey, TString, TStringView>::~ICipher() = default;
} // namespace xk7