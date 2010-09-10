/* vim:set ts=4 sw=4 sts=4 et: */

#ifndef BLOCKMODEL_UTIL_HPP
#define BLOCKMODEL_UTIL_HPP

/// Calculates the Akaike information criterion of some model
template <typename T>
double aic(const T& model) {
    return 2 * (model.getNumParameters() - model.getLogLikelihood());
}

#endif

