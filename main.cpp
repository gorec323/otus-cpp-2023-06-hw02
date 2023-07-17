#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <algorithm>
#include <tuple>

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
std::vector<std::string> split(const std::string &str, char delimitter)
{
    std::vector<std::string> result;

    std::string::size_type start = 0;
    auto stop = str.find_first_of(delimitter);
    while(stop != std::string::npos)
    {
        result.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(delimitter, start);
    }

    result.push_back(str.substr(start));

    return result;
}

class IpAddr
{
public:
    IpAddr(std::vector<std::string> &&parts)
    {
        std::transform(parts.cbegin(), parts.cend(), m_ipParts.begin(), [](const std::string &strPart)
        {
            return static_cast<std::uint8_t>(std::stoul(strPart));
        });
    }

    ~IpAddr() = default;

    bool operator>(const IpAddr &other) const noexcept
    {
        return (toU32() > other.toU32());
    }

    void print() const noexcept
    {
        for (auto ip_part = m_ipParts.cbegin(); ip_part != m_ipParts.cend(); ++ip_part) {
            if (ip_part != m_ipParts.cbegin())
                std::cout << ".";

            std::cout << std::to_string(*ip_part);
        }
        std::cout << std::endl;
    }

    template<typename... Args>
    inline bool filter(Args... flt) const
    {
        return filter_impl(m_ipParts.cbegin(), flt...);
    }

    inline bool filter_any(std::uint8_t flt) const
    {
        return std::any_of(m_ipParts.cbegin(), m_ipParts.cend(),
                           [flt](std::uint8_t v)
        {
            return v == flt;
        });
    }

private:
    template<typename ItrerType>
    bool filter_impl(ItrerType) const
    {
        return false;
    }

    template<typename ItrerType , typename T, typename... Args>
    bool filter_impl(ItrerType it, T v, Args... flt) const
    {
        if (it == m_ipParts.cend())
            return true; // дошли до конца адреса

        if (*it != v)
            return false;

        if (sizeof...(flt) == 0)
            return true;

        return filter_impl(++it, flt...);

    }

    inline std::uint32_t toU32() const noexcept
    {
        return static_cast<std::uint32_t>(m_ipParts[0]) << 24
                | static_cast<std::uint32_t>(m_ipParts[1]) << 16
                | static_cast<std::uint32_t>(m_ipParts[2]) << 8
                | m_ipParts[3];
    }

    std::array<std::uint8_t, 4> m_ipParts;
};

int main(int argc, char const *argv[])
{
    try {
        std::vector<IpAddr> ip_pool;

        for (std::string line; std::getline(std::cin, line);) {
            std::vector<std::string> v = split(line, '\t');
            ip_pool.push_back(split(v.at(0), '.'));
        }

        // reverse lexicographically sort

        std::sort(ip_pool.begin(), ip_pool.end(), std::greater<IpAddr>());

        for(auto &&ip : ip_pool)
            ip.print();

        // 222.173.235.246
        // 222.130.177.64
        // 222.82.198.61
        // ...
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        for(auto &&ip : ip_pool) {
            if (ip.filter(1u))
                ip.print();
        }


        // TODO filter by first byte and output
        // ip = filter(1)

        // 1.231.69.33
        // 1.87.203.225
        // 1.70.44.170
        // 1.29.168.152
        // 1.1.234.8

        // TODO filter by first and second bytes and output
        // ip = filter(46, 70)

        for(auto &&ip : ip_pool) {
            if (ip.filter(46u, 70u))
                ip.print();
        }


        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76

        // TODO filter by any byte and output
        // ip = filter_any(46)

        for (auto &&ip : ip_pool) {
            if (ip.filter_any(46u))
                ip.print();
        }

        // 186.204.34.46
        // 186.46.222.194
        // 185.46.87.231
        // 185.46.86.132
        // 185.46.86.131
        // 185.46.86.131
        // 185.46.86.22
        // 185.46.85.204
        // 185.46.85.78
        // 68.46.218.208
        // 46.251.197.23
        // 46.223.254.56
        // 46.223.254.56
        // 46.182.19.219
        // 46.161.63.66
        // 46.161.61.51
        // 46.161.60.92
        // 46.161.60.35
        // 46.161.58.202
        // 46.161.56.241
        // 46.161.56.203
        // 46.161.56.174
        // 46.161.56.106
        // 46.161.56.106
        // 46.101.163.119
        // 46.101.127.145
        // 46.70.225.39
        // 46.70.147.26
        // 46.70.113.73
        // 46.70.29.76
        // 46.55.46.98
        // 46.49.43.85
        // 39.46.86.85
        // 5.189.203.46
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
