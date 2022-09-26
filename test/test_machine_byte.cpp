#define CATCH_CONFIG_MAIN

#include <catch2/catch_all.hpp>

#include "machine_byte.h"
#include <limits>

TEST_CASE("machine_byte, construct signed") {
  Machine_byte a(-1);
  CHECK(std::bitset<32>(0xFFFFFFFF) == a.get_bits());

  Machine_byte b(-123);
  CHECK(std::bitset<32>(0xFFFFFF85) == b.get_bits());

  Machine_byte c(std::numeric_limits<int32_t>::min());
  CHECK(std::bitset<32>(0x80000000) == c.get_bits());

  Machine_byte d(0);
  CHECK(std::bitset<32>(0x00000000) == d.get_bits());

  Machine_byte e(1);
  CHECK(std::bitset<32>(0x00000001) == e.get_bits());

  Machine_byte f(std::numeric_limits<int32_t>::max());
  CHECK(std::bitset<32>(0x7FFFFFFF) == f.get_bits());

  Machine_byte g(static_cast<int64_t>(std::numeric_limits<int32_t>::min()) -
                 10);
  CHECK(std::bitset<32>(0x80000000) == g.get_bits());

  Machine_byte h(static_cast<int64_t>(std::numeric_limits<int32_t>::max()) +
                 10);
  CHECK(std::bitset<32>(0x7FFFFFFF) == h.get_bits());
}

TEST_CASE("machine_byte, set and get bits") {
  Machine_byte b(0);

  b.set_bits(std::bitset<32>(1234));
  CHECK(std::bitset<32>(1234) == b.get_bits());
}

TEST_CASE("machine_byte, to unsigned") {
  Machine_byte b(0);
  CHECK(0 == b.to_unsigned32());

  b.set_bits(std::bitset<32>(1234));
  CHECK(1234 == b.to_unsigned32());

  b.set_bits(std::bitset<32>(std::numeric_limits<uint32_t>::max()));
  CHECK(std::numeric_limits<uint32_t>::max() == b.to_unsigned32());
}

TEST_CASE("machine_byte, to signed") {
  Machine_byte b(0);
  CHECK(0 == b.to_signed32());

  b.set_bits(std::bitset<32>(1234));
  CHECK(1234 == b.to_signed32());

  b.set_bits(std::bitset<32>(std::numeric_limits<int32_t>::max()));
  CHECK(std::numeric_limits<int32_t>::max() == b.to_signed32());

  b.set_bits(std::bitset<32>(-1234));
  CHECK(-1234 == b.to_signed32());

  b.set_bits(std::bitset<32>(std::numeric_limits<int32_t>::min()));
  CHECK(std::numeric_limits<int32_t>::min() == b.to_signed32());
}

TEST_CASE("machine_byte, addition positive") {
  Machine_byte a(2);
  Machine_byte b(3);
  a = a + b;
  CHECK(5 == a.to_unsigned32());
}

TEST_CASE("machine_byte, addition negative") {
  Machine_byte a(-2);
  Machine_byte b(-3);
  a = a + b;
  CHECK(-5 == a.to_signed32());
}

TEST_CASE("machine_byte, subtraction positive") {
  Machine_byte a(3);
  Machine_byte b(2);
  a = a - b;
  CHECK(1 == a.to_unsigned32());
}

TEST_CASE("machine_byte, subtraction negative") {
  Machine_byte a(-3);
  Machine_byte b(-2);
  a = a - b;
  CHECK(-1 == a.to_signed32());
}
