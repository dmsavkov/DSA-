#include <gtest/gtest.h>

#include <cmath>
#include <sstream>
#include <stdexcept>

#include "Gauss_solve.h"
#include "util.h"

namespace
{

GaussMatrix make_augmented(std::initializer_list<std::initializer_list<double>> rows)
{
    Eigen::Index const n = static_cast<Eigen::Index>(rows.size());
    Eigen::Index const m = static_cast<Eigen::Index>(rows.begin()->size());
    GaussMatrix ab(n, m);
    Eigen::Index i = 0;
    for (auto const &row : rows)
    {
        Eigen::Index j = 0;
        for (double v : row)
        {
            ab(i, j++) = v;
        }
        ++i;
    }
    return ab;
}

} // namespace

TEST(GaussSolve, Small2x2)
{
    GaussMatrix ab = make_augmented({{2, 1, 5}, {1, 3, 7}});
    GaussVector x = Gauss_solve(ab);
    ASSERT_EQ(x.size(), 2);
    EXPECT_NEAR(x(0), 1.6, 1e-9);
    EXPECT_NEAR(x(1), 1.8, 1e-9);
}

TEST(GaussSolve, Identity3x3)
{
    GaussMatrix ab = make_augmented(
    {
        {1, 0, 0, 2},
        {0, 1, 0, 3},
        {0, 0, 1, 4},
    });
    GaussVector x = Gauss_solve(ab);
    ASSERT_EQ(x.size(), 3);
    EXPECT_NEAR(x(0), 2.0, 1e-9);
    EXPECT_NEAR(x(1), 3.0, 1e-9);
    EXPECT_NEAR(x(2), 4.0, 1e-9);
}

TEST(GaussSolve, SingularThrows)
{
    GaussMatrix ab = make_augmented({{1, 2, 3}, {2, 4, 6}});
    EXPECT_THROW(Gauss_solve(ab), std::runtime_error);
}

TEST(GaussUtil, LoadCsvFixture)
{
    GaussMatrix ab = load_csv_to_matrix("system_2x2.csv");
    ASSERT_EQ(ab.rows(), 2);
    ASSERT_EQ(ab.cols(), 3);
    GaussVector x = Gauss_solve(ab);
    EXPECT_NEAR(x(0), 1.6, 1e-9);
    EXPECT_NEAR(x(1), 1.8, 1e-9);
}

TEST(GaussUtil, PrintSolutionFormat)
{
    GaussVector x(2);
    x << 1.25, -2.5;
    std::ostringstream oss;
    print_solution_csv(oss, x);
    EXPECT_EQ(oss.str().substr(0, 2), "x\n");
    std::istringstream in(oss.str());
    std::string header;
    std::getline(in, header);
    ASSERT_EQ(header, "x");
    double a = 0;
    double b = 0;
    in >> a >> b;
    EXPECT_NEAR(a, 1.25, 1e-14);
    EXPECT_NEAR(b, -2.5, 1e-14);
}
