#include <boost/regex.hpp>

#include <cassert>
#include <deque>
#include <fmt/core.h>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <utility>

static constexpr const unsigned OUTPUT_OFFSET = 1000;

using Val = unsigned;

struct Bot {
    void put(Val v) noexcept {
        if (!low) {
            low = v;
        } else {
            high = v;
        }
        if (low.value_or(0) > high.value_or(0)) {
            std::swap(low, high);
        }
    }

    std::optional<Val> low, high;
};

struct Rule
{
    unsigned dst_low = 0;
    unsigned dst_high = 0;
};

struct Input
{
    unsigned bot = 0;
    Val val = 0 ;
};

using Rules = std::map<unsigned, Rule>;
using Inputs = std::vector<Input>;

int main()
{
    static const boost::regex RX_RULE("^bot (\\d+) gives low to ([a-z]+) (\\d+) and high to ([a-z]+) (\\d+)$");
    static const boost::regex RX_INPUT("^value (\\d+) goes to bot (\\d+)$");

    Rules rules;
    Inputs inputs;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) break;

        boost::smatch res;
        if (boost::regex_search(line, res, RX_RULE)) {
            Rule r;
            unsigned bot = std::stoul(res[1]);
            r.dst_low = std::stoul(res[3]);
            if (res[2] == "output") {
                r.dst_low += OUTPUT_OFFSET;
            }
            r.dst_high = std::stoul(res[5]);
            if (res[4] == "output") {
                r.dst_high += OUTPUT_OFFSET;
            }
            rules.insert({bot, std::move(r)});
        } else if(boost::regex_search(line, res, RX_INPUT)) {
            Input& i = inputs.emplace_back();
            i.bot = std::stoul(res[2]);
            i.val = std::stoul(res[1]);
        } else {
            assert(false);
        }
    }

    std::map<unsigned, Bot> bots;

    for (auto const& input : inputs) {
        Bot& dst = bots[input.bot];
        assert(!dst.low || !dst.high);
        if (dst.low) {
            dst.high = input.val;
        } else {
            dst.low = input.val;
        }
        if (dst.low > dst.high) {
            std::swap(dst.low, dst.high);
        }

        std::set<unsigned> work_queue;
        for (auto& [bid, bot] : bots) {
            if (bot.low && bot.high) {
                work_queue.insert(bid);
            }
        }

        while(!work_queue.empty()) {
            unsigned bid = *work_queue.begin();
            work_queue.erase(work_queue.begin());

            Bot& b0 = bots.at(bid);
            assert(b0.low && b0.high);

            if (b0.low == 17 && b0.high == 61) {
                fmt::print("1: {}\n", bid);
                return 0;
            }

            Rule const& rule = rules.at(bid);
            Bot& b1 = bots[rule.dst_low];
            assert(!b1.low || !b1.high);

            Bot& b2 = bots[rule.dst_high];
            assert(!b2.low || !b2.high);

            b1.put(b0.low.value());
            b0.low.reset();

            b2.put(b0.high.value());
            b0.high.reset();

            if (rule.dst_low < OUTPUT_OFFSET && b1.low && b1.high) work_queue.insert(rule.dst_low);
            if (rule.dst_high < OUTPUT_OFFSET && b2.low && b2.high) work_queue.insert(rule.dst_high);
        }
    }

    return 0;
}
