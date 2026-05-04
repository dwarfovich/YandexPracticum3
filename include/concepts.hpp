#pragma once

#include "book.hpp"

#include <concepts>
#include <iterator>

namespace bookdb {

template <typename T>
concept BookIterator = std::bidirectional_iterator<T>;

template <typename T>
concept BookContainerLike = BookIterator<typename T::iterator> &&
                            std::is_same_v<Book, typename T::value_type> && requires(T a, Book book) {
                                a.push_back(book);
                                a.emplace_back(book);
                                { a.back() } -> std::same_as<Book &>;
                            };

template <typename S, typename I>
concept BookSentinel = BookIterator<I> && std::sentinel_for<S, I>;

template <typename P>
concept BookPredicate = std::predicate<P, const Book &>;

template <typename C>
concept BookComparator = requires(C c, const Book &book) {
    { c(book, book) } -> std::convertible_to<bool>;
};

}  // namespace bookdb