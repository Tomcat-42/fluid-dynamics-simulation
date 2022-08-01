#include "algorithm"

int IX(int x, int y, int N) {
  int i = std::clamp(x, 0, N - 1);
  int j = std::clamp(y, 0, N - 1);

  return (j * N) + i;
}
