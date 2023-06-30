#include "BigInt.h"

constexpr int base = 10;

enum class CompareResult {
  LESS = -1, MORE = 1, EQUAL = 0
};

bool checkNumIsZero(const DataStructure::SimpleVector<char> &numbers) {
  return numbers.size() == 1 && numbers[0] == 0;
}

namespace NumbersLibrary {
NumbersLibrary::BigInt::BigInt() {
  numbers = DataStructure::SimpleVector<char>();
  numbers.push(0);
}

BigInt::BigInt(int n) {
  auto num = std::to_string(n);
  pushDigits(num, *this);
  this->numbers = RemoveLeadingZeros(numbers);
}

NumbersLibrary::BigInt::BigInt(const std::string &num) {
  if (num.empty()) throw std::invalid_argument("empty string");
  pushDigits(num, *this);
  this->numbers = RemoveLeadingZeros(numbers);
}

bool NumbersLibrary::BigInt::operator==(const NumbersLibrary::BigInt &num) const {
  if (!(num.numbers == numbers)) return false;
  if (IsNegative() != num.IsNegative()) return false;
  return true;
}

NumbersLibrary::BigInt::~BigInt() = default;

DataStructure::SimpleVector<char> &NumbersLibrary::BigInt::getNumbers() {
  return numbers;
}

NumbersLibrary::BigInt &NumbersLibrary::BigInt::operator+=(const BigInt &other) {
  auto otherCopy = other;
  DataStructure::SimpleVector<char> res;
  if (otherCopy.IsNegative() == this->IsNegative()) {
    res = sum(this->getNumbers(), otherCopy.getNumbers());
    this->numbers = RemoveLeadingZeros(res);
    if (checkNumIsZero(this->numbers)) SetIsNegative(false);
    return *this;
  }

  if (this->abs() < otherCopy.abs()) std::swap(*this, otherCopy);

  res = sub(this->getNumbers(), otherCopy.getNumbers());
  this->numbers = RemoveLeadingZeros(res);
  if (checkNumIsZero(this->numbers)) SetIsNegative(false);

  return *this;
}

NumbersLibrary::BigInt &NumbersLibrary::BigInt::operator-=(const BigInt &other) {
  auto otherCopy = other;
  otherCopy.SetIsNegative(!(other.IsNegative()));
  return *this += otherCopy;
}

NumbersLibrary::BigInt &NumbersLibrary::BigInt::operator*=(const BigInt &other) {
  DataStructure::SimpleVector<char> res;
  SetIsNegative(this->IsNegative() != other.IsNegative());

  res = mul(this->getNumbers(), other.getNumbers());
  this->numbers = RemoveLeadingZeros(res);
  if (checkNumIsZero(this->numbers)) SetIsNegative(false);
  return *this;
}

NumbersLibrary::BigInt &NumbersLibrary::BigInt::operator/=(const BigInt &other) {
  DataStructure::SimpleVector<char> res;

  SetIsNegative(this->IsNegative() != other.IsNegative());
  res = div(*this, other).first;
  this->numbers = RemoveLeadingZeros(res);
  return *this;
}

NumbersLibrary::BigInt &NumbersLibrary::BigInt::operator%=(const BigInt &other) {
  DataStructure::SimpleVector<char> res;
  res = div(*this, other).second;
  this->numbers = RemoveLeadingZeros(res);
  SetIsNegative(false);
  return *this;
}

DataStructure::SimpleVector<char> NumbersLibrary::BigInt::getNumbers() const {
  return numbers;
}

NumbersLibrary::BigInt NumbersLibrary::BigInt::operator+(const NumbersLibrary::BigInt &b) const {
  auto c = *this;
  return c += b;
}

NumbersLibrary::BigInt NumbersLibrary::BigInt::operator-(const NumbersLibrary::BigInt &b) const {
  auto c = *this;
  return c -= b;
}

NumbersLibrary::BigInt NumbersLibrary::BigInt::operator*(const BigInt &b) const {
  auto c = *this;
  return c *= b;
}

NumbersLibrary::BigInt NumbersLibrary::BigInt::operator/(const BigInt &b) const {
  auto c = *this;
  return c /= b;
}

NumbersLibrary::BigInt NumbersLibrary::BigInt::operator%(const BigInt &b) const {
  auto c = *this;
  return c %= b;
}

BigInt BigInt::operator&(const BigInt &b) const {
  auto c = *this;
  return c &= b;
}

BigInt BigInt::operator|(const BigInt &b) const {
  auto c = *this;
  return c |= b;
}

BigInt BigInt::operator^(const BigInt &b) const {
  auto c = *this;
  return c ^= b;
}

bool NumbersLibrary::BigInt::operator>=(const NumbersLibrary::BigInt &b) const {
  return compare(*this, b) != -1;
}

bool NumbersLibrary::BigInt::operator<=(const NumbersLibrary::BigInt &b) const {
  return compare(*this, b) != 1;
}

bool NumbersLibrary::BigInt::operator>(const NumbersLibrary::BigInt &b) const {
  return compare(*this, b) == 1;
}

bool NumbersLibrary::BigInt::operator<(const NumbersLibrary::BigInt &b) const {
  return compare(*this, b) == -1;
}

bool NumbersLibrary::BigInt::operator!=(const NumbersLibrary::BigInt &b) const {
  return !(b == *this);
}

DataStructure::SimpleVector<char> NumbersLibrary::BigInt::sum(const DataStructure::SimpleVector<char> &leftInput,
                                                              const DataStructure::SimpleVector<char> &rightInput) {
  auto res = DataStructure::SimpleVector<char>();
  int carry = 0;

  int lSize = static_cast<int>(leftInput.size());
  int rSize = static_cast<int>(rightInput.size());
  for (int i = 1; i <= std::max(lSize, rSize); i++) {
    int leftNum = (lSize - i >= 0) ? static_cast<int>(leftInput[lSize - i]) : 0;
    int rightNum = (rSize - i >= 0) ? static_cast<int>(rightInput[rSize - i]) : 0;
    char data = static_cast<char>(((leftNum + rightNum + carry) % 10));
    res.PushLeft(data);
    carry = (leftNum + rightNum + carry) / 10;
  }
  if (carry > 0) res.PushLeft(static_cast<char>(carry));
  return res;
}

DataStructure::SimpleVector<char> NumbersLibrary::BigInt::sub(const DataStructure::SimpleVector<char> &leftInput,
                                                              const DataStructure::SimpleVector<char> &rightInput) {
  auto res = DataStructure::SimpleVector<char>();
  int carry = 0;
  int lSize = static_cast<int>(leftInput.size());
  int rSize = static_cast<int>(rightInput.size());
  for (int i = 1; i <= std::max(lSize, rSize); i++) {
    int leftNum = (lSize - i >= 0) ? static_cast<int>(leftInput[lSize - i]) : 0;
    int rightNum = (rSize - i >= 0) ? static_cast<int>(rightInput[rSize - i]) : 0;
    int diff = leftNum - rightNum - carry;
    if (diff < 0) {
      diff += 10;
      carry = 1;
    } else {
      carry = 0;
    }

    res.PushLeft(static_cast<char>(diff));
  }

  return res;
}

DataStructure::SimpleVector<char> NumbersLibrary::BigInt::mul(const DataStructure::SimpleVector<char> &leftInput,
                                                              const DataStructure::SimpleVector<char> &rightInput) {
  auto res = DataStructure::SimpleVector<char>();
  int carry = 0;

  int lSize = static_cast<int>(leftInput.size());
  int rSize = static_cast<int>(rightInput.size());
  for (int j = lSize - 1; j >= 0; j--) {
    int leftNum = static_cast<unsigned char>(leftInput[j]);
    auto temp = DataStructure::SimpleVector<char>();
    for (int k = 0; k < lSize - j - 1; k++) {
      temp.PushLeft(0);
    }
    for (int i = rSize - 1; i >= 0; i--) {
      int rightNum = static_cast<unsigned char>(rightInput[i]);
      char data = static_cast<char>(((leftNum * rightNum + carry) % 10));
      temp.PushLeft(data);
      carry = (leftNum * rightNum + carry) / 10;
    }
    if (carry > 0) {
      temp.PushLeft(static_cast<char>(carry));
      carry = 0;
    }
    res = sum(res, temp);
  }
  return res;
}

std::pair<DataStructure::SimpleVector<char>, DataStructure::SimpleVector<char>>
NumbersLibrary::BigInt::div(const BigInt &leftInput, const BigInt &rightInput) {
  auto left = DataStructure::SimpleVector<char>();
  auto right = DataStructure::SimpleVector<char>();
  if (checkNumIsZero(rightInput.getNumbers())) throw std::invalid_argument("division by zero");

  auto leftInputAbs = leftInput.abs();
  auto rightInputAbs = rightInput.abs();

  if (leftInputAbs.abs() < rightInputAbs.abs()) {
    left.push(0);
    right = leftInputAbs.getNumbers();
    return {left, right};
  } else if (leftInputAbs == rightInputAbs) {
    left.push(1);
    right.push(0);
    return {left, right};
  }
  BigInt temp = BigInt("0");
  BigInt dec = BigInt("10");
  for (int i = 0; i < static_cast<int>(leftInputAbs.getNumbers().size()); i++) {
    temp *= dec;
    temp += BigInt(std::string(1, static_cast<char>((leftInputAbs.getNumbers()[i] + '0'))));

    for (int j = 9; j >= 0; j--) {
      BigInt substractFactor = rightInputAbs * BigInt(std::to_string(j));
      if (substractFactor <= temp) {
        left.push(static_cast<char>(j));
        temp -= substractFactor;
        break;
      }
    }
  }
  right = temp.getNumbers();

  return {left, right};
}

NumbersLibrary::BigInt NumbersLibrary::BigInt::abs() const {
  auto res = *this;
  res.SetIsNegative(false);
  return res;
}

std::ostream& operator<<(std::ostream &os, const NumbersLibrary::BigInt &printObject) {
  if (printObject.IsNegative()) os << "-";
  os << printObject.numbers;
  return os;
}

DataStructure::SimpleVector<char>
NumbersLibrary::BigInt::RemoveLeadingZeros(const DataStructure::SimpleVector<char> &vector) {

  if (vector.size() == 1) return {vector};
  auto sliceVector = DataStructure::SimpleVector<char>();
  size_t digitsArraySize = vector.size();

  size_t leadIndex = 0;
  while (leadIndex < digitsArraySize) {
    if (static_cast<int>(vector[static_cast<int>(leadIndex)]) != 0) break;
    leadIndex++;
  }
  if (leadIndex == digitsArraySize) sliceVector.push(0);
  else {
    for (size_t i = leadIndex; i < digitsArraySize; ++i) {
      sliceVector.push(vector[static_cast<int>(i)]);
    }
  }
  return sliceVector;
}

BigInt BigInt::operator++() {
  *this += BigInt("1");
  return *this;
}
NumbersLibrary::BigInt BigInt::operator++(int) &{
  NumbersLibrary::BigInt c = *this;
  *this += BigInt("1");
  return c;
}
BigInt BigInt::operator--(int) &{
  NumbersLibrary::BigInt c = *this;
  *this -= BigInt("1");
  return c;
}
BigInt BigInt::operator--() {
  *this -= BigInt("1");
  return *this;
}
BigInt BigInt::bin(const BigInt &num) {
  auto numCopy = num;
  auto emptyNum = BigInt();
  emptyNum.getNumbers().PopBack();
  auto zero = BigInt("0");
  auto two = BigInt("2");
  if (numCopy == zero) {
    emptyNum.getNumbers().push(0);
    return emptyNum;
  }
  while (numCopy > zero) {
    emptyNum.getNumbers().PushLeft((numCopy % two).getNumbers()[0]);
    numCopy /= two;
  }
  return emptyNum;
}

BigInt BigInt::dec(const BigInt &num) {
  auto numCopy = num;
  auto emptyNum = BigInt("0");
  auto zero = BigInt("0");
  auto ten = BigInt("10");
  auto two = BigInt("2");
  auto one = BigInt("0");
  auto base = BigInt("1");
  while (numCopy > zero) {
    auto lastDigit = numCopy % ten;
    numCopy /= ten;
    emptyNum += lastDigit * base;
    base *= two;
  }

  return emptyNum;
}

// TODO REFACTOR BINARY OPERATORS

BigInt BigInt::ApplyOperator(const BigInt &num2, BitFunc f) {
  auto binNum1 = BigInt::bin(*this);
  auto binNum2 = BigInt::bin(num2);
  auto binNum1Digits = binNum1.getNumbers();
  auto binNum2Digits = binNum2.getNumbers();
  auto emptyNum = DataStructure::SimpleVector<char>();

  int lSize = static_cast<int>(binNum1Digits.size());
  int rSize = static_cast<int>(binNum2Digits.size());
  for (int i = 1; i <= std::max(lSize, rSize); i++) {
    int leftNum = (lSize - i >= 0) ? static_cast<int>(binNum1Digits[lSize - i]) : 0;
    int rightNum = (rSize - i >= 0) ? static_cast<int>(binNum2Digits[rSize - i]) : 0;
    char data;
    switch (f) {
      case MULTIPLY:data = static_cast<char>((leftNum * rightNum));
        break;
      case PLUS:data = static_cast<char>((leftNum + rightNum));
        break;
      case NOT_EQUAL:data = static_cast<char>((leftNum != rightNum));
        break;
    }
    emptyNum.PushLeft(data);
  }

  this->numbers = emptyNum;
  *this = dec(*this);
  return *this;
}

BigInt BigInt::operator&=(const BigInt &num2) {
  return ApplyOperator(num2, MULTIPLY);
}
BigInt BigInt::operator|=(const BigInt &num2) {
  return ApplyOperator(num2, PLUS);
}
BigInt BigInt::operator^=(const BigInt &num2) {
  return ApplyOperator(num2, NOT_EQUAL);
}

BigInt BigInt::operator~() const {
  auto binNum = BigInt::bin(*this);
  auto nums = binNum.getNumbers();
  for (int i = 0; i < static_cast<int>(nums.size()); i++) {
    nums[i] = static_cast<char>(!(bool(nums[i])));
  }
  binNum.getNumbers() = nums;
  return BigInt::dec(binNum);
}

void pushDigits(const std::string &num, BigInt &object) {
  std::string number = HandleNegative(num, object);
  for (char c : number) {
    if (!isdigit(c)) throw std::invalid_argument("invalid char in string");
    object.getNumbers().push(static_cast<char>(c - '0'));
  }
  object.getNumbers() = BigInt::RemoveLeadingZeros(object.getNumbers());
}

std::istream& operator>>(std::istream &o, BigInt &inputObject) {
  int currChar = o.peek();
  while (std::isdigit(currChar)) {
    inputObject.getNumbers().push(static_cast<char>(currChar - '0'));
    o.get();
    currChar = o.peek();
  }

  inputObject.getNumbers() = BigInt::RemoveLeadingZeros(inputObject.getNumbers());
  return o;
}

std::string HandleNegative(const std::string &num, BigInt &object) {
  num[0] == '-' ? object.SetIsNegative(true) : object.SetIsNegative(false);
  if (object.IsNegative()) return num.substr(1);
  return num;
}
BigInt::operator std::string() const {
  auto res = std::string();
  for (int i = 0; i < static_cast<int>(numbers.size()); ++i) {
    res += std::to_string(static_cast<int>(numbers[i]));
  }
  return res;
}
bool BigInt::IsNegative() const {
  return isNegative;
}
void BigInt::SetIsNegative(bool is_negative) {
  isNegative = is_negative;
}
size_t BigInt::getSize() const {
  return numbers.size();
}
int compare(const BigInt &a, const BigInt &b) {

  if (a.IsNegative() && !b.IsNegative()) return static_cast<int>(CompareResult::LESS);

  if (!a.IsNegative() && b.IsNegative()) return static_cast<int>(CompareResult::MORE);

  // a and b has equal sign
  if (a.getSize() < b.getSize()) return static_cast<int>(a.IsNegative() ? CompareResult::MORE : CompareResult::LESS);

  if (a.getSize() > b.getSize()) return static_cast<int>(a.IsNegative() ? CompareResult::LESS : CompareResult::MORE);

  for (int i = 0; i < static_cast<int>(a.getNumbers().size()); i++) {
    if (a.numbers[i] < b.numbers[i])
      return static_cast<int>(a.IsNegative() ? CompareResult::MORE : CompareResult::LESS);

    if (a.numbers[i] > b.numbers[i])
      return static_cast<int>(a.IsNegative() ? CompareResult::LESS : CompareResult::MORE);
  }
  return static_cast<int>(CompareResult::EQUAL);
}

}