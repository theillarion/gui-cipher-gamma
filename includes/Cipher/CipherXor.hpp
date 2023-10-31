#pragma once

#include "Cipher/ICipher.hpp"

#include <type_traits>
#include <memory>

namespace xk7
{
	template <class KeyT, class KeyViewT = const KeyT&>
	class CipherXor : public ICipher<KeyT, KeyT, KeyViewT>
	{
	public:
		enum TypeCrypt
		{
			Default = 0,
			BlockClutchMode,
		};
	private:
		using base_type = ICipher<KeyT, KeyT, KeyViewT>;

	public:
		using key_type = typename base_type::key_type;
		using string_type = typename base_type::string_type;
		using string_view_type = typename base_type ::string_view_type;
		using char_type = typename base_type::char_type;

	private:
		static_assert(std::is_same_v<key_type, string_type>, "Key must be a string type");

	private:
		string_type encryptImpl(string_view_type message, std::function<char_type(char_type)> unary_op) const
		{
			string_type result{};
			result.reserve(message.size());

			std::transform(std::begin(message), std::end(message), std::back_inserter(result), unary_op);

			return result;
		}
	public:
		explicit CipherXor(TypeCrypt type_crypt = {}, key_type key = {})
			:	type_crypt_{ type_crypt }, key_{ key }
		{ }

		~CipherXor() override = default;

		inline string_type encrypt(string_view_type message) const override
		{
			std::function<char_type(char_type)> encryptFunc;
			if (std::size(key_) != 0)
				encryptFunc = [key = key_, i = 0](char_type symbol) mutable
				{
					return char_type(symbol.unicode() ^ key[i++ % std::size(key)].unicode());
				};
			else
				encryptFunc = [](char_type s) { return s; };

			if (type_crypt_ == BlockClutchMode)
				return encryptImpl(message,
				   [encryptFunc = encryptFunc, prev_block = char_type{ 0 }] (char_type symbol) mutable
				   {
					   auto result = encryptFunc(char_type(symbol.unicode() ^ prev_block.unicode()));
					   prev_block = result;
					   return result;
				   }
				);
			return encryptImpl(message, encryptFunc);
		}

		inline string_type decrypt(string_view_type message) const override
		{
			std::function<char_type(char_type)> decryptFunc;
			if (std::size(key_) != 0)
				decryptFunc = [key = key_, i = 0](char_type symbol) mutable
				{
					return char_type(symbol.unicode() ^ key[i++ % std::size(key)].unicode());
				};
			else
				decryptFunc = [](char_type s) { return s; };

			if (type_crypt_ == BlockClutchMode)
				return encryptImpl(message,
				   [decryptFunc = decryptFunc, prev_block = char_type{ 0 }] (char_type symbol) mutable
				   {
					   auto result = char_type(prev_block.unicode() ^ decryptFunc(symbol).unicode());
					   prev_block = symbol;
					   return result;
				   }
				);
			return encryptImpl(message, decryptFunc);
		}

		inline const key_type& getKey() const noexcept override
		{
			return key_;
		}
		inline void setKey(const key_type& key) noexcept override
		{
			key_ = key;
		}

	protected:
		TypeCrypt type_crypt_;
		key_type key_;
	};
} // namespace xk7