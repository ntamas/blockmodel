/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef _GRAPH_UTIL_H

#include <stdexcept>
#include <igraph/cpp/graph.h>

/// Supported formats
typedef enum {
    GRAPH_FORMAT_AUTO,
    GRAPH_FORMAT_UNKNOWN,
    GRAPH_FORMAT_EDGELIST,
    GRAPH_FORMAT_NCOL
} GraphFormat;

/// Exception thrown when the format of a graph is unknown
class UnknownGraphFormatException : public std::exception {
private:
    std::string m_msg;

public:
    UnknownGraphFormatException(const std::string& filename = "");
    ~UnknownGraphFormatException() throw() {}
    const char* what() const throw() {
        return m_msg.c_str();
    }
};

class GraphUtil {
public:
    /// Tries to detect the format of a graph from its filename
    static GraphFormat detectFormat(const std::string& filename);

    /// Reads a graph without having to know what format it is in
    static igraph::Graph readGraph(const std::string& filename,
            GraphFormat format = GRAPH_FORMAT_AUTO);

    /// Reads a graph from the given stream using the given format
    static igraph::Graph readGraph(FILE* fptr, GraphFormat format);
};

#endif       // _GRAPH_UTIL_H
