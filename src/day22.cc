#include "point2d.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/regex.hpp>
#include <fmt/core.h>

#include <cassert>
#include <deque>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

using Coord = unsigned;
using Point = Gfx_2d::Point<Coord>;

struct Node {
    Node(unsigned sz, unsigned usd) noexcept
        : _size {sz}
        , _used {usd}
        , _important {false}
    { }

    Node(Node const&) = default;
    Node(Node&&) = default;

    Node& operator=(Node const&) = default;
    Node& operator=(Node&&) = default;

    bool operator== (Node const& oth) const noexcept {
        return std::tie(_size, _used, _important) == std::tie(oth._size, oth._used, oth._important);
    }

    void move_from(Node& oth) noexcept {
        assert(avail() >= oth._used);
        assert(_used == 0);

        _used += oth._used;
        oth._used = 0;

        _important = oth._important;
        oth._important = false;
    }

    unsigned size() const noexcept { return _size; }
    unsigned used() const noexcept { return _used; }
    unsigned avail() const noexcept { return _size - _used; }

    bool important() const noexcept { return _important; }
    void set_important(bool v = true) noexcept { _important = v; }

private:
    unsigned _size = 0, _used = 0;
    bool _important = false;
};
using Disk = std::unordered_map<Point, Node>;

using PointPair = std::pair<Point, Point>;

template<>
struct std::hash<Node>
{
    size_t operator() (Node const& n) const noexcept {
        size_t seed = 2016;
        boost::hash_combine(seed, std::hash<unsigned> {}(n.size()));
        boost::hash_combine(seed, std::hash<unsigned> {}(n.used()));
        boost::hash_combine(seed, std::hash<unsigned> {}(n.avail()));
        boost::hash_combine(seed, std::hash<bool> {}(n.important()));
        return seed;
    }
};

template<>
struct std::hash<Disk>
{
    size_t operator() (Disk const& disk) const noexcept {
        size_t seed = 2016;
        Point zero, data;
        for (auto const& [px, n1] : disk) {
            if (n1.used() == 0) {
                zero = px;
            }
            if (n1.important()) {
                data = px;
            }
        }
        boost::hash_combine(seed, std::hash<Point> {}(zero));
        boost::hash_combine(seed, std::hash<Point> {}(data));
        return seed;
    }
};

template<>
struct std::hash<PointPair>
{
    size_t operator() (PointPair const& p) const noexcept {
        size_t seed = 2016;
        boost::hash_combine(seed, std::hash<Point> {}(p.first));
        boost::hash_combine(seed, std::hash<Point> {}(p.second));
        return seed;
    }
};

void part1(Disk const& disk)
{
    std::unordered_set<PointPair> pairs;

    for (auto const& [px1, n1] : disk) {
        for (auto const& [px2, n2] : disk) {
            if (px1 == px2 || n2.used() == 0) continue;
            if (n1.avail() >= n2.used()) {
                if (px1 < px2) {
                    pairs.insert({px1, px2});
                } else {
                    pairs.insert({px2, px1});
                }
            }
        }
    }
    fmt::print("1: {}\n", pairs.size());
}

[[maybe_unused]] void dump(Disk const& d)
{
    unsigned max_x = 0, max_y = 0;
    for (auto const& [px, n] : d) {
        max_x = std::max(max_x, px.x);
        max_y = std::max(max_y, px.y);
    }
    for (unsigned y = 0; y <= max_y; ++y) {
        for (unsigned x = 0; x <= max_x; ++x) {
            if (d.at({x, y}).important()) {
                fmt::print("G");
            } else if (d.at({x, y}).used() == 0) {
                fmt::print("_");
            } else {
                fmt::print(".");
            }
        }
        fmt::print("\n");
    }
}

void part2(Disk const& input)
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


    Point start;
    for (auto const& [px, n1] : input) {
        if (n1.used() == 0) {
            start = px;
            break;
        }
    }

    using WorkItem = std::tuple<Point, Disk>;
    std::deque<WorkItem> work_queue;
    work_queue.push_back({start, input});

    Graph g;

    std::unordered_map<size_t, Vertex> disk2vertex;
    Vertex from_v = boost::add_vertex(VertexDistance {1}, g);
    std::vector<Vertex> dst_v;

    std::hash<Disk> hash_disk{};
    disk2vertex.insert({hash_disk(input), from_v});

    while (!work_queue.empty()) {
        const auto current = work_queue.front();
        work_queue.pop_front();

        const Point& px = std::get<Point>(current);
        const Disk& disk = std::get<Disk>(current);
        const auto from = disk2vertex.at(hash_disk(disk));

        assert(disk.at(px).used() == 0);
        assert(disk.at(px).important() == false);

        // detect destination
        if (disk.at({0,0}).important()) {
            dst_v.push_back(from);
            continue;
        }

        for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right}) {
            Point next_px = px + dir;

            if (!disk.contains(next_px)) continue; // of bounds
            if (disk.at(px).avail() < disk.at(next_px).used()) continue; // not enough space

            Disk next_disk = disk;
            next_disk.at(px).move_from(next_disk.at(next_px));

            assert(next_disk.at(px).used() > 0);
            assert(next_disk.at(next_px).used() == 0);

            auto next_hash = hash_disk(next_disk);
            auto dit = disk2vertex.find(next_hash);
            if (dit == disk2vertex.end()) {
                const auto to = boost::add_vertex(VertexDistance {1}, g);
                bool ok = disk2vertex.insert({next_hash, to}).second;
                assert(ok);
                boost::add_edge(from, to, EdgeWeight{1}, g);
                work_queue.push_back({next_px, next_disk});
            } else {
                boost::add_edge(from, dit->second, EdgeWeight{1}, g);
            }
        }
    }

    auto dist_map = boost::get(boost::vertex_distance, g);
    std::vector<Vertex> pred(boost::num_vertices(g));
    boost::dijkstra_shortest_paths(g, from_v, boost::distance_map(dist_map).predecessor_map(&pred[0]));

    if (dst_v.size() == 0) {
        fmt::print("End state NOT found!\n");
    } else {
        unsigned shortest = -1;
        for (auto v : dst_v) {
            shortest = std::min(shortest, dist_map[v]);
        }
        fmt::print("2: {}\n", shortest);

        // {
        //     std::vector<Vertex> path;
        //     for (Vertex i = shortest_v; i != from_v; i = pred[i]) {
        //         path.push_back(i);
        //     }
        //     path.push_back(from_v);
        //     std::reverse(path.begin(), path.end());

        //     for (auto const& v : path) {
        //         fmt::print("\nVertex {}\n", v);
        //         for (auto const& [d, vx] : disk2vertex) {
        //             if (vx == v) {
        //                 dump(d);
        //                 break;
        //             }
        //         }
        //     }
        // }
    }
}

int main()
{
    static const boost::regex RX("^/dev/grid/node-x([0-9]+)-y([0-9]+) +([0-9]+)T +([0-9]+)T +([0-9]+)T +([0-9]+)%$");

    Disk disk;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) break;

        boost::smatch m;
        if (!boost::regex_match(line, m, RX)) {
            continue;
        }

        Point px;
        px.x = std::stoi(m.str(1));
        px.y = std::stoi(m.str(2));

        Node n(std::stoi(m.str(3)), std::stoi(m.str(4)));

        disk.insert({std::move(px), std::move(n)});
    };

    part1(disk);

    Point top_right{};
    for (auto& [px, n] : disk) {
        if (px.y == 0 && top_right.x < px.x) {
            top_right = px;
        }
    }
    disk.at(top_right).set_important();

    part2(disk);

    return 0;
}
