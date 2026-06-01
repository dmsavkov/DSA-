#include "Gauss_solve.h"

#include <cmath>
#include <stdexcept>

GaussVector Gauss_solve(GaussMatrix &ab)
{
    Eigen::Index const n = ab.rows();
    if (n == 0 || ab.cols() != n + 1)
    {
        throw std::runtime_error("Matrix must be Nx(N+1)");
    }

    constexpr double pivot_eps = 1e-9;
    GaussVector solution(n);

    for (Eigen::Index i = 0; i < n; ++i)
    {
        Eigen::Index pivot_row = i;
        double max_abs = std::abs(ab(i, i));
        for (Eigen::Index k = i + 1; k < n; ++k)
        {
            double const v = std::abs(ab(k, i));
            if (v > max_abs)
            {
                max_abs = v;
                pivot_row = k;
            }
        }

        if (max_abs < pivot_eps)
        {
            throw std::runtime_error("No unique solution");
        }

        if (pivot_row != i)
        {
            ab.row(i).swap(ab.row(pivot_row));
        }

        for (Eigen::Index j = i + 1; j < n; ++j)
        {
            double const multiplier = ab(j, i) / ab(i, i);
            ab.row(j) -= multiplier * ab.row(i);
        }
    }

    for (Eigen::Index i = n - 1; i >= 0; --i)
    {
        double sum = ab(i, n);
        for (Eigen::Index j = i + 1; j < n; ++j)
        {
            sum -= ab(i, j) * solution(j);
        }
        solution(i) = sum / ab(i, i);
    }

    return solution;
}
