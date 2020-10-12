#include <algorithm>
#include <random>
#include <vector>
// Given some container `container`...

int main() {
    std::vector<int> list;
    list.push_back(3);
     list.push_back(4);
      list.push_back(5);
       list.push_back(6);
  std::shuffle(list.begin(), list.end(), std::random_device());
}
