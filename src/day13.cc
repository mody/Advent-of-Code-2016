#include "point2d.h"

#include <bitset>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths_no_color_map.hpp>

#include <boost/graph/graph_selectors.hpp>
#include <fmt/core.h>

#include <string>
#include <unordered_set>

using Coord = unsigned;
using Point = Gfx_2d::Point<Coord>;

struct World
{
    World(const Coord _fav, const Coord _max_x, const Coord _max_y)
        : fav {_fav}
        , max_x {_max_x}
        , max_y {_max_y}
    {
        for (Coord y = 0; y < max_y; ++y) {
            const uint64_t v1 = y + y * y;
            for (Coord x = 0; x < max_x; ++x) {
                const uint64_t v2 = fav + (x * x + 3 * x + 2 * x * y + v1);
                std::bitset<64> v3 = v2;
                if (v3.count() & 0x1) {
                    // odd, wall
                } else {
                    data.insert({x, y});
                }
            }
        }
    }

    void dump() const noexcept
    {
        for (Coord y = 0; y < max_y; ++y) {
            for (Coord x = 0; x < max_x; ++x) {
                if (data.contains({x, y})) {
                    fmt::print(".");
                } else {
                    fmt::print("#");
                }
            }
            fmt::print("\n");
        }
    }

    void do_work(const Coord dst_x, const Coord dst_y) const
    {
        using EdgeWeight = boost::property<boost::edge_weight_t, unsigned>;
        using VertexDistance = boost::property<boost::vertex_distance_t, unsigned>;
        using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexDistance, EdgeWeight>;
        using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

        Graph g;

        std::unordered_map<Point, Vertex> point2vertex;

        for (Coord y = 0; y < max_y; ++y) {
            for (Coord x = 0; x < max_x; ++x) {
                const Point px{x, y};
                if (data.contains(px)) {
                    point2vertex.insert({px, boost::add_vertex(g)});
                }
            }
        }

        for (Point const& px : data) {
            const Vertex from = point2vertex.at(px);
            for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right}) {
                auto i2 = point2vertex.find(px + dir);
                if (i2 != point2vertex.end()) {
                    boost::add_edge(from, i2->second, EdgeWeight {1}, g);
                }
            }
        }

        const Vertex start_v = point2vertex.at({1,1}), dst_v = point2vertex.at({dst_x, dst_y});

        auto dist_map = boost::get(boost::vertex_distance, g);
        std::vector<Vertex> pred(boost::num_vertices(g));
        boost::dijkstra_shortest_paths_no_color_map(g, start_v, boost::distance_map(dist_map).predecessor_map(&pred[0]));
        fmt::print("1: {}\n", dist_map[dst_v]);

        unsigned cnt = 0;
        for (auto const& [_,v] : point2vertex) {
            if (dist_map[v] < 51) {
                ++cnt;
            }
        }
        fmt::print("2: {}\n", cnt);
    }

    std::unordered_set<Point> data;
    Coord fav, max_x, max_y;
};

int main()
{
    World w(1352, 100, 100);
    w.do_work(31, 39);

    return 0;
}
