#ifndef HELPERS_H
#define HELPERS_H

#include <unordered_map>
#include <algorithm>
#include <array>
#include <bitset>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <iomanip>

enum vty { FLT, DBL, BOL, CHR, HLF, WHL, LNG, PTR, INVALID };

// Copied from value logger repository
// This union stores the value of a given llvm Value
//
union value_store {
    uint32_t ptr;
    uint8_t i8;
    uint16_t i16;
    uint32_t i32;
    float f32;
    double d64;
    uint64_t i64;
    uint8_t m_bytes[sizeof(double)];

    public:
    value_store() { this->i64 = 0; }
    value_store(void *ptr, vty typ) {
        this->i64 = 0;
        switch (typ) {
            case FLT:
                this->f32 = *((float *)ptr);
                break;
            case DBL:
                this->d64 = *((double *)ptr);
                break;
            case BOL:
                this->i8 = *((uint8_t *)ptr);
                break;
            case CHR:
                this->i8 = *((uint8_t *)ptr);
                break;
            case HLF:
                this->i16 = *((uint16_t *)ptr);
                break;
            case WHL:
                this->i32 = *((uint32_t *)ptr);
                break;
            case LNG:
                this->i64 = *((uint64_t *)ptr);
                break;
            case PTR:
                this->ptr = *((uint64_t *)ptr);
                break;
            default:
                this->i32 = 0;
        }   
    }

    uint64_t vtytosize(vty typ) {
        switch (typ) {
            case FLT:
                return 4;
                break;
            case DBL:
                return 8;
                break;
            case BOL:
                return 1;
                break;
            case CHR:
                return 1;
                break;
            case HLF:
                return 2;
                break;
            case WHL:
                return 4;
                break;
            case LNG:
                return 8;
                break;
            case PTR:
                return sizeof(void *);
                break;
            default:
                return sizeof(void *);
        }
    }

    void dump(std::ostream &out, vty typ) {
        // int size = vtytosize(typ);
        // std::reverse_copy(m_bytes, m_bytes + size, std::ostream_iterator<int>(out << std::hex, ""));
    }
};

#endif