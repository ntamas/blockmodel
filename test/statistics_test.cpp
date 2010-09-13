/* vim:set ts=4 sw=4 sts=4 et: */

#include <block/statistics.h>
#include <igraph/cpp/vector.h>

#include "test_common.cpp"

using igraph::Vector;

int test_mann_whitney_test() {
    {
        double xA_data[] = { 7, 3, 6, 2, 4, 3, 5, 5 };
        double xB_data[] = { 3, 5, 6, 4, 6, 5, 7, 5 };
        Vector xA(8, xA_data);
        Vector xB(8, xB_data);
        MannWhitneyTest test(xA, xB);
        if (test.getTestStatistic() != 23)
            return 1;
        if (!ALMOST_EQUALS(test.getP(), 0.333, 0.01))
            return 101;
        if (!ALMOST_EQUALS(MannWhitneyTest(xA, xB, LESS_THAN).getP(),
                    0.166, 0.01))
            return 102;
        if (!ALMOST_EQUALS(MannWhitneyTest(xA, xB, GREATER_THAN).getP(),
                    0.833, 0.01))
            return 103;
    }

    {
        double xA_data[] = { 19, 22, 16, 29, 24 };
        double xB_data[] = { 20, 11, 17, 12 };
        Vector xA(5, xA_data);
        Vector xB(4, xB_data);
        MannWhitneyTest test(xA, xB);
        if (test.getTestStatistic() != 3)
            return 2;
        if (!ALMOST_EQUALS(test.getP(), 0.08641, 0.0001))
            return 104;
    }

    {
        double xA_data[] = { 1, 1, 1 };
        double xB_data[] = { 0, 0, 0 };
        Vector xA(3, xA_data);
        Vector xB(3, xB_data);
        MannWhitneyTest test(xA, xB);
        if (test.getTestStatistic() != 0)
            return 3;
    }

    {
        double data[] = { 1, 1, 1 };
        Vector xA(3, data);
        Vector xB = xA;

        MannWhitneyTest test(xA, xB);
        if (test.getP() == test.getP())   /* testing for nan */
            return 105;
        test = MannWhitneyTest(xA, xB, GREATER_THAN);
        if (test.getP() == test.getP())   /* testing for nan */
            return 106;
        test = MannWhitneyTest(xA, xB, LESS_THAN);
        if (test.getP() == test.getP())   /* testing for nan */
            return 107;
    }

    return 0;
}

int main(int argc, char* argv[]) {
    CHECK(test_mann_whitney_test);
}
        

