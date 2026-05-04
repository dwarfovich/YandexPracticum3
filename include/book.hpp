#pragma once

#include <format>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>

namespace bookdb {

inline static constexpr int invalid_year = std::numeric_limits<int>::lowest();
inline static constexpr double invalid_rating = std::numeric_limits<double>::lowest();
inline static constexpr std::string_view invalid_author = "";

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

constexpr Genre GenreFromString(std::string_view s) {
    if (s == "Fiction") {
        return Genre::Fiction;
    } else if (s == "NonFiction") {
        return Genre::NonFiction;
    } else if (s == "SciFi") {
        return Genre::SciFi;
    } else if (s == "Biography") {
        return Genre::Biography;
    } else if (s == "Mystery") {
        return Genre::Mystery;
    } else {
        return Genre::Unknown;
    }
}

constexpr std::string_view ToStringView(Genre genre) {
    switch (genre) {
    case bookdb::Genre::Fiction:
        return "Fiction";
    case bookdb::Genre::NonFiction:
        return "NonFiction";
    case bookdb::Genre::SciFi:
        return "SciFi";
    case bookdb::Genre::Biography:
        return "Biography";
    case bookdb::Genre::Mystery:
        return "Mystery";
    default:
        return "Unknown";
    }
}

struct Book {
    constexpr Book() = default;
    constexpr Book(Genre g) : genre{g} {}
    constexpr Book(std::string t, const std::string_view &a, int y = invalid_year, Genre g = Genre::Unknown, double r = invalid_rating, int rc = 0)
        : author{a}, title{t}, year{y}, genre{g}, rating{r}, read_count{rc} {}
    constexpr Book(std::string_view newGenre) : genre{GenreFromString(newGenre)} {}

    std::string title;
    std::string_view author = invalid_author;
    int year = invalid_year;
    Genre genre = Genre::Unknown;
    double rating = invalid_rating;
    int read_count = 0;
};

}  // namespace bookdb

namespace std {

template <>
struct formatter<bookdb::Genre, char> {
    template <typename FormatContext>
    auto format(const bookdb::Genre g, FormatContext &fc) const {
        return format_to(fc.out(), "{}", ToStringView(g));
    }

    constexpr auto parse(format_parse_context &ctx) const {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

template <>
struct formatter<bookdb::Book, char> {
    template <typename FormatContext>
    auto format(const bookdb::Book& b, FormatContext &fc) const {
        return std::format_to(fc.out(), "{} | {} | {} | {} | {}", b.title, b.author, b.genre, b.year, b.rating);
    }

    constexpr auto parse(format_parse_context &ctx) const {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

}  // namespace std