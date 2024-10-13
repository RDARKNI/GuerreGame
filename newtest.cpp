#include <iostream>
#include <optional>
int main() {
  std::optional a = std::optional<int>();
  std::cout << sizeof(a) << std::endl;
}