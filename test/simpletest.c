
#include <stdio.h>

float milad (float s) {
  int a = 2, b = 1;
  int c = a  + b;
  int d = c / 2;
  return s + 1.1;
}

template <typename T>
T gg (T s) {
  int a = 2, b = 1;
  int c = a  + b;
  int d = c / 2;
  return s + 1;
}

void a() { printf("Found a\n"); }
void b() { printf("Found b\n"); }
int main() {
  b();
  a();
  b();
  for (int i = 0; i < 10; i++) {
    printf("%f\n", milad((float)i));
  }

  for (int i = 0; i < 10; i++) 
    printf("%f\n", milad((float)i));

  printf("Done!\n");
  return 0;
}

