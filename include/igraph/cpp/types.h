/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef IGRAPHPP_TYPES_H
#define IGRAPHPP_TYPES_H

namespace igraph {

#define IGRAPH_TRY(call) {\
    int __result = call;    \
    if (__result != 0)      \
        throw std::runtime_error("igraph error"); \
}

typedef igraph_integer_t integer_t;
typedef igraph_real_t real_t;
typedef igraph_neimode_t NeighborMode;

}       // end of namespaces

#endif  // IGRAPHPP_VECTOR_H


