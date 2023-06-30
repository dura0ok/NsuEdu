#pragma once

#include "SimpleVector.h"
#include <algorithm>

namespace NumbersLibrary {
class BigInt {
 public:

  BigInt();
  explicit BigInt(int);
  explicit BigInt(const std::string &);

  size_t getSize() const;
  [[nodiscard]] bool IsNegative() const;
  void SetIsNegative(bool is_negative);
  bool operator==(const BigInt &num) const;

  BigInt operator+(const BigInt &b) const;

  BigInt operator-(const BigInt &b) const;

  BigInt operator*(const BigInt &b) const;

  BigInt operator/(const BigInt &b) const;

  BigInt operator%(const BigInt &b) const;

  BigInt operator&(const BigInt &b) const;
  BigInt operator|(const BigInt &b) const;
  BigInt operator^(const BigInt &b) const;
  BigInt operator~() const;

  //  BigInt operator^(const BigInt&, const BigInt&);
//  BigInt operator%(const BigInt&, const BigInt&);
  BigInt operator&=(const BigInt &num2);
  BigInt operator|=(const BigInt &num2);
  BigInt operator^=(const BigInt &num2);

  explicit operator std::string() const;



//  BigInt operator|(const BigInt&, const BigInt&);


  //BigInt operator-(const BigInt &otherCopy) const;
  BigInt &operator+=(const BigInt &otherCopy);

  BigInt operator++(int) &;
  BigInt operator++();

  BigInt operator--(int) &;
  BigInt operator--();

  BigInt &operator-=(const BigInt &b);

  BigInt &operator*=(const BigInt &b);

  BigInt &operator/=(const BigInt &b);

  BigInt &operator%=(const BigInt &b);

  bool operator>=(const BigInt &b) const;

  bool operator<=(const BigInt &b) const;

  bool operator>(const BigInt &b) const;

  bool operator<(const BigInt &b) const;

  bool operator!=(const BigInt &b) const;

  ~BigInt();

  friend std::ostream& operator<<(std::ostream &os, const NumbersLibrary::BigInt &printObject);
  friend std::istream& operator>>(std::istream &o, BigInt &inputObject);
  friend void pushDigits(const std::string &num, BigInt &object);
  friend std::string HandleNegative(const std::string &num, BigInt &object);
  friend int compare(const NumbersLibrary::BigInt &a, const NumbersLibrary::BigInt &b);

  static BigInt bin(const BigInt &num);
  static BigInt dec(const BigInt &num);

 private:
  bool isNegative{};

  DataStructure::SimpleVector<char> numbers;

  // ================= Methods =================
  DataStructure::SimpleVector<char> &getNumbers();
  [[nodiscard]] DataStructure::SimpleVector<char> getNumbers() const;

  static DataStructure::SimpleVector<char> RemoveLeadingZeros(const DataStructure::SimpleVector<char> &vector);

  static DataStructure::SimpleVector<char>
  sum(const DataStructure::SimpleVector<char> &leftInput, const DataStructure::SimpleVector<char> &rightInput);

  static DataStructure::SimpleVector<char>
  sub(const DataStructure::SimpleVector<char> &left, const DataStructure::SimpleVector<char> &right);

  [[nodiscard]] BigInt abs() const;

  static DataStructure::SimpleVector<char>
  mul(const DataStructure::SimpleVector<char> &leftInput, const DataStructure::SimpleVector<char> &rightInput);

  static std::pair<DataStructure::SimpleVector<char>, DataStructure::SimpleVector<char>>
  div(const BigInt &leftInput, const BigInt &rightInput);

  enum BitFunc {
    PLUS, MULTIPLY, NOT_EQUAL
  };

  BigInt ApplyOperator(const BigInt &num2, BitFunc f);
};

}

