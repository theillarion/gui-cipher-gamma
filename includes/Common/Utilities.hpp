#pragma once

#include <QString>

#include <type_traits>

namespace xk7
{
    template <typename Number>
    std::enable_if_t<std::is_integral_v<Number>, QString > fromNumber(Number number)
    {
        return QString::number(number);
    }

    template <typename Number>
    std::enable_if_t<!std::is_integral_v<Number>, QString > fromNumber(Number number)
    {
        return QString::fromStdString(number.str());
    }

    template <typename Number>
    std::enable_if_t<std::is_same_v<Number, int>, Number > tryToNumber(const QString& string, bool* result)
    {
        return string.toInt(result);
    }

    template <typename Number>
    std::enable_if_t<!std::is_integral_v<Number>, Number > tryToNumber(const QString& string, bool* result)
    {
        *result = true;
        try
        {
            if (string.size() == 0 || (string[0] == '-' && string.size() == 1) || string[0] == '+')
            {
                *result = false;
                return Number{};
            }
            if (string[0] == '-' && string.size() > 1 && string[1] == '0')
            {
                *result = false;
                return Number{};
            }
            return Number{ string.toStdString() };

        }
        catch (std::exception&)
        {
            *result = false;
            return Number{};
        }
    }
} // namespace xk7