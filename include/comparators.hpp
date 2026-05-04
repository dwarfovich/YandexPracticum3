#pragma once

#include "book.hpp"

namespace bookdb::comp {

struct LessByAuthor {
    bool operator()(const Book& lhs, const Book& rhs) {
        return lhs.author < rhs.author;
    }
};

struct LessByPopularity {
    bool operator()(const Book &lhs, const Book &rhs) { return lhs.read_count < rhs.read_count; }
};

struct LessByRating {
    bool operator()(const Book &lhs, const Book &rhs) { return lhs.rating < rhs.rating; }
};


}  // namespace bookdb::comp