#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <array>
#include <cstddef>
#include <cstdlib>
#include <fmt/core.h>
#include <iostream>
#include <iterator>
#include <map>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

enum class Register
{
    A = 0,
    B,
    C,
    D,
};

enum class Op
{
    NOP = 0,
    CPY,
    INC,
    DEC,
    JNZ,
    TGL,
};

using Argument = std::variant<bool, Register, int64_t>;

struct Instruction {
    Op op = Op::NOP;
    Argument arg1;
    Argument arg2;
};

using Program = std::vector<Instruction>;

struct CPU
{
    CPU(Program p) noexcept : program{std::move(p)} {}

    void run() noexcept;

    constexpr bool has_value(Argument const& arg) const noexcept { return arg.index() > 0; }

    constexpr int64_t& get_value(Argument const& arg) noexcept {
        return std::visit(
            [this](auto&& _arg) -> int64_t& {
                using T = std::decay_t<decltype(_arg)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                    tmp1 = _arg;
                    return tmp1;
                } else if constexpr (std::is_same_v<T, Register>) {
                    return registers.at(static_cast<unsigned>(_arg));
                } else {
                    assert(false);
                }
            },
            arg);
    }

    Program program;
    std::array<int64_t, sizeof(Register)> registers = {0};
    int64_t tmp1 = 0;
};


void CPU::run() noexcept
{
    for (auto pc = program.begin(); pc != program.end();)
    {
        Instruction const& i = *pc;

        switch(i.op) {
        case Op::INC:
            get_value(i.arg1) += 1;
            ++pc;
            break;
        case Op::DEC:
            get_value(i.arg1) -= 1;
            ++pc;
            break;
        case Op::CPY:
            get_value(i.arg2) = get_value(i.arg1);
            ++pc;
            break;
        case Op::JNZ:
            if (get_value(i.arg1)) {
                std::advance(pc, get_value(i.arg2));
            } else {
                ++pc;
            }
            break;
        case Op::TGL:
            if (std::distance(pc, program.end()) > get_value(i.arg1)) {
                Instruction& tgl = *std::next(pc, get_value(i.arg1));
                if (has_value(tgl.arg2)) {
                    if (tgl.op == Op::CPY) {
                        tgl.op = Op::JNZ;
                    } else {
                        tgl.op = Op::CPY;
                    }
                } else {
                    if (tgl.op == Op::INC) {
                        tgl.op = Op::DEC;
                    } else {
                        tgl.op = Op::INC;
                    }
                }
            }
            ++pc;
            break;
        default:
            assert(false);
        }
    }
}


int main()
{
    Program prog;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        static const std::map<std::string, Op> str2op = {
            {"cpy", Op::CPY}, {"inc", Op::INC}, {"dec", Op::DEC}, {"jnz", Op::JNZ}, {"tgl", Op::TGL}};

        auto is_reg = [](std::string_view const s) -> bool {
            return (s.size() == 1 && (s.at(0) == 'a' || s.at(0) == 'b' || s.at(0) == 'c' || s.at(0) == 'd'));
        };
        auto str2reg = [](std::string_view const s) -> Register {
            const unsigned char c = s.at(0);
            switch (c) {
            case 'a': return Register::A;
            case 'b': return Register::B;
            case 'c': return Register::C;
            case 'd': return Register::D;
            default: assert(false);
            }
        };

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "));
        assert(parts.size() > 1);

        Instruction i = {};
        i.op = str2op.at(parts.at(0));
        switch(i.op) {
        case Op::INC:
            assert(parts.size() == 2);
            i.arg1 = str2reg(parts.at(1));
            break;
        case Op::DEC:
            assert(parts.size() == 2);
            i.arg1 = str2reg(parts.at(1));
            break;
        case Op::CPY:
            assert(parts.size() == 3);
            if (is_reg(parts.at(1))) {
                i.arg1 = str2reg(parts.at(1));
            } else {
                i.arg1 = std::stoll(parts.at(1));
            }
            if (is_reg(parts.at(2))) {
                i.arg2 = str2reg(parts.at(2));
            } else {
                i.arg2 = std::stoll(parts.at(2));
            }
            break;
        case Op::JNZ:
            assert(parts.size() == 3);
            if (is_reg(parts.at(1))) {
                i.arg1 = str2reg(parts.at(1));
            } else {
                i.arg1 = std::stoll(parts.at(1));
            }
            if (is_reg(parts.at(2))) {
                i.arg2 = str2reg(parts.at(2));
            } else {
                i.arg2 = std::stoll(parts.at(2));
            }
            break;
        case Op::TGL:
            assert(parts.size() == 2);
            i.arg1 = str2reg(parts.at(1));
            break;
        default:
            assert(false);
        }
        prog.push_back(std::move(i));
    }

    CPU cpu1(prog);
    cpu1.registers.at(static_cast<unsigned>(Register::A)) = 7;
    cpu1.run();
    fmt::print("1: {}\n", cpu1.registers.at(static_cast<unsigned>(Register::A)));

    CPU cpu2(prog);
    cpu2.registers.at(static_cast<unsigned>(Register::A)) = 12;
    cpu2.run();
    fmt::print("2: {}\n", cpu2.registers.at(static_cast<unsigned>(Register::A)));

    return 0;
}
