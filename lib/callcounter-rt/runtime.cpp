#include "utilities.h"


template<typename T1>
std::unordered_map<T1, value_store> m;

template<typename T1>
int check_entry(T1 e) {
  return m<T1>.count(e) > 0;
}

template<typename T1>
void insert_entry(T1 key, value_store val) {
  m<T1>[key] = val;
}

template<typename T1>
void *get_entry(T1 key) {
  return &(m<T1>[key]);
}

extern "C" {

void MEMOIZER_insertEntry(void* key_ptr, vty k_type, void* val_ptr, vty v_type) {
  value_store key(key_ptr, k_type);
  value_store val(val_ptr, v_type);
  switch(k_type){
    case FLT:
      return insert_entry(key.f32, val);
    case WHL:
      return insert_entry(key.i32, val);
    default:
      return insert_entry(key.i64, val);
  }
}


int MEMOIZER_existEntry(void* key_ptr, vty typ) {
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

void* MEMOIZER_getEntry(void* key_ptr, vty type) {
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

}
