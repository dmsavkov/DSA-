#include <iostream>
#include <string>

#include "Gauss_solve.h"
#include "util.h"

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <augmented_matrix.csv>\n";
        return 1;
    }

    try
    {
        GaussMatrix ab = load_csv_to_matrix(argv[1]);
        GaussVector solution = Gauss_solve(ab);
        print_solution_csv(std::cout, solution);
    }
    catch (std::exception const &ex)
    {
        std::cerr << ex.what() << '\n';
        return 1;
    }

    return 0;
}
