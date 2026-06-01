#include "util.h"

#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <lazycsv.hpp>

namespace
{

bool is_rectangular(std::vector<std::vector<double>> const &rows)
{
    if (rows.empty())
    {
        return false;
    }
    size_t const width = rows.front().size();
    if (width == 0)
    {
        return false;
    }
    for (auto const &row : rows)
    {
        if (row.size() != width)
        {
            return false;
        }
    }
    return true;
}

} // namespace

GaussMatrix load_csv_to_matrix(const char *filename)
{
    std::vector<std::vector<double>> rows;
    lazycsv::parser parser{filename};
    for (auto const row : parser)
    {
        std::vector<double> values;
        bool numeric_row = true;
        for (auto const cell : row)
        {
            try
            {
                values.push_back(std::stod(std::string(cell.raw())));
            }
            catch (std::exception const &)
            {
                numeric_row = false;
                break;
            }
        }
        if (numeric_row && !values.empty())
        {
            rows.push_back(values);
        }
    }

    if (!is_rectangular(rows))
    {
        throw std::runtime_error("CSV must be a rectangular numeric table");
    }

    Eigen::Index const n = static_cast<Eigen::Index>(rows.size());
    Eigen::Index const m = static_cast<Eigen::Index>(rows.front().size());
    if (n == 0 || m < 2)
    {
        throw std::runtime_error("Augmented matrix must have at least one row and two columns");
    }
    if (m != n + 1)
    {
        throw std::runtime_error("Expected N rows and N+1 columns (augmented matrix)");
    }

    GaussMatrix matrix(n, m);
    for (Eigen::Index i = 0; i < n; ++i)
    {
        for (Eigen::Index j = 0; j < m; ++j)
        {
            matrix(i, j) = rows[static_cast<size_t>(i)][static_cast<size_t>(j)];
        }
    }
    return matrix;
}

void print_solution_csv(std::ostream &out, GaussVector const &solution, int prec)
{
    out << "x\n";
    char buf[64];
    char fmt[16];
    std::snprintf(fmt, sizeof(fmt), "%%.%dg", prec);
    for (Eigen::Index i = 0; i < solution.size(); ++i)
    {
        std::snprintf(buf, sizeof(buf), fmt, solution(i));
        out << buf << '\n';
    }
}
