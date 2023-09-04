#include <fmt/core.h>
#include <limits>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>

#include <iostream>

using CryptoPP::byte;

int main()
{
    std::string password;

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
            password += TO_STR.at(digest[2] & 0xf);
            if (password.size() == 8) {
                break;
            }
        }
    }

    fmt::print("1: {}\n", password);

    return 0;
}
