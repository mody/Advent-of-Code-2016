#include "point2d.h"

#include <fmt/core.h>
#include <sstream>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <deque>
#include <string_view>
#include <unordered_set>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;

enum class Door
{
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3,
};
using Doors = std::bitset<sizeof(Door)>;

static constexpr const std::array<Gfx_2d::Direction, sizeof(Door)> door2dir = {
    Gfx_2d::Up,
    Gfx_2d::Down,
    Gfx_2d::Left,
    Gfx_2d::Right,
};
static constexpr const std::array<unsigned char, sizeof(Door)> door2door = {
    'U',
    'D',
    'L',
    'R',
};

std::string MD5_short(std::string_view const input)
{
    static const std::array<unsigned char, 16> to_char = {'0', '1', '2', '3', '4', '5', '6', '7',
                                                          '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    std::string result;

    CryptoPP::Weak::MD5 md5;
    result.resize(md5.DigestSize());
    md5.Update((const CryptoPP::byte*)&input[0], input.size());
    md5.Final((CryptoPP::byte*)&result[0]);

    const unsigned char c1 = result.at(0);
    const unsigned char c2 = result.at(1);

    result.resize(4);
    result.at(static_cast<unsigned>(Door::Up)) = to_char.at((c1 & 0xf0) >> 4);
    result.at(static_cast<unsigned>(Door::Down)) = to_char.at((c1 & 0x0f));
    result.at(static_cast<unsigned>(Door::Left)) = to_char.at((c2 & 0xf0) >> 4);
    result.at(static_cast<unsigned>(Door::Right)) = to_char.at((c2 & 0x0f));

    return result;
}


struct State
{
    State() noexcept;

    void move_me(Door const dir) noexcept;

    bool operator==(State const& oth) const noexcept
    {
        return std::tie(px, doors, moves) == std::tie(oth.px, oth.doors, oth.moves);
    }

    std::string to_string() const noexcept
    {
        return fmt::format("@ ({},{});{};{}", px.x, px.y, doors.to_string(), moves);
    }

    Point px = {};
    Doors doors = {};
    std::string moves = {};

    static const std::string SEED;

private:
    void update_doors() noexcept;
};


State::State() noexcept
{
    update_doors();
}

void State::move_me(Door const dir) noexcept
{
    moves += door2door.at(static_cast<unsigned>(dir));
    px += door2dir.at(static_cast<unsigned>(dir));
    assert(px.x >= 0);
    assert(px.y >= 0);
    assert(px.x < 4);
    assert(px.y < 4);
    update_doors();
}

void State::update_doors() noexcept
{
    const std::string d = MD5_short(SEED + moves);
    assert(d.size() == 4);
    doors.set(
        static_cast<unsigned>(Door::Up),
        (d.at(static_cast<unsigned>(Door::Up)) >= 'b' && d.at(static_cast<unsigned>(Door::Up)) <= 'f'));
    doors.set(
        static_cast<unsigned>(Door::Down),
        (d.at(static_cast<unsigned>(Door::Down)) >= 'b' && d.at(static_cast<unsigned>(Door::Down)) <= 'f'));
    doors.set(
        static_cast<unsigned>(Door::Left),
        (d.at(static_cast<unsigned>(Door::Left)) >= 'b' && d.at(static_cast<unsigned>(Door::Left)) <= 'f'));
    doors.set(
        static_cast<unsigned>(Door::Right),
        (d.at(static_cast<unsigned>(Door::Right)) >= 'b' && d.at(static_cast<unsigned>(Door::Right)) <= 'f'));

    if (px.x == 0) doors.reset(static_cast<unsigned>(Door::Left));
    if (px.x == 3) doors.reset(static_cast<unsigned>(Door::Right));
    if (px.y == 0) doors.reset(static_cast<unsigned>(Door::Up));
    if (px.y == 3) doors.reset(static_cast<unsigned>(Door::Down));

    // fmt::print("MD5_short: {} -> {} -> {}\n", SEED + moves, d, doors.to_string());
}

template<>
struct std::hash<State>
{
    size_t operator() (State const& s) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, std::hash<Point>{}(s.px));
        boost::hash_combine(seed, std::hash<Doors>{}(s.doors));
        return seed;
    }
};


std::string process(bool shortest)
{
    std::deque<State> work_queue;
    std::unordered_set<State> seen;

    work_queue.push_back({});

    std::string result;

    while(!work_queue.empty())
    {
        const State s = work_queue.front();
        work_queue.pop_front();

        // fmt::print("Pop {}\n", s.to_string());

        for (auto dir : {Door::Up, Door::Down, Door::Left, Door::Right})
        {
            if (s.doors.test(static_cast<unsigned>(dir))) {
                State next_state = s;
                next_state.move_me(dir);

                if (next_state.px == Point{3,3}) {
                    if (shortest) {
                        result = next_state.moves;
                        work_queue.clear();
                        break;
                    } else {
                        if (result.size() < next_state.moves.size()) {
                            result = next_state.moves;
                        }
                        continue;
                    }
                }

                auto it = seen.find(next_state);
                if (it == seen.end()) {
                    work_queue.push_back(next_state);
                    seen.insert(std::move(next_state));
                } else {
                    assert(it->moves.size() < next_state.moves.size());
                }
            }
        }
    }
    return result;
}

// const std::string State::SEED = "ihgpwlah";
// const std::string State::SEED = "kglvqrro";
// const std::string State::SEED = "ulqzkmiv";

const std::string State::SEED = "vkjiggvb";

int main()
{
    fmt::print("1: {}\n", process(true));
    fmt::print("2: {}\n", process(false).size());

    return 0;
}
