#include <fmt/core.h>
#include <limits>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>

#include <map>

using CryptoPP::byte;

int main()
{
    std::string password1;
    password1.reserve(8);
    std::map<unsigned, unsigned char> password2;

    CryptoPP::Weak::MD5 md5;
    std::string digest;

    for (unsigned i = 0; i < std::numeric_limits<unsigned>::max(); ++i) {
        const std::string msg = "abbhdwsy" + std::to_string(i);
        md5.Update((const byte*)&msg[0], msg.size());
        digest.resize(md5.DigestSize());
        md5.Final((byte*)&digest[0]);

        if (digest[0] == 0 && digest[1] == 0 && (digest[2] & 0xf0) == 0) {
            static const std::array<unsigned char, 16> TO_STR = {'0', '1', '2', '3', '4', '5', '6', '7',
                                                                 '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
            byte k1 = digest[2] & 0xf;

            if (password1.size() < 8) {
                password1 += TO_STR.at(k1);
            }

            if (k1 < 8) {
                password2.insert({k1, TO_STR.at((digest[3] & 0xf0) >> 4)});
            }

            if (password2.size() == 8) {
                break;
            }
        }
    }

    fmt::print("1: {}\n", password1);

    std::string pwd2(8, '\0');
    for (auto const& [k,v] : password2) {
        pwd2[k] = v;
    }
    fmt::print("2: {}\n", pwd2);

    return 0;
}
