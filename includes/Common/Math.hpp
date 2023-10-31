#pragma once

namespace xk7::math
{
    template <typename SignedNumber, typename UnsignedNumber>
    UnsignedNumber mod(SignedNumber number, UnsignedNumber module)
    {
		SignedNumber div = number % static_cast<SignedNumber>(module);
		if (div < 0)
			return div + module;
		return div;
    }
} // namespace xk7::math