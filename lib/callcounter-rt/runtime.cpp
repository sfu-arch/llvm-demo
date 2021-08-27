
#include <cstdint>
#include <cstdio>
#include <unordered_map>

std::unordered_map<int, int> in_out_map;


extern "C" {

void MEMOIZER_insertEntry(int key, int value) {
  in_out_map[key] = value;
}

int MEMOIZER_existEntry(int key) {
  return in_out_map.count(key);
}

int MEMOIZER_getEntry(int key) {
  return in_out_map[key];
}

}
