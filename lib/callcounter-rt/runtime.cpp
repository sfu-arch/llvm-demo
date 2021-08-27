
#include <cstdint>
#include <cstdio>
#include <unordered_map>

std::unordered_map<int, int> in_out_map;


extern "C" {

void MEMOIZER_insertData(int key, int value) {
  in_out_map[key] = value;
}

}
