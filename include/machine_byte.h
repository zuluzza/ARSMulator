#ifndef MACHINE_BYTE_H
#define MACHINE_BYTE_H

#include <bitset>

#define MAX_31_BITS_VALUE 0x7FFFFFFF
#define BIT_COUNT 32

class Machine_byte {
public:
  Machine_byte(int64_t value, bool use_signed = false) {
    if (value < 0) {
      value = -value;
      if (value > (static_cast<int64_t>(MAX_31_BITS_VALUE) + 1)) {
        value = static_cast<int64_t>(MAX_31_BITS_VALUE) + 1;
      }
      // negative number's are in 2's complement format
      bits = std::bitset<BIT_COUNT>(~value + 1);
      bits[31] = 1;
    } else // value non-negative
    {
      if (use_signed && value > MAX_31_BITS_VALUE) {
        value = MAX_31_BITS_VALUE;
      }
      bits = std::bitset<BIT_COUNT>(value);
      if (use_signed) {
        bits[31] = 0;
      }
    }
  }

  Machine_byte(std::bitset<BIT_COUNT> new_bits) { bits = new_bits; }

  uint32_t to_unsigned32() const { return bits.to_ulong(); }

  int32_t to_signed32() const { return (int32_t)bits.to_ulong(); }

  void set_bits(std::bitset<BIT_COUNT> new_bits) { bits = new_bits; }

  std::bitset<BIT_COUNT> get_bits() const { return bits; }

  Machine_byte operator+(const Machine_byte &second) {
    std::bitset<BIT_COUNT> result;
    carry = false;
    for (uint8_t i = 0; i < BIT_COUNT; ++i) {
      const bool a = this->get_bits()[i];
      const bool b = second.get_bits()[i];
      result[i] = a ^ b ^ carry;
      carry = (a && b) || (a && carry) || (b && carry);
    }
    return Machine_byte(result);
  }

  Machine_byte operator-(const Machine_byte &second) {
    std::bitset<BIT_COUNT> result;
    borrow = false;
    for (uint8_t i = 0; i < BIT_COUNT; ++i) {
      const bool a = this->get_bits()[i];
      const bool b = second.get_bits()[i];
      result[i] = borrow ? !(a ^ b) : (a ^ b);
      borrow = borrow ? (!a || (a && b)) : (!a && b);
    }
    return Machine_byte(result);
  }

  Machine_byte operator&(const Machine_byte &second) const {
    std::bitset<BIT_COUNT> tmp = get_bits();
    tmp &= second.get_bits();
    return Machine_byte(tmp);
  }

  Machine_byte operator|(const Machine_byte &second) const {
    std::bitset<BIT_COUNT> tmp = get_bits();
    tmp |= second.get_bits();
    return Machine_byte(tmp);
  }

  Machine_byte operator^(const Machine_byte &second) const {
    std::bitset<BIT_COUNT> tmp = get_bits();
    tmp ^= second.get_bits();
    return Machine_byte(tmp);
  }

  bool get_carry() const { return carry; }
  void set_carry(const bool use_carry) { carry = use_carry; }
  bool get_borrow() const { return borrow; }

private:
  std::bitset<BIT_COUNT> bits;
  bool carry;
  bool borrow;
};

#endif // MACHINE_BYTE_H