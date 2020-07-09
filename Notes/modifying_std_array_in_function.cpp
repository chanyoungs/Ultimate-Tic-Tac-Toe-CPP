#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <array>

using namespace std;

void f(std::array<int, 2> *A)
{
  (*A)[0] = 10;
}

int main()
{
  std::array<int, 2> A{0, 0};
  cout << "Before: " << A[0] << endl;
  f(&A);
  cout << "After: " << A[0];
  return 0;
}
