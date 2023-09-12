#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>
#include <cryptopp/hex.h>

#include <fmt/core.h>

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
            // if (i == (input.size() - 1) || c != input.at(i + 1)) {
                return std::string(count, c);
            // }
        }
    }
    return {};
}

int main()
{
    std::map<unsigned, std::string> three_pos;
    std::multimap<std::string, unsigned> five_pos;

    // for (unsigned i = 0; i < std::numeric_limits<unsigned>::max(); ++i) {
    for (unsigned i = 0; i < 50000; ++i) {
        const std::string msg = "ngcjuoqr" + std::to_string(i);
        // const std::string msg = "abc" + std::to_string(i);

        CryptoPP::Weak::MD5 md5;
        md5.Update((const byte*)&msg[0], msg.size());
        std::string decoded;
        decoded.resize(md5.DigestSize());
        md5.Final((byte*)&decoded[0]);

        CryptoPP::HexEncoder encoder(nullptr, false);
        encoder.Put((byte*)decoded.data(), decoded.size());
        encoder.MessageEnd();

        std::string encoded;
        const auto size = encoder.MaxRetrievable();
        if (size) {
            encoded.resize(size);
            encoder.Get((byte*)&encoded[0], encoded.size());
        }

        // fmt::print ("{}\t{} ", i, encoded);

        std::string key = find_key(5, encoded);
        if (key.size() == 5) {
            // fmt::print("found {}", key);
            five_pos.insert({key, i});
            // fmt::print("\n");
        }

        key = (key.size() == 5) ? key.substr(0, 3) : find_key(3, encoded);
        if (key.size() == 3) {
            // fmt::print("found {}", key);
            three_pos.insert({i, key});
        }

        // fmt::print("\n");
    }

    unsigned num_keys = 0;
    unsigned result1 = 0;

    for (auto const& [i, key3] : three_pos) {
        const std::string key5 = std::string(5, key3.at(0));
        // fmt::print("{} at {} ", key3, i);
        for (auto it5 = five_pos.lower_bound(key5); it5 != five_pos.end() && it5->first == key5; ++it5) {
            // fmt::print(", key5 at {}", it5->second);
            const auto diff = (it5->second - i);
            if (diff > 0 && diff < 1000) {
                ++num_keys;
                // fmt::print(" {} within limit, keys {}", diff, num_keys);
                if (num_keys == 64) {
                    result1 = i;
                }
                break;
            }
        }
        // fmt::print("\n");
        if (num_keys == 64) {
            break;
        }
    }

    fmt::print("1: {}\n", result1);

    return 0;
}
