#pragma once

#include <concepts>
#include <iterator>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookIterator = std::bidirectional_iterator<T>;

template <typename T>
concept BookContainerLike = BookIterator<typename T::iterator> &&
                            std::is_same_v<bookdb::Book, typename T::value_type> && requires(T a, bookdb::Book book) {
                                a.push_back(book);
                                a.emplace_back(book);
                                { a.back() } -> std::same_as<bookdb::Book&>;
                            };

template <typename S, typename I>
concept BookSentinel = BookIterator<I> && std::sentinel_for<S, I>;

template <typename P>
concept BookPredicate = true;

template <typename C>
concept BookComparator = true;

}  // namespace bookdb