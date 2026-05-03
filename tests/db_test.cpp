#include "book_database.hpp"
#include "filters.hpp"
#include "statistics.hpp"

#include <gtest/gtest.h>

#include <deque>

using namespace bookdb;

TEST(TestDb, TestInnerTypes) {
    using BookContainer = BookDatabase<std::vector<Book>>;
    static_assert(requires { typename BookContainer::value_type; });
    static_assert(requires { typename BookContainer::reference; });
    static_assert(requires { typename BookContainer::const_reference; });
    static_assert(requires { typename BookContainer::pointer; });
    static_assert(requires { typename BookContainer::const_pointer; });
    static_assert(requires { typename BookContainer::difference_type; });
    static_assert(requires { typename BookContainer::iterator; });
    static_assert(requires { typename BookContainer::const_iterator; });
    static_assert(requires { typename BookContainer::AuthorContainer; });
}

TEST(TestDb, TestEmptyDb) {
    BookDatabase<std::vector<Book>> db;
    ASSERT_EQ(db.size(), 0);
    ASSERT_EQ(db.GetAuthors().size(), 0);
}

TEST(TestDb, TestPushBack) {
    BookDatabase<std::vector<Book>> db;

    db.PushBack(Book{"a", "a"});
    ASSERT_EQ(db.GetBooks().front().author, "a");
    ASSERT_EQ(*db.GetAuthors().begin(), "a");
    db.PushBack(Book{"b", "b"});
    ASSERT_EQ(db.size(), 2);
    ASSERT_EQ(db.GetAuthors().size(), 2);
    db.PushBack(Book{"c", "a"});
    ASSERT_EQ(db.size(), 3);
    ASSERT_EQ(db.GetAuthors().size(), 2);
    Book book{"d", "d"};
    db.PushBack(book);
    ASSERT_EQ(db.size(), 4);
    ASSERT_EQ(db.GetAuthors().size(), 3);
}

TEST(TestDb, TestEmplaceBack) {
    BookDatabase<std::vector<Book>> db;

    db.EmplaceBack("a", "a");
    ASSERT_EQ(db.size(), 1);
    ASSERT_EQ(db.GetAuthors().size(), 1);
    db.EmplaceBack("b", "a", 1900, Genre::Biography, 3., 1);
    ASSERT_EQ(db.size(), 2);
    ASSERT_EQ(db.GetAuthors().size(), 1);
}

TEST(TestDb, TestClear) {
    BookDatabase<std::vector<Book>> db;
    db.PushBack(Book{"a", "a"});
    db.PushBack(Book{"b", "b"});
    db.PushBack(Book{"c", "a"});
    db.Clear();
    ASSERT_EQ(db.size(), 0);
    ASSERT_EQ(db.GetAuthors().size(), 0);
}

TEST(TestFilters, TestYearBetweenNegative) {
    auto p = YearBetween(-10, -1);
    ASSERT_TRUE(p(Book{"", "", -10}));
    ASSERT_TRUE(p(Book{"", "", -1}));
    ASSERT_FALSE(p(Book{"", "", -11}));
    ASSERT_FALSE(p(Book{"", "", 0}));
}

TEST(TestFilters, TestYearBetweenZero) {
    auto p = YearBetween(-10, 10);
    ASSERT_TRUE(p(Book{"", "", 0}));
    ASSERT_TRUE(p(Book{"", "", -10}));
    ASSERT_TRUE(p(Book{"", "", 10}));
    ASSERT_FALSE(p(Book{"", "", -11}));
    ASSERT_FALSE(p(Book{"", "", 11}));
}

TEST(TestFilters, TestGenreIs) {
    auto p = GenreIs(Genre::Biography);
    ASSERT_TRUE(p(Book{"", "", 0, Genre::Biography}));
    ASSERT_FALSE(p(Book{"", "", 0, Genre::Unknown}));
    ASSERT_FALSE(p(Book{"", "", 0, Genre::Fiction}));
}

TEST(TestFilters, TestAllOfSinglePredicate) {
    auto p = all_of(YearBetween(1900, 1999));
    ASSERT_TRUE(p(Book{"", "", 1950, Genre::Biography, 3.}));
    ASSERT_FALSE(p(Book{"", "", 1899, Genre::Biography, 3.}));
    ASSERT_FALSE(p(Book{"", "", 2000, Genre::Biography, 3.}));
}

TEST(TestFilters, TestAllOfMultiplePredicats) {
    auto p = all_of(YearBetween(1900, 1999), RatingAbove(3.));
    ASSERT_TRUE(p(Book{"", "", 1950, Genre::Biography, 3.}));
    ASSERT_TRUE(p(Book{"", "", 1950, Genre::Biography, 55.}));
    ASSERT_FALSE(p(Book{"", "", 1899, Genre::Biography, 10.}));
    ASSERT_FALSE(p(Book{"", "", 2000, Genre::Biography, 30.}));
    ASSERT_FALSE(p(Book{"", "", 1950, Genre::Biography, 2.}));
}

TEST(TestFilters, TestAnyOfSinglePredicate) {
    auto p = any_of(YearBetween(1900, 1999));
    ASSERT_TRUE(p(Book{"", "", 1950, Genre::Biography, 3.}));
    ASSERT_FALSE(p(Book{"", "", 1899, Genre::Biography, 3.}));
    ASSERT_FALSE(p(Book{"", "", 2000, Genre::Biography, 3.}));
}

TEST(TestFilters, TestAnyOfMultiplePredicats) {
    auto p = any_of(YearBetween(1900, 1999), RatingAbove(3.));
    ASSERT_TRUE(p(Book{"", "", 1950, Genre::Biography, 3.}));
    ASSERT_TRUE(p(Book{"", "", 1950, Genre::Biography, 1.}));
    ASSERT_TRUE(p(Book{"", "", 2999, Genre::Biography, 5.}));
    ASSERT_FALSE(p(Book{"", "", 1899, Genre::Biography, 2.}));
}

TEST(TestFilters, TestFilterOnEmptyDb) {
    using BookContainer = BookDatabase<std::deque<Book>>;
    BookContainer db;
    auto filtered = filterBooks(db.begin(), db.end(), all_of(YearBetween(1900, 1999), RatingAbove(4.5)));
    ASSERT_TRUE(filtered.empty());
}

TEST(TestFilters, TestFilter) {
    using BookContainer = BookDatabase<std::list<Book>>;
    BookContainer db;
    auto filtered = filterBooks(db.begin(), db.end(), all_of(YearBetween(1900, 1999), RatingAbove(4.5)));
    ASSERT_TRUE(filtered.empty());

    db.PushBack(Book{"", "", 1950, Genre::Biography, 3.});
    filtered = filterBooks(db.begin(), db.end(), all_of(YearBetween(1900, 1999), RatingAbove(4.5)));
    ASSERT_TRUE(filtered.empty());

    db.PushBack(Book{"", "", 1950, Genre::Biography, 4.5});
    filtered = filterBooks(db.begin(), db.end(), all_of(YearBetween(1900, 1999), RatingAbove(4.5)));
    ASSERT_EQ(filtered.size(), 1);

    db.PushBack(Book{"", "", 1300, Genre::Biography, 4.5});
    filtered = filterBooks(db.begin(), db.end(), all_of(YearBetween(1900, 1999), RatingAbove(4.5)));
    ASSERT_EQ(filtered.size(), 1);

    db.PushBack(Book{"", "", 1999, Genre::Biography, 100.});
    filtered = filterBooks(db.begin(), db.end(), all_of(YearBetween(1900, 1999), RatingAbove(4.5)));
    ASSERT_EQ(filtered.size(), 2);
}

TEST(TestStatistics, TestHistogram) {
    BookDatabase<std::vector<Book>> db;
    auto histogram = buildAuthorHistogramFlat(db);
    ASSERT_TRUE(histogram.empty());

    db.EmplaceBack("1", "a");
    histogram = buildAuthorHistogramFlat(db);
    ASSERT_EQ(histogram.size(), 1);
    
    db.EmplaceBack("2", "a");
    histogram = buildAuthorHistogramFlat(db);
    ASSERT_EQ(histogram.size(), 1);
    ASSERT_EQ(histogram.begin()->second, 2);

    db.EmplaceBack("3", "b");
    histogram = buildAuthorHistogramFlat(db);
    ASSERT_EQ(histogram.size(), 2);
    ASSERT_EQ(histogram["a"], 2);
    ASSERT_EQ(histogram["b"], 1);

    db.EmplaceBack("3", "a");
    histogram = buildAuthorHistogramFlat(db);
    ASSERT_EQ(histogram.size(), 2);
    ASSERT_EQ(histogram["a"], 3);
    ASSERT_EQ(histogram["b"], 1);
}

/*
 std::string title;
    std::string_view author = invalid_author;
    int year = invalid_year;
    Genre genre = Genre::Unknown;
    double rating = invalid_rating;
    int read_count = 0;
    */
TEST(TestStatistics, TestCalculateGenreRatings) {
    BookDatabase<std::deque<Book>> db;
    auto genreRatings = calculateGenreRatings(db.begin(), db.end());
    ASSERT_TRUE(genreRatings.empty());

    db.EmplaceBack("1", "a", 1900, Genre::Biography, 1.);
    genreRatings = calculateGenreRatings(db.begin(), db.end());
    ASSERT_DOUBLE_EQ(genreRatings[Genre::Biography].first, 1.);

    db.EmplaceBack("1", "b", 1900, Genre::Biography, 2.);
    genreRatings = calculateGenreRatings(db.begin(), db.end());
    ASSERT_DOUBLE_EQ(genreRatings[Genre::Biography].first, 1.5);

    db.EmplaceBack("1", "c", 1900, Genre::Fiction, 2.);
    genreRatings = calculateGenreRatings(db.begin(), db.end());
    ASSERT_DOUBLE_EQ(genreRatings[Genre::Biography].first, 1.5);
    ASSERT_DOUBLE_EQ(genreRatings[Genre::Fiction].first, 2.);
}

TEST(TestStatistics, TestCalculateGenreRatings) {
    }