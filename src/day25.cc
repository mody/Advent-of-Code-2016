#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <array>
#include <cstddef>
#include <cstdlib>
#include <fmt/core.h>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <string>
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
    OUT,
};

using Argument = std::variant<bool, Register, int64_t>;

struct Instruction {
    Op op = Op::NOP;
    Argument arg1;
    Argument arg2;
    bool dump_regs = false;
};

using Program = std::vector<Instruction>;

static constexpr const unsigned MAX_OUTPUT = 12;

struct CPU
{
    CPU(Program const& p) noexcept
        : program {p}
    { }

    void run() noexcept;

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

    Program const& program;
    std::array<int64_t, sizeof(Register)> registers = {0};
    std::string output;
    int64_t tmp1 = 0;
};


void CPU::run() noexcept
{
    output.clear();

    for (auto pc = program.begin(); pc != program.end();)
    {
        Instruction const& i = *pc;

        if (i.dump_regs) {
            fmt::print(
                "A: {}; B: {}; C: {}; D:{}\n",
                registers.at(static_cast<unsigned>(Register::A)),
                registers.at(static_cast<unsigned>(Register::B)),
                registers.at(static_cast<unsigned>(Register::C)),
                registers.at(static_cast<unsigned>(Register::D)));
        }

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
        case Op::OUT:
            output.push_back(get_value(i.arg1) == 1 ? '1' : '0');
            if (output.size() == MAX_OUTPUT) {
                pc = program.end();
            } else {
                ++pc;
            }
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
            {"cpy", Op::CPY}, {"inc", Op::INC}, {"dec", Op::DEC}, {"jnz", Op::JNZ}, {"out", Op::OUT}};

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
        if (parts.at(0) == "!") {
            i.dump_regs = true;
            parts.erase(parts.begin());
        }
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
        case Op::OUT:
            assert(parts.size() == 2);
            i.arg1 = str2reg(parts.at(1));
            break;
        default: assert(false);
        }
        prog.push_back(std::move(i));
    }


    const std::string target = []() {
        std::string ret;
        unsigned char c = '0';
        for (unsigned i =0; i<MAX_OUTPUT; ++i) {
            ret += c;
            c = (c == '0' ? '1' : '0');
        }
        return ret;
    }();

    {
            CPU cpu1(prog);
            for (unsigned x = 0; x < std::numeric_limits<unsigned>::max(); ++x) {
                cpu1.registers = {0};
                cpu1.registers.at(static_cast<unsigned>(Register::A)) = x;
                cpu1.run();
                if (cpu1.output == target) {
                    fmt::print("1: {} ({})\n", x, cpu1.output);
                    break;
                }
            }
    }

    return 0;
}
