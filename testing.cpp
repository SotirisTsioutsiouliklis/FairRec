#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>


int main() {
   int yo [4] = {3};
   std::cout << yo[2] << std::endl;
   yo[2] = 4;
   std::cout << yo[2] << std::endl;
   yo [4] = {0};
   std::cout << yo[2] << std::endl;
}