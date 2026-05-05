#include <cmath>
#include <algorithm>

#include "rasterize.h"
#include "utilities.h"

using namespace std;

/**
 * @param a:n-dimensional point
 * @param b:n-dimensional point
 * @param d:dimension to step along
 * @return p, vector of all integer points between a and b on dim d
 */
vector<Vertex> DDA(const Vertex& a_,
                    const Vertex& b_,
                    const unsigned int& d)
{
    // error checks
    if (a_.size() != b_.size()) {
        throw invalid_argument("Dimensions of a and b must be the same");
    }
    
    // if the start and end vertices have the same d coord, trivial case
    if (std::fabs(a_.at(d) - b_.at(d)) < 1e-6) { return vector<Vertex>(); }

    vector<Vertex> results;

    Vertex a = a_;
    Vertex b = b_;
    if (a.at(d) > b.at(d)) std::swap(a, b);

    Vertex delta = b - a;
    Vertex s = delta / delta.at(d);
    float e = ceil(a.at(d)) - a.at(d);
    Vertex o = s * e;
    Vertex p = a + o;
    
    while (p.at(d) < b.at(d)) {
        Vertex pix = p;
        pix.at(d) = std::round(pix.at(d));
        results.push_back(pix);
        p = p + s;
    }

    return results;
};

/**
 * @param p: n-dimensional endpoint of triangle with xy coords
 * @param q: n-dimensional endpoint of triangle with xy coords
 * @param r: n-dimensional endpoint of triangle with xy coords
 * @return all points insize the triangle with integer coords
 */
vector<Vertex> scanline(const Vertex& p,
                        const Vertex& q,
                        const Vertex& r,
                        bool hyp)
{
    // error checks
    if (p.size() != q.size() || p.size() != r.size()) {
        throw invalid_argument("Dimensions of p,q, and r must be the same");
    }

    vector<Vertex> pixels;
    
    // find top, middle, bottom vertices
    auto [t, b, m] = minMaxMid(p, q, r);

    vector<Vertex> top_edge = DDA(t, m, 1);
    vector<Vertex> bottom_edge = DDA(m, b, 1);
    vector<Vertex> full_edge = DDA(t, b, 1);
    vector<Vertex> full_edge_bottom;
    for (auto& v: full_edge) {
        if (v.at(1) >= m.at(1)) {
            full_edge_bottom.push_back(v);
        }
    }

    // fill between edges from left to right
    bool full_edge_left = (top_edge.size() > 0) ? (full_edge.front().at(0) < top_edge.front().at(0)) : full_edge.front().at(0) < bottom_edge.front().at(0);
    if (full_edge_left) {
        FillBetweenEdges(full_edge, top_edge, pixels, hyp);
        FillBetweenEdges(full_edge_bottom, bottom_edge, pixels, hyp);
    } else {
        FillBetweenEdges(top_edge, full_edge, pixels, hyp);
        FillBetweenEdges(bottom_edge, full_edge_bottom, pixels, hyp);
    }

    return pixels;
};

/**
 * @todo apply perspective correction
 */
void FillBetweenEdges(const vector<Vertex>& left_edge,
                    const vector<Vertex>& right_edge,
                    vector<Vertex>& pixels,
                    bool hyp)
{
    // trivial case
    if (left_edge.size() == 0 || right_edge.size() == 0) {
        cout << "One of the edges is empty, cause of concern? (not concerned when two edges start at same y level like in smallgap)" << endl;
        return;
    }

    size_t n = std::min(left_edge.size(), right_edge.size());
    for (size_t i=0; i<n; i++) {
        Vertex left = left_edge[i];
        Vertex right = right_edge[i];

        // TODO: test perspective correction
        // Apply perspective correction
        if (hyp) {
            for (size_t j=4; j<left.size(); j++) {
                left.at(j) *= left.at(3);
                right.at(j) *= right.at(3);
            }
        }

        float l_test_rgb = left.at(4)/left.at(7);
        float r_test_rgb = right.at(4)/right.at(7);

        vector<Vertex> row = DDA(left, right, 0);

        // Recover perspective-correct attributes
        if (hyp) {
            for (auto& v : row) {
                for (size_t j=4; j<v.size(); j++) {
                    v.at(j) /= v.at(3);
                }
            }
        }

        pixels.insert(pixels.end(), row.begin(), row.end());
    }
};

tuple<Vertex, Vertex, Vertex> minMaxMid(const Vertex& p,
                                         const Vertex& q,
                                         const Vertex& r)
{
    Vertex t = p, m = q, b = r;
    if (t.at(1) > m.at(1)) std::swap(t, m);
    if (t.at(1) > b.at(1)) std::swap(t, b);
    if (m.at(1) > b.at(1)) std::swap(m, b);
    return  {t, b, m};
};