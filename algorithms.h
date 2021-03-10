#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

#include <iterator>
#include <functional>

template<typename Iterator>
bool IsSorted(Iterator begin, Iterator end) {
  if (begin == end) {
    return true;
  }
  Iterator iter{begin};
  Iterator next{++begin};
  while (next != end) {
    if (*next < *iter) {
      return false;
    }
    iter = next;
    ++next;
  }
  return true;
}

template<typename Iterator>
Iterator MaxElement(Iterator begin, Iterator end) {
  if (begin == end) {
    return end;
  }
  Iterator candidate{begin};
  Iterator iter{begin};
  ++iter;
  while (iter != end) {
    if (*candidate < *iter) {
      candidate = iter;
    }
    ++iter;
  }
  return candidate;
}

template<typename Iterator>
typename std::iterator_traits<Iterator>::value_type
Accumulate(Iterator begin, Iterator end) {
  typename std::iterator_traits<Iterator>::value_type accumulator{};
  for (Iterator iter{begin}; iter != end; ++iter) {
    accumulator += *iter;
  }
  return accumulator;
}

template<typename Iterator, typename FunctorType>
int CountIf(Iterator begin, Iterator end, FunctorType pred) {
  int accumulator{0};
  for (Iterator iter{begin}; iter != end; ++iter) {
    accumulator += (pred(*iter) ? 1 : 0);
  }
  return accumulator;
}

template<typename Iterator>
int CountIfNot(
    Iterator begin,
    Iterator end,
    bool (*pred)(const typename std::iterator_traits<Iterator>::value_type&)) {
  int accumulator{0};
  for (Iterator iter{begin}; iter != end; ++iter) {
    accumulator += (pred(*iter) ? 0 : 1);
  }
  return accumulator;
}

template<typename Iterator>
Iterator FindIf(
    Iterator begin,
    Iterator end,
    std::function<bool
        (const typename std::iterator_traits<Iterator>::value_type&)> pred) {
  for (Iterator iter{begin}; iter != end; ++iter) {
    if (pred(*iter)) {
      return iter;
    }
  }
  return end;
}

#endif  // ALGORITHMS_H_
