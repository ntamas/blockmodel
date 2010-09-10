/* vim:set ts=4 sw=4 sts=4 et: */

#include <block/io.hpp>
#include <block/util.hpp>
#include <igraph/cpp/graph.h>

using namespace igraph;

template<>
void PlainTextWriter<UndirectedBlockmodel>::write(
        const UndirectedBlockmodel& model, std::ostream& os) {
    const Graph* pGraph = model.getGraph();
    int n = pGraph->vcount();
    int k = model.getNumTypes();

    os << "INFO\n";
    os << "num_vertices\t" << n << '\n';
    os << "num_types\t" << k << '\n';
    os << "log_likelihood\t" << model.getLogLikelihood() << '\n';
    os << "aic\t" << 2 * (model.getNumParameters() - model.getLogLikelihood()) << '\n';
    os << '\n';

    os << "TYPES\n";
    for (int i = 0; i < n; i++)
        os << i << '\t' << model.getType(i) << '\n';
    os << '\n';

    os << "PROBABILITIES\n";
    for (int i = 0; i < k; i++) {
        os << model.getProbability(i, 0);
        for (int j = 1; j < k; j++)
            os << '\t' << model.getProbability(i, j);
        os << '\n';
    }
}

template<>
void JSONWriter<UndirectedBlockmodel>::write(
        const UndirectedBlockmodel& model, std::ostream& os) {
    const Graph* pGraph = model.getGraph();
    int n = pGraph->vcount();
    int k = model.getNumTypes();

    os << "{\n"
       << "    \"info\": {\n"
       << "        \"num_vertices\": " << n << ",\n"
       << "        \"num_types\": " << k << ",\n"
       << "        \"log_likelihood\": " << model.getLogLikelihood() << ",\n"
       << "        \"aic\": " << aic(model) << "\n"
       << "    },\n"
       << "    \"parameters\": {\n"
       << "        \"types\": [";
    for (int i = 0; i < n-1; i++) {
        if (i % 15 == 0)
            os << "\n                  ";
        os << model.getType(i) << ", ";
    }
    if (n > 0)
        os << model.getType(n-1);
    os << "\n        ],\n";
    os << "        \"p\": [";
    if (k > 0) {
        for (int i = 0; i < k; i++) {
            os << "\n              [";
            for (int j = 0; j < k-1; j++) {
                os << model.getProbability(i, j) << ", ";
            }
            os << model.getProbability(i, k-1) << "]";
            if (i < k-1)
                os << ", ";
        }
    }
    os << "\n        ]\n"
       << "    }\n"
       << "}\n";
}

