#pragma once

#include "book_database.hpp"
#include "comparators.hpp"

#include <boost/container/flat_map.hpp>
#include <boost/unordered/unordered_flat_map.hpp>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <print>
#include <random>
#include <stdexcept>
#include <string_view>

namespace bookdb {

template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T> &db) {

    using FlatMap = boost::container::flat_map<std::string_view, std::size_t, Comparator>;
    FlatMap map;
    for (const auto &book : db.GetBooks()) {
        ++map[book.author];
    }

    return map;
}

template <BookIterator It, BookSentinel<It> Sent>
auto calculateGenreRatings(It begin, Sent end) {
    using GenresStatisticsUMap = boost::unordered::unordered_flat_map<Genre, std::pair<double, size_t>>;
    GenresStatisticsUMap map;
    static constexpr std::size_t reservationSize = 100;
    map.reserve(reservationSize);
    for (; begin != end; ++begin) {
        if (begin->rating == invalid_rating) {
            continue;
        }

        auto &statistics = map[begin->genre];
        statistics.first += begin->rating;
        ++statistics.second;
    }
    for (auto &[genre, statistics] : map) {
        statistics.first /= statistics.second;
    }

    return map;
}

template <BookContainerLike T>
double calculateAverageRating(const BookDatabase<T> &books) {
    std::size_t count = 0;
    auto rating = std::accumulate(books.cbegin(), books.cend(), 0., [&count](double acc, const auto &book) {
        if (book.rating == invalid_rating) {
            return acc;
        } else {
            ++count;
            return acc + book.rating;
        }
    });

    if (count == 0) {
        return invalid_rating;
    } else {
        return rating / count;
    }
}

template <BookContainerLike T>
auto sampleRandomBooks(const BookDatabase<T> &db, size_t num) {
    std::vector<std::reference_wrapper<const Book>> samples;
    samples.reserve(num);
    std::mt19937 generator{std::random_device{}()};
    std::sample(db.cbegin(), db.cend(), std::back_inserter(samples), num, generator);

    return samples;
}

template <BookContainerLike T, typename Comparator = bookdb::comp::LessByPopularity>
auto getTopNBy(BookDatabase<T> &db, size_t n, Comparator comp = {}) {
    std::partial_sort(db.begin(), db.begin() + n, db.end(), comp);
    
    return std::vector<std::reference_wrapper<const Book>>{db.begin(), db.begin() + n};
}

}  // namespace bookdb
