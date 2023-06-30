#pragma once
#include <iostream>
#include <tuple>

template<class Ch, class Tr, class Tuple, std::size_t N>
static void printTuple(std::basic_ostream<Ch, Tr> &out, const Tuple &tuple) {
  constexpr std::size_t index = N - 1;
  if constexpr (index > 0) {
	printTuple<Ch, Tr, Tuple, index>(out, tuple);
	out << ", " << std::get<index>(tuple);
  } else {
	out << std::get<0>(tuple);
  }

}

template<typename Ch, typename Tr, typename ...Types>
std::ostream &operator<<(std::basic_ostream<Ch, Tr> &outStream, const std::tuple<Types...> &tuple) {
  std::cout << "(";
  printTuple<Ch, Tr, decltype(tuple), sizeof...(Types)>(outStream, tuple);
  std::cout << ")\n";
  return outStream;
}