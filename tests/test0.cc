// TEST 0

#include "BasicMPLcppProject.hh"

int main(void)
{

  std::cout << "TEST 0"
            << std::endl
            << std::endl;

  // EPSILON TEST
  std::cout << "Epsilon TEST" << std::endl
            << "Epsilon (machine) = " << mpl::EPSILON_MACHINE << std::endl
            << "Epsilon (mp)      = " << mpl::EPSILON << std::endl
            << std::endl;

  // INFINITY TEST
  std::cout << "INFINITY TEST" << std::endl
            << "Infinity (mp)     = " << mpl::INFTY << std::endl
            << std::endl;

  // NOT-A-NUMBER TEST
  std::cout << "QUIET NOT-A-NUMBER TEST" << std::endl
            << "Quiet NaN (mp)     = " << mpl::QUIET_NAN << std::endl
            << std::endl;

  return 0;
}