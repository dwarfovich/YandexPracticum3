#pragma once

#include "book.hpp"
#include "concepts.hpp"

#include <algorithm>

namespace bookdb {

constexpr auto YearBetween(int start, int end) noexcept {
    return [=](const Book& book){ return book.year >= start && book.year <= end; };
}
constexpr auto RatingAbove(double min_rating) noexcept {
    return [=](const Book &book) { return book.rating >= min_rating; };
}
constexpr auto GenreIs(Genre genre) noexcept {
    return [=](const Book &book) { return book.genre == genre; };
}

template <BookIterator It, BookSentinel<It> Sent, BookPredicate Pred>
auto filterBooks(It first, Sent last, Pred pred){
    std::vector < std::reference_wrapper<const Book> > result;
    std::copy_if(first, last, std::back_inserter(result), [&pred](const auto& book){ return pred(book);});

    return result;
}

template <BookPredicate... Preds>
struct FilterAllOf {
    std::tuple<Preds...> preds;

    constexpr bool operator()(const Book &book) const noexcept {
        return std::apply([&](const auto&... p) { return (p(book) && ...); }, preds);
    }
};

template <BookPredicate... Preds>
struct FilterAnyOf {
    std::tuple<Preds...> preds;

    constexpr bool operator()(const Book &book) const noexcept {
        return std::apply([&](const auto &...p) { return (p(book) || ...); }, preds);
    }
};

template <BookPredicate... Preds>
constexpr auto all_of(Preds... preds) noexcept {
    return FilterAllOf<Preds...>{std::tuple{preds...}};
}

template <BookPredicate... Preds>
constexpr auto any_of(Preds... preds) noexcept {
    return FilterAnyOf<Preds...>{std::tuple{preds...}};
}

}  // namespace bookdb