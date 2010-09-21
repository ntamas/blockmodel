/* vim:set ts=4 sw=4 sts=4 et: */

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sstream>
#include <stdexcept>
#include <string>
#include <block/io.hpp>
#include <block/util.hpp>
#include <igraph/cpp/graph.h>

using namespace igraph;
using namespace std;

/***************************************************************************/

template<>
void PlainTextReader<UndirectedBlockmodel>::read(
        UndirectedBlockmodel& model, istream& is) {
    enum { SECTION_INFO, SECTION_TYPES, SECTION_PROBABILITIES, SECTION_UNKNOWN } section;
    bool finished = false;
    Vector types;
    Vector probabilities;

    string currentHeading;
    string line;
    stringstream iss;

    string name;
    int type;
    double p;

    if (is.fail())
        throw runtime_error("error while reading stream");

    section = SECTION_UNKNOWN;

    while (!finished) {
        getline(is, line);
        if (is.eof())
            break;
        if (is.fail())
            throw runtime_error("error while reading stream");

        /* Is the line empty? If so, a new heading will follow */
        if (line.size() == 0) {
            currentHeading = "";
            continue;
        }

        /* Are we looking for a heading? If so, store it */
        if (currentHeading == "") {
            currentHeading = line;
            if (currentHeading == "INFO")
                section = SECTION_INFO;
            else if (currentHeading == "PROBABILITIES")
                section = SECTION_PROBABILITIES;
            else if (currentHeading == "TYPES")
                section = SECTION_TYPES;
            else
                section = SECTION_UNKNOWN;
            continue;
        }

        /* If we are here, we have the heading and the line is not empty */
        switch (section) {
            case SECTION_INFO:
                /* Nothing to do, we don't need anything from here */
                break;

            case SECTION_TYPES:
                /* Store the next type */
                iss.str(line);
                iss >> name >> type;
                if (iss.bad())
                    throw runtime_error("error while parsing probability matrix");
                types.push_back(type);
                iss.clear();
                break;

            case SECTION_PROBABILITIES:
                /* Store the next row in the probability matrix */
                iss.str(line);
                while (!iss.eof()) {
                    iss >> p;
                    if (iss.bad())
                        throw runtime_error("error while parsing probability matrix");
                    probabilities.push_back(p);
                }
                iss.clear();
                break;

            default:
                break;
        }
    }

    // Find how many types do we have
    size_t n = sqrt(probabilities.size());
    if (n * n != probabilities.size())
        throw runtime_error("probability matrix must be square");

    if (types.min() < 0)
        throw runtime_error("negative type index found");
    if (types.max() >= n)
        throw runtime_error("too large type index found");

    Matrix probMat(n, n);
    copy(probabilities.begin(), probabilities.end(), probMat.begin());

    model.setTypes(types);
    model.setProbabilities(probMat);
}

/***************************************************************************/

template<>
void PlainTextWriter<UndirectedBlockmodel>::write(
        const UndirectedBlockmodel& model, ostream& os) {
    const Graph* pGraph = model.getGraph();
    int n = pGraph->vcount();
    int k = model.getNumTypes();
    time_t now = time(0);

    os << "INFO\n";
    os << "date\t" << ctime(&now);
    if (pGraph->hasAttribute("filename"))
        os << "filename\t" << pGraph->getAttribute("filename").as<std::string>() << '\n';
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
        const UndirectedBlockmodel& model, ostream& os) {
    const Graph* pGraph = model.getGraph();
    int n = pGraph->vcount();
    int k = model.getNumTypes();
    time_t timestamp = time(0);
    char* formatted_date = ctime(&timestamp);

    // Trim the newline from the formatted date
    formatted_date[strlen(formatted_date)-1] = 0;

    os << "{\n"
       << "    \"info\": {\n"
       << "        \"date\": \"" << formatted_date << "\",\n"
       << "        \"timestamp\": " << timestamp << ",\n"
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

