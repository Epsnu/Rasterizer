#ifndef RASTERIZE_H
#define RASTERIZE_H

#include <vector>

#include "classes.h"


std::vector<Vertex> DDA(const Vertex& a_,
                        const Vertex& b_,
                        const unsigned int& d
);

std::vector<Vertex> scanline(const Vertex& p,
                            const Vertex& q,
                            const Vertex& r,
                            bool hyp = false
);

void FillBetweenEdges(const std::vector<Vertex>& left_edge,
                        const std::vector<Vertex>& right_edge,
                        std::vector<Vertex>& pixels,
                        bool hyp = false
);

std::tuple<Vertex, Vertex, Vertex> minMaxMid(const Vertex& p,
                            const Vertex& q,
                            const Vertex& r
);

// std::pair<Vertex,Vertex> scanlineDDAHelper(Vertex a,
//                                         Vertex b,
//                                         const unsigned int& d
// );

// Vertex hyperbolicInterpolation(const std::vector<Vertex>& points);
// std::vector<float> computeBarycentricCoords(const Vertex& p,
//                                                 const Vertex& q,
//                                                 const Vertex& r,
//                                                 const Vertex& point
// );

#endif