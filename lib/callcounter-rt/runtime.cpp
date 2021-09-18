#include "utilities.h"
#include <set>
#include <vector>


template<typename T1>
std::unordered_map<T1, value_store> m;

template<typename T1>
std::unordered_map<T1, std::vector<value_store>> output_map;

int hit_count = 0;
int total_count = 0;
int total_out = 0;
int unique_out = 0;

template<typename T1>
inline int check_entry(T1 e) {
  // std::cout << "total count " << total_count << ", hit count: " << hit_count << std::endl;
  total_count++;
  return m<T1>.count(e) > 0;
}

template<typename T1>
inline void insert_entry(T1 key, value_store val) {
  m<T1>[key] = val;
  total_out = m<T1>.size();
}

template<typename T1>
inline void insert_output_entry(T1 key, value_store val) {
  if (output_map<T1>.count(key) == 0) {
    output_map<T1>[key] = std::vector<value_store>();
  }
  output_map<T1>[key].push_back(val);
  unique_out = output_map<T1>.size();
}

template<typename T1>
inline void *get_entry(T1 key) {
  hit_count++;
  return &(m<T1>[key]);
}


extern "C" {

void __attribute__((always_inline)) MEMOIZER_insertEntry(void* key_ptr, vty k_type, void* val_ptr, vty v_type) {
  value_store key(key_ptr, k_type);
  value_store val(val_ptr, v_type);

  // TODO: cover other cases
  switch(v_type){
    case FLT:
      insert_output_entry(val.f32, key);
      break;
    case WHL:
      insert_output_entry(val.i32, key);
      break;
    default:
      insert_output_entry(val.i64, key);
  }

  switch(k_type){
    case FLT:
      return insert_entry(key.f32, val);
    case WHL:
      return insert_entry(key.i32, val);
    default:
      return insert_entry(key.i64, val);
  }
}


int __attribute__((always_inline)) MEMOIZER_existEntry(void* key_ptr, vty typ) {
  value_store val(key_ptr, typ);
  switch(typ){
    case FLT:
      return check_entry(val.f32);
    case WHL:
      return check_entry(val.i32);
    default:
      return check_entry(val.i64);
  }
}

void* __attribute__((always_inline)) MEMOIZER_getEntry(void* key_ptr, vty type) {
  value_store key(key_ptr, type);
  
  switch(type){
    case FLT:
      return get_entry(key.f32);
    case WHL:
      return get_entry(key.i32);
    default:
      return get_entry(key.i64);
  }
}

void MEMOIZER_printStats() {
  if (total_count != 0) {
    printf("############################ Memoization Stats ############################\n");
    printf("Total count: %d, hit count: %d, ratio: %f\n", total_count, hit_count, (float)hit_count/total_count);
    printf("total outs: %d, unique outs: %d, output reuse ratio: %f\n",total_out, unique_out, 1 - (double)unique_out/total_out);

  }
}

}
