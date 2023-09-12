#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/hex.h>
#include <cryptopp/md5.h>

#include <fmt/core.h>

#include <oneapi/tbb/blocked_range.h>
#include <oneapi/tbb/concurrent_map.h>
#include <oneapi/tbb/parallel_for.h>

#include <cassert>
#include <iterator>
#include <limits>
#include <map>
#include <string_view>


using CryptoPP::byte;

std::string find_key(const unsigned size, const std::string_view input)
{
    unsigned char c = '\0';
    unsigned count = 0;
    for (unsigned i = 0; i < input.size(); ++i) {
        if (c != input.at(i)) {
            c = input.at(i);
            count = 1;
        } else {
            ++count;
        }
        if (count == size) {
            return std::string(count, c);
        }
    }
    return {};
}

unsigned process(unsigned stretch)
{
    oneapi::tbb::concurrent_map<unsigned, std::string> three_pos;
    oneapi::tbb::concurrent_multimap<std::string, unsigned> five_pos;

    oneapi::tbb::parallel_for(
        oneapi::tbb::blocked_range<unsigned>(0, 30000),
        [&three_pos, &five_pos, &stretch](oneapi::tbb::blocked_range<unsigned> const& r) {
            CryptoPP::Weak::MD5 md5;
            std::string decoded;
            decoded.resize(md5.DigestSize());

            for (unsigned i = r.begin(); i < r.end(); ++i) {
                std::string msg = "ngcjuoqr" + std::to_string(i);

                for (unsigned j = 0; j <= stretch; ++j) {
                    md5.Update((const byte*)&msg[0], msg.size());
                    md5.Final((byte*)&decoded[0]);

                    msg.resize(md5.DigestSize() * 2);
                    unsigned idx = 0;
                    for (auto const& c : decoded) {
                        static const std::array<unsigned char, 16> to_char = {'0', '1', '2', '3', '4', '5', '6', '7',
                                                                              '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
                        msg.at(idx++) = to_char.at((c & 0xf0) >> 4);
                        msg.at(idx++) = to_char.at(c & 0x0f);
                    }
                }

                std::string key = find_key(5, msg);
                if (key.size() == 5) {
                    five_pos.insert({key, i});
                }

                key = (key.size() == 5) ? key.substr(0, 3) : find_key(3, msg);
                if (key.size() == 3) {
                    three_pos.insert({i, key});
                }
            }
        });

    unsigned num_keys = 0;
    unsigned result = 0;

    for (auto const& [i, key3] : three_pos) {
        const std::string key5 = std::string(5, key3.at(0));
        for (auto it5 = five_pos.lower_bound(key5); it5 != five_pos.end() && it5->first == key5; ++it5) {
            const auto diff = (it5->second - i);
            if (diff > 0 && diff < 1000) {
                ++num_keys;
                if (num_keys == 64) {
                    result = i;
                }
                break;
            }
        }
        if (num_keys == 64) {
            break;
        }
    }
    return result;
}

int main()
{
    fmt::print("1: {}\n", process(0));
    fmt::print("2: {}\n", process(2016));

    return 0;
}
