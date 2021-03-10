#ifndef BIG_INTEGER_H_
#define BIG_INTEGER_H_

#include <string>
#include <vector>

namespace big_num_arithmetic {

class BigInteger {
 public:
  BigInteger() = default;
  explicit BigInteger(int64_t);
  static BigInteger FromString(const std::string&, int);

  [[nodiscard]] std::string ToString(int base,
                                     bool should_show_base = false) const;
  [[nodiscard]] inline int Sign() const {
    return this->NumberOfDigits() == 0 ? 0 : this->is_negative ? -1 : 1;
  }
  inline void Negate() { this->is_negative = !this->is_negative; }
  inline void Abs() { this->is_negative = false; }

  bool operator==(const BigInteger&) const;
  bool operator!=(const BigInteger&) const;
  bool operator<(const BigInteger&) const;
  bool operator>(const BigInteger&) const;
  bool operator<=(const BigInteger&) const;
  bool operator>=(const BigInteger&) const;

  bool operator==(int64_t) const;
  bool operator!=(int64_t) const;
  bool operator<(int64_t) const;
  bool operator>(int64_t) const;
  bool operator<=(int64_t) const;
  bool operator>=(int64_t) const;

  friend bool operator==(int64_t, const BigInteger&);
  friend bool operator!=(int64_t, const BigInteger&);
  friend bool operator<(int64_t, const BigInteger&);
  friend bool operator>(int64_t, const BigInteger&);
  friend bool operator<=(int64_t, const BigInteger&);
  friend bool operator>=(int64_t, const BigInteger&);

  BigInteger& operator+=(const BigInteger&);
  BigInteger& operator-=(const BigInteger&);
  BigInteger& operator*=(const BigInteger&);
  BigInteger& operator/=(const BigInteger&);

  BigInteger& operator+=(int64_t);
  BigInteger& operator-=(int64_t);
  BigInteger& operator*=(int64_t);
  BigInteger& operator/=(int64_t);

  BigInteger operator+(const BigInteger&) const;
  BigInteger operator-(const BigInteger&) const;
  BigInteger operator*(const BigInteger&) const;
  BigInteger operator/(const BigInteger&) const;

  BigInteger operator+(int64_t) const;
  BigInteger operator-(int64_t) const;
  BigInteger operator*(int64_t) const;
  BigInteger operator/(int64_t) const;

  friend BigInteger operator+(int64_t, const BigInteger&);
  friend BigInteger operator-(int64_t, const BigInteger&);
  friend BigInteger operator*(int64_t, const BigInteger&);
  friend BigInteger operator/(int64_t, const BigInteger&);

  uint32_t operator%(uint32_t) const;

  const BigInteger operator++(int);
  BigInteger& operator++();
  const BigInteger operator--(int);
  BigInteger& operator--();

  BigInteger operator-() const;
  BigInteger operator+() const { return *this; }

  explicit operator int64_t() const;

  static int64_t internal_base;

 private:
  std::vector<int64_t> digits_;
  bool is_negative{false};

  [[nodiscard]] int64_t DigitAt(size_t pos) const;
  [[nodiscard]] int64_t& DigitAt(size_t pos);
  [[nodiscard]] int64_t SignedDigitAt(size_t pos) const;
  [[nodiscard]] ssize_t NumberOfDigits() const;
  [[nodiscard]] int64_t LeadingDigit() const;

  void RemoveZeroes();
  inline void SetSign(int64_t value);
  void InsertLeastSignificantDigit(int64_t digit, int64_t quantity);
  void InsertLeastSignificantDigit(int64_t digit);
  inline void PushLeadingDigit(int64_t digit);
  inline void PushDigitToEnd(int64_t digit);
  void ReverseDigitsAndNormalize();

  static inline int64_t RemainderByBase(int64_t number);
  friend BigInteger MultiplyByShort(const BigInteger&, int64_t);
  static int CompareAbsoluteValues(const BigInteger& lhs,
                                   const BigInteger& rhs);
};

struct DivisionByZeroError {};

}  // namespace big_num_arithmetic

std::ostream& operator<<(std::ostream&, const big_num_arithmetic::BigInteger&);
std::istream& operator>>(std::istream&, big_num_arithmetic::BigInteger&);

#endif  // BIG_INTEGER_H_
