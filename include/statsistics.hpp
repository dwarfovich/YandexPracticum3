#pragma once

#include "book_database.hpp"

#include <boost/container/flat_map.hpp>

#include <algorithm>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string_view>
#include <print>

namespace bookdb {

template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T> &db) {

    using FlatMap = boost::container::flat_map<std::string_view, std::size_t, Comparator>;
    FlatMap map;
    for (const auto& book : db.GetBooks()){
        ++map[book.author];
    }

    return map;
}

template <BookIterator T>
auto calculateGenreRatings(T begin, T end) {
    return 0;
}

}  // namespace bookdb
