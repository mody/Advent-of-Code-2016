#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/container_hash/hash.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <array>
#include <bitset>
#include <boost/graph/graph_selectors.hpp>
#include <deque>
#include <fmt/core.h>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <string_view>

// lets use NAND to detect collision
enum Material
{
    Lithium = 0,
    Hydrogen,
    Strontium,
    Plutonium,
    Thulium,
    Ruthenium,
    Curium,
    Material_Size,
};
using Materials = std::bitset<Material_Size>;

static const std::map<std::string, Material> Str2Mat = {
    {"hydrogen", Hydrogen},
    {"lithium", Lithium},
    {"strontium", Strontium},
    {"plutonium", Plutonium},
    {"thulium", Thulium},
    {"ruthenium", Ruthenium},
    {"curium", Curium},
};

struct Floor
{
    bool operator==(Floor const& oth) const noexcept
    {
        return std::tie(generators, microchips) == std::tie(oth.generators, oth.microchips);
    }

    bool valid() const noexcept
    {
        const Materials mask = ~(generators & microchips);
        if ((generators).any() && (microchips & mask).any()) {
            return false;
        }
        return true;
    }

    Floor& operator+= (Floor const& oth) noexcept {
        generators |= oth.generators;
        microchips |= oth.microchips;
        return *this;
    }

    Floor& operator-= (Floor const& oth) noexcept {
        generators &= ~oth.generators;
        microchips &= ~oth.microchips;
        return *this;
    }

    std::unordered_set<Floor> move_candidates() const noexcept;

    std::string to_string() const noexcept { return generators.to_string() + "|" + microchips.to_string(); }

    Materials generators;
    Materials microchips;

};

Floor operator+(Floor lhs, Floor const& rhs) noexcept
{
    lhs.generators |= rhs.generators;
    lhs.microchips |= rhs.microchips;
    return lhs;
}

Floor operator-(Floor lhs, Floor const& rhs) noexcept
{
    lhs.generators &= ~rhs.generators;
    lhs.microchips &= ~rhs.microchips;
    return lhs;
}

struct Floors
{
    Floor& at(unsigned idx) { return data.at(idx); }
    Floor const& at(unsigned idx) const { return data.at(idx); }

    bool operator==(Floors const& oth) const noexcept
    {
        return (elevator == oth.elevator) ? (data == oth.data) : false;
    }

    bool valid() const noexcept
    {
        return std::all_of(data.cbegin(), data.cend(), [](Floor const& f) { return f.valid(); });
    }

    Floors target_state() const
    {
        Floors target = {.elevator = 3};
        for (auto const& f : data) {
            target.data.at(3).generators |= f.generators;
            target.data.at(3).microchips |= f.microchips;
        }
        assert(target.data.at(3).generators == target.data.at(3).microchips);
        target.dump();
        return target;
    }

    void dump() const
    {
        for (unsigned i = 0; i < 4; ++i) {
            fmt::print("F{} ", 4 - i);
            if (elevator == (3 - i)) {
                fmt::print("E ");
            } else {
                fmt::print(". ");
            }
            fmt::print("{}\n", data.at(3 - i).to_string());
        }
    }

    unsigned elevator = 0;
    std::array<Floor, 4> data;
};

namespace std {

template<>
struct hash<Floor>
{
    size_t operator()(Floor const& f) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, std::hash<unsigned long> {}(f.generators.to_ulong()));
        boost::hash_combine(seed, std::hash<unsigned long> {}(f.microchips.to_ulong()));
        return seed;
    }
};

template<>
struct hash<Floors>
{
    size_t operator()(Floors const& fs) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, std::hash<unsigned> {}(fs.elevator));
        boost::hash_combine(seed, std::hash<Floor> {}(fs.data.at(0)));
        boost::hash_combine(seed, std::hash<Floor> {}(fs.data.at(1)));
        boost::hash_combine(seed, std::hash<Floor> {}(fs.data.at(2)));
        boost::hash_combine(seed, std::hash<Floor> {}(fs.data.at(3)));
        return seed;
    }
};

}  // namespace std

std::unordered_set<Floor> Floor::move_candidates() const noexcept
{
    std::unordered_set<Floor> candidates;
    for (unsigned long i = 1; i < (1 << Material_Size); ++i) {
        Materials mask {i};
        if (mask.count() > 2) continue;
        auto gens = (mask & generators);
        auto chips = (mask & microchips);
        if (gens == mask) {
            candidates.insert({.generators = gens});
        }
        if (chips == mask) {
            candidates.insert({.microchips = chips});
        }
        if (gens.count() + chips.count() == 2) {
            candidates.insert({.generators = std::move(gens), .microchips = std::move(chips)});
        }
    }
    return candidates;
}

void part1(const Floors starting_floors)
{
    using EdgeWeight = boost::property<boost::edge_weight_t, unsigned>;
    using VertexDistance = boost::property<boost::vertex_distance_t, unsigned>;
    using Graph = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::directedS,
        VertexDistance,
        EdgeWeight>;
    using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

    Graph g;

    std::unordered_map<Floors, Vertex> floors2vertex;
    std::map<Vertex, Floors> vertex2floors;
    std::unordered_set<Floors> visited;

    std::deque<Floors> to_process;

    const Vertex start_v = boost::add_vertex(VertexDistance {0}, g);
    to_process.push_back(starting_floors);
    floors2vertex.insert({starting_floors, start_v});
    vertex2floors.insert({start_v, starting_floors});

    const Floors target_floors = starting_floors.target_state();
    const Vertex dst_v = boost::add_vertex(VertexDistance {0}, g);
    floors2vertex.insert({target_floors, dst_v});
    vertex2floors.insert({dst_v, target_floors});

    fmt::print("starting at {}\n", start_v);

    while(!to_process.empty()) {
        const auto current_floors = to_process.front();
        to_process.pop_front();

        const Vertex from = floors2vertex.at(current_floors);

        fmt::print("\nstart processing at {}:\n", from);
        current_floors.dump();

        Floor const& current_floor = current_floors.data.at(current_floors.elevator);
        fmt::print("candidates: {}\n", current_floor.move_candidates().size());
        for (auto const& candidate : current_floor.move_candidates()) {
            assert(candidate.valid());

            auto connect_state = [&floors2vertex, &vertex2floors, &g, &from,
                                  &to_process](Floors const& new_floors) {
                if (!new_floors.valid()) {
                    fmt::print("Invalid floors created\n");
                    new_floors.dump();
                    return;
                }
                auto known = floors2vertex.find(new_floors);
                if (known == floors2vertex.end()) {
                    Vertex next = boost::add_vertex(VertexDistance {0}, g);
                    boost::add_edge(from, next, EdgeWeight {1}, g);
                    bool added = floors2vertex.insert({new_floors, next}).second;
                    vertex2floors.insert({next, new_floors});
                    if (!added) {
                        fmt::print("XXX vertex collision!\n");
                    }

                    fmt::print("new floors from {} to {}:\n", from, next);
                    new_floors.dump();

                    to_process.push_back(new_floors);
                    fmt::print("added at {}\n", next);
                } else {
                    boost::add_edge(from, known->second, EdgeWeight {1}, g);
                    fmt::print("known candidate\n");
                }
            };

            fmt::print("candidate: {}\n", candidate.to_string());
            const Floor old_floor = current_floor - candidate;
            if (old_floor.valid() && current_floors.elevator < 3) {
                Floors new_floors = current_floors;
                new_floors.data.at(current_floors.elevator) = old_floor;

                new_floors.elevator += 1;
                new_floors.data.at(new_floors.elevator) += candidate;
                fmt::print("Able to move up:\n");
                connect_state(new_floors);
            }
            if (old_floor.valid() && current_floors.elevator > 0) {
                Floors new_floors = current_floors;
                new_floors.data.at(current_floors.elevator) = old_floor;

                new_floors.elevator -= 1;
                new_floors.data.at(new_floors.elevator) += candidate;
                fmt::print("Able to move down\n");
                connect_state(new_floors);
            }
        }
    }

    fflush(stdout);

    assert(floors2vertex.contains(starting_floors));
    assert(floors2vertex.contains(target_floors));


    auto dist_map = boost::get(boost::vertex_distance, g);
    std::vector<Vertex> pred(boost::num_vertices(g));
    boost::dijkstra_shortest_paths(g, start_v, boost::distance_map(dist_map).predecessor_map(&pred[0]));

    if (dist_map[dst_v] != (std::numeric_limits<decltype(dist_map)::value_type>::max() - 1)) {
        std::deque<Vertex> path;
        {
            for (Vertex x = dst_v; x != start_v; x = pred[x]) {
                path.push_front(x);
            }
            path.push_front(start_v);
        }

        fmt::print("\npath:\n");
        for (auto const& v : path) {
            fmt::print("{}:\n", v);
            vertex2floors.at(v).dump();
        }

        fmt::print("\n1: {}\n", dist_map[dst_v]);
    } else {
        fmt::print("\n1: PATH NOT FOUND\n");
    }

    fflush(stdout);

}

std::string parse_material(const std::string_view mat)
{
    std::vector<std::string> parts;
    boost::algorithm::split(parts, mat, boost::algorithm::is_any_of("-"));
    return parts.at(0);
}

int main()
{
    Floors floors;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        boost::algorithm::erase_all(line, ",.");
        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "));

        unsigned f = 0;
        if (parts.at(1) == "first") {
            f = 0;
        } else if (parts.at(1) == "second") {
            f = 1;
        } else if (parts.at(1) == "third") {
            f = 2;
        } else if (parts.at(1) == "fourth") {
            f = 3;
        } else {
            assert(false);
        }

        if (parts.at(4) != "a") {
            // empty floor
            continue;
        }

        Floor& floor = floors.at(f);

        unsigned pos = 4;
        for (bool last = false; last == false && pos < parts.size();) {
            if (parts.at(pos++) != "a") {
                last = true;
                ++pos;
            }

            const std::string sm = parse_material(parts.at(pos++));
            const Material m = Str2Mat.at(sm);

            if (boost::algorithm::starts_with(parts.at(pos), "microchip")) {
                floor.microchips.set(m);
                fmt::print("level {} add {} microchip\n", f, sm);
            } else if (boost::algorithm::starts_with(parts.at(pos), "generator")) {
                floor.generators.set(m);
                fmt::print("level {} add {} generator\n", f, sm);
            } else {
                assert(false);
            }
            ++pos;
        }
    }

    part1(floors);

    return 0;
}
