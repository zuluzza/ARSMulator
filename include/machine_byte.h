#ifndef MACHINE_BYTE_H
#define MACHINE_BYTE_H

#include <bitset>

#define MAX_31_BITS_VALUE 0x7FFFFFFF

class Machine_byte {
public:
  Machine_byte(int64_t value) {
    if (value < 0) {
      value = -value;
      if (value > (static_cast<int64_t>(MAX_31_BITS_VALUE) + 1)) {
        value = static_cast<int64_t>(MAX_31_BITS_VALUE) + 1;
      }
      // negative number's are in 2's complement format
      bits = std::bitset<32>(~value + 1);
      bits[31] = 1;
    } else // value non-negative
    {
      if (value > MAX_31_BITS_VALUE) {
        value = MAX_31_BITS_VALUE;
      }
      bits = std::bitset<32>(value);
      bits[31] = 0;
    }
  }

  uint32_t to_unsigned32() const { return bits.to_ulong(); }

  int32_t to_signed32() const { return (int32_t)bits.to_ulong(); }

  void set_bits(std::bitset<32> new_bits) { bits = new_bits; }

  std::bitset<32> get_bits() const { return bits; }

  Machine_byte operator+(const Machine_byte &b) {
    int64_t val_a = this->to_signed32();
    int64_t val_b = b.to_signed32();
    return Machine_byte(val_a + val_b);
  }

  Machine_byte operator-(const Machine_byte &b) {
    int64_t val_a = this->to_signed32();
    int64_t val_b = b.to_signed32();
    return Machine_byte(val_a - val_b);
  }

private:
  std::bitset<32> bits;
};

#endif // MACHINE_BYTE_H