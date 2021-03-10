#include "big_integer.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <stdexcept>

namespace {

bool IsShowbaseSet(std::ostream& ostream) {
  return (ostream.flags() & std::ostream::showbase) == std::ostream::showbase;
}
inline char DigitToChar(int64_t digit) {
  return static_cast<char>(digit < 10 ? digit + '0' : digit - 10 + 'a');
}
template<typename S>
int GetStreamBase(const S& stream) {
  auto base_field{stream.flags() & std::iostream::basefield};
  switch (base_field) {
    case std::ios_base::hex: {
      return 16;
    }
    case std::ios_base::oct: {
      return 8;
    }
    default: {
      return 10;
    }
  }
}
void FormatHexInput(std::string& target) {
  // Takes input string which may be in hex format and removes the '0x' part
  if (target[0] == '-' && target[2] == 'x') {
    target = "-" + target.substr(3);
  } else if (target[1] == 'x') {
    target = target.substr(2);
  }
}

}  // namespace

namespace big_num_arithmetic {

namespace {

void ThrowIfBaseIsInvalid(int base) {
  if (base < 2 || base > 36) {
    throw std::logic_error("Invalid base");
  }
}
int CharToDigit(char ch) {
  if (ch >= '0' && ch <= '9') {
    return ch - '0';
  }
  if (ch >= 'a' && ch <= 'z') {
    return ch - 'a' + 10;
  }
  return -1;
}
std::string GetReversedBasePrefix(int base) {
  switch (base) {
    case 8: {
      return "0";
    }
    case 16: {
      return "x0";
    }
    default: {
      return "";
    }
  }
}
int64_t GuessDigit(const BigInteger& lhs, const BigInteger& rhs) {
  int64_t quotient_l_bound{0};
  int64_t quotient_u_bound{BigInteger::internal_base};
  while (quotient_l_bound + 1 < quotient_u_bound) {
    int64_t middle_candidate{(quotient_l_bound + quotient_u_bound) / 2};
    if (MultiplyByShort(rhs, middle_candidate) > lhs) {
      quotient_u_bound = middle_candidate;
    } else {
      quotient_l_bound = middle_candidate;
    }
  }
  return quotient_l_bound;
}

}  // namespace

int64_t BigInteger::internal_base = 1'000'000'000;

BigInteger::BigInteger(int64_t value) {
  this->SetSign(value);
  while (value != 0) {
    this->PushLeadingDigit(std::abs(value % internal_base));
    value /= internal_base;
  }
}
BigInteger BigInteger::FromString(const std::string& input, int base) {
  ThrowIfBaseIsInvalid(base);

  for (size_t i{0}; i < input.size(); i++) {
    if (i == 0 && input[0] == '-') {
      continue;
    }
    int digit{CharToDigit(input[i])};
    if (digit >= base || digit == -1) {
      throw std::runtime_error("Invalid symbol at index " + std::to_string(i));
    }
  }
  BigInteger result{0};
  BigInteger base_power{1};
  for (auto iter{input.rbegin()}; iter < input.rend(); iter++) {
    if (*iter == '-') {
      continue;
    }
    result += MultiplyByShort(base_power, CharToDigit(*iter));
    base_power = MultiplyByShort(base_power, base);
  }
  return input[0] == '-' ? -result : result;
}

std::string BigInteger::ToString(int base, bool should_show_base) const {
  ThrowIfBaseIsInvalid(base);
  std::string reversed_result{*this == 0 ? "0" : ""};
  BigInteger temp{*this};
  temp.Abs();
  while (temp != 0) {
    int64_t digit{int64_t(temp - MultiplyByShort(temp / base, base))};
    reversed_result += DigitToChar(digit);
    temp /= base;
  }
  if (should_show_base) {
    reversed_result += GetReversedBasePrefix(base);
  }
  if (*this < 0) {
    reversed_result += '-';
  }
  std::reverse(reversed_result.begin(), reversed_result.end());
  return reversed_result;
}

bool BigInteger::operator==(const BigInteger& rhs) const {
  if (this->Sign() != rhs.Sign()) {
    return false;
  }
  return CompareAbsoluteValues(*this, rhs) == 0;
}
bool BigInteger::operator!=(const BigInteger& rhs) const {
  return !(*this == rhs);
}
bool BigInteger::operator<(const BigInteger& rhs) const {
  if (this->Sign() != rhs.Sign()) {
    return this->Sign() < rhs.Sign();
  }
  return CompareAbsoluteValues(*this, rhs) * this->Sign() == -1;
}
bool BigInteger::operator>(const BigInteger& rhs) const {
  return !(*this == rhs || *this < rhs);
}
bool BigInteger::operator<=(const BigInteger& rhs) const {
  return *this == rhs || *this < rhs;
}
bool BigInteger::operator>=(const BigInteger& rhs) const {
  return !(*this < rhs);
}

bool BigInteger::operator==(int64_t rhs) const {
  return *this == BigInteger(rhs);
}
bool BigInteger::operator!=(int64_t rhs) const {
  return *this != BigInteger(rhs);
}
bool BigInteger::operator<(int64_t rhs) const {
  return *this < BigInteger(rhs);
}
bool BigInteger::operator>(int64_t rhs) const {
  return *this > BigInteger(rhs);
}
bool BigInteger::operator<=(int64_t rhs) const {
  return *this <= BigInteger(rhs);
}
bool BigInteger::operator>=(int64_t rhs) const {
  return *this >= BigInteger(rhs);
}

bool operator==(int64_t lhs, const BigInteger& rhs) {
  return rhs == lhs;
}
bool operator!=(int64_t lhs, const BigInteger& rhs) {
  return rhs != lhs;
}
bool operator<(int64_t lhs, const BigInteger& rhs) {
  return rhs > lhs;
}
bool operator>(int64_t lhs, const BigInteger& rhs) {
  return rhs < lhs;
}
bool operator<=(int64_t lhs, const BigInteger& rhs) {
  return rhs >= lhs;
}
bool operator>=(int64_t lhs, const BigInteger& rhs) {
  return rhs <= lhs;
}

BigInteger& BigInteger::operator+=(int64_t value) {
  return *this += BigInteger(value);
}
BigInteger& BigInteger::operator-=(int64_t value) {
  return *this -= BigInteger(value);
}
BigInteger& BigInteger::operator*=(int64_t value) {
  return *this *= BigInteger(value);
}
BigInteger& BigInteger::operator/=(int64_t value) {
  return *this /= BigInteger(value);
}

BigInteger& BigInteger::operator+=(const BigInteger& rhs) {
  return *this = *this + rhs;
}
BigInteger& BigInteger::operator-=(const BigInteger& rhs) {
  return *this = *this - rhs;
}
BigInteger& BigInteger::operator*=(const BigInteger& rhs) {
  return *this = *this * rhs;
}
BigInteger& BigInteger::operator/=(const BigInteger& rhs) {
  return *this = *this / rhs;
}

BigInteger BigInteger::operator+(int64_t rhs) const {
  return *this + BigInteger(rhs);
}
BigInteger BigInteger::operator-(int64_t rhs) const {
  return *this - BigInteger(rhs);
}
BigInteger BigInteger::operator*(int64_t rhs) const {
  return *this * BigInteger(rhs);
}
BigInteger BigInteger::operator/(int64_t rhs) const {
  return *this / BigInteger(rhs);
}

BigInteger operator+(int64_t lhs, const BigInteger& rhs) {
  return BigInteger(lhs) + rhs;
}
BigInteger operator-(int64_t lhs, const BigInteger& rhs) {
  return BigInteger(lhs) - rhs;
}
BigInteger operator*(int64_t lhs, const BigInteger& rhs) {
  return BigInteger(lhs) * rhs;
}
BigInteger operator/(int64_t lhs, const BigInteger& rhs) {
  return BigInteger(lhs) / rhs;
}

BigInteger BigInteger::operator+(const BigInteger& rhs) const {
  if (CompareAbsoluteValues(*this, rhs) == -1) {
    return rhs + *this;
  }
  if (*this < 0) {
    return -(-*this + -rhs);
  }
  BigInteger result;
  int64_t carry{0};
  for (size_t i{0}; i < this->NumberOfDigits() || carry != 0; i++) {
    int64_t digit{carry};
    if (i < this->NumberOfDigits()) {
      digit += this->DigitAt(i);
    }
    if (i < rhs.NumberOfDigits()) {
      digit += rhs.SignedDigitAt(i);
    }
    result.PushLeadingDigit(RemainderByBase(digit));
    carry = (digit - result.LeadingDigit()) / internal_base;
  }
  result.RemoveZeroes();
  return result;
}
BigInteger BigInteger::operator-(const BigInteger& rhs) const {
  return *this < 0 ? -(-*this + rhs) : *this + -rhs;
}
BigInteger BigInteger::operator*(const BigInteger& rhs) const {
  if (*this < 0) {
    return -(-*this * rhs);
  }
  if (rhs < 0) {
    return -(*this * -rhs);
  }
  BigInteger result{0};
  for (ssize_t i{0}; i < rhs.NumberOfDigits(); i++) {
    BigInteger lhs_times_digit{MultiplyByShort(*this, rhs.DigitAt(i))};
    lhs_times_digit.InsertLeastSignificantDigit(0, i);
    result += lhs_times_digit;
  }
  result.RemoveZeroes();
  return result;
}
BigInteger BigInteger::operator/(const BigInteger& rhs) const {
  if (rhs == 0) {
    throw DivisionByZeroError();
  }
  if (*this < 0) {
    return -(-*this / rhs);
  }
  if (rhs < 0) {
    return -(*this / -rhs);
  }
  // Inspired by the traditional soviet 'Delenie ugolkom' algorithm :)

  // The result is stored in reverse to provide O(1) insertion of
  // least significant digit. The result is then reversed before returning.
  BigInteger reversed_result{0};
  BigInteger trailing_dividend{0};
  for (ssize_t i{this->NumberOfDigits() - 1}; i >= 0; i--) {
    trailing_dividend.InsertLeastSignificantDigit(this->DigitAt(i));
    int64_t digit{GuessDigit(trailing_dividend, rhs)};
    reversed_result.PushDigitToEnd(digit);
    trailing_dividend -= MultiplyByShort(rhs, digit);
  }
  reversed_result.ReverseDigitsAndNormalize();
  return reversed_result;
}

uint32_t BigInteger::operator%(uint32_t rhs) const {
  return (int64_t(*this - (*this / rhs) * rhs) + rhs) % rhs;
}

const BigInteger BigInteger::operator++(int) {
  BigInteger temp{*this};
  *this += 1;
  return temp;
}
BigInteger& BigInteger::operator++() {
  return *this += 1;
}
const BigInteger BigInteger::operator--(int) {
  BigInteger temp{*this};
  *this -= 1;
  return temp;
}
BigInteger& BigInteger::operator--() {
  return *this -= 1;
}

BigInteger BigInteger::operator-() const {
  BigInteger temp{*this};
  temp.Negate();
  return temp;
}

BigInteger::operator int64_t() const {
  if (*this > INT64_MAX || *this < INT64_MIN) {
    throw std::runtime_error("int64_t overflow");
  }
  int64_t result{0};
  int64_t power{1};
  for (auto digit : this->digits_) {
    result += digit * power;
    if (BigInteger(power) * internal_base <= INT64_MAX) {
      power *= internal_base;
    }
  }
  return result * this->Sign();
}

int64_t BigInteger::DigitAt(size_t pos) const { return this->digits_.at(pos); }
int64_t& BigInteger::DigitAt(size_t pos) { return this->digits_.at(pos); }
int64_t BigInteger::SignedDigitAt(size_t pos) const {
  return this->Sign() * this->DigitAt(pos);
}
ssize_t BigInteger::NumberOfDigits() const { return this->digits_.size(); }
int64_t BigInteger::LeadingDigit() const {
  return this->digits_.back();
}

void BigInteger::RemoveZeroes() {
  ssize_t rightmost_nonzero;
  for (rightmost_nonzero = this->NumberOfDigits() - 1; rightmost_nonzero >= 0;
       rightmost_nonzero--) {
    if (this->DigitAt(rightmost_nonzero) != 0) {
      break;
    }
  }
  this->digits_.resize(rightmost_nonzero + 1);
}
void BigInteger::SetSign(int64_t value) {
  this->is_negative = value < 0;
}
void BigInteger::InsertLeastSignificantDigit(int64_t digit) {
  this->digits_.insert(this->digits_.begin(), digit);
}
void BigInteger::InsertLeastSignificantDigit(int64_t digit, int64_t quantity) {
  this->digits_.insert(this->digits_.begin(), quantity, digit);
}
void BigInteger::PushLeadingDigit(int64_t digit) {
  this->digits_.push_back(digit);
}
void BigInteger::PushDigitToEnd(int64_t digit) {
  this->digits_.push_back(digit);
}
void BigInteger::ReverseDigitsAndNormalize() {
  std::reverse(this->digits_.begin(), this->digits_.end());
  this->RemoveZeroes();
}

int64_t BigInteger::RemainderByBase(int64_t number) {
  return ((number % internal_base) + internal_base) % internal_base;
}
BigInteger MultiplyByShort(const BigInteger& lhs, int64_t rhs) {
  BigInteger result;
  int64_t carry{0};
  for (ssize_t i{0}; i < lhs.NumberOfDigits() || carry != 0; i++) {
    int64_t digit{carry};
    if (i < lhs.NumberOfDigits()) {
      digit += lhs.DigitAt(i) * rhs;
    }
    result.PushLeadingDigit(digit % BigInteger::internal_base);
    carry = digit / BigInteger::internal_base;
  }
  result.RemoveZeroes();
  result.is_negative = lhs < 0;
  return result;
}
int BigInteger::CompareAbsoluteValues(const BigInteger& lhs,
                                      const BigInteger& rhs) {
  if (lhs.NumberOfDigits() != rhs.NumberOfDigits()) {
    return lhs.NumberOfDigits() < rhs.NumberOfDigits() ? -1 : 1;
  }
  for (ssize_t i{lhs.NumberOfDigits() - 1}; i >= 0; i--) {
    if (lhs.DigitAt(i) != rhs.DigitAt(i)) {
      return lhs.DigitAt(i) < rhs.DigitAt(i) ? -1 : 1;
    }
  }
  return 0;
}

}  // namespace big_num_arithmetic

std::istream &
operator>>(std::istream &is, big_num_arithmetic::BigInteger &big_int) {
  std::string number;
  is >> number;
  int sign{1};
  if (number.at(0) == '-') {
    sign = -1;
    number.erase(0, 1);
  }
  if (is.flags() & std::ios::hex) {
    std::string number16 = number.substr(2, number.size() - 2);
    big_int = big_num_arithmetic::BigInteger::FromString(number16, 16);
  } else if (is.flags() & std::ios::oct) {
    std::string number8 = number.substr(1, number.size() - 1);
    big_int = big_num_arithmetic::BigInteger::FromString(number8, 8);
  } else {
    big_int = big_num_arithmetic::BigInteger::FromString(number, 10);
  }
  big_int *= sign;
  return is;
}

std::ostream &
operator<<(std::ostream &os, const big_num_arithmetic::BigInteger &big_int) {
  std::string str = big_int.ToString(10);
  if (os.flags() & std::ios::showbase) {
    if (os.flags() & std::ios::hex) {
      if (big_int < 0) {
        os << '-' << std::hex << std::showbase << str;
      } else {
        os << std::hex << std::showbase << str;
      }
    } else if (os.flags() & std::ios::oct) {
      if (big_int < 0) {
        os << '-' << std::oct << std::showbase << str;
      } else {
        os << std::oct << std::showbase << str;
      }
    } else {
      if (big_int < 0) {
        os << '-' << str;
      } else {
        os << str;
      }
    }
  } else {
    os << str;
  }
  return os;
}
