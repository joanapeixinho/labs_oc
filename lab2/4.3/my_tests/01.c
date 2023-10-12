#include "../SimpleCache.h"

int main() {
  // Initialize cache
  resetTime();
  initCache();

  uint32_t value1 = 1, value0 = 0;
  write(0, (unsigned char *)(&value0));
  //write(0, (unsigned char *)(&value1));

  return 0;
}
