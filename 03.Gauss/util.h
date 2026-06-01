#ifndef GAUSS_UTIL_H
#define GAUSS_UTIL_H

#include <iosfwd>

#include <Eigen/Dense>

typedef Eigen::VectorXd GaussVector;
typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> GaussMatrix;

GaussMatrix load_csv_to_matrix(const char *filename);
void print_solution_csv(std::ostream &out, GaussVector const &solution, int prec = 15);

#endif
