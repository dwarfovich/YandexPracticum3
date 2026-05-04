#pragma once

#include <print>
#include <span>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    using value_type = BookContainer::value_type;
    using reference = BookContainer::reference;
    using const_reference = BookContainer::const_reference;
    using pointer = BookContainer::pointer;
    using const_pointer = BookContainer::const_pointer;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = BookContainer::iterator;
    using const_iterator = BookContainer::const_iterator;

    using AuthorContainer = std::unordered_set<std::string, TransparentStringHash, std::equal_to<>>;

    BookDatabase() = default;
    BookDatabase(std::initializer_list<Book> books) {
        for (auto book : books) {
            EmplaceBack(std::move(book));
        }
    }

    iterator begin() { return books_.begin(); }
    iterator end() { return books_.end(); }
    const_iterator begin() const { return books_.begin(); }
    const_iterator end() const { return books_.end(); }
    const_iterator cbegin() const { return books_.cbegin(); }
    const_iterator cend() const { return books_.cend(); }

    size_type size() const { return books_.size(); }

    void PushBack(const Book &book) { AddBook(Book{book}); }
    void PushBack(Book &&book) { AddBook(std::move(book)); }
    template <typename... Args>
        requires std::constructible_from<Book, Args...>
    void EmplaceBack(Args &&...args) {
        books_.emplace_back(std::forward<Args>(args)...);
        const auto &[iter, inserted] = authors_.insert(std::string(books_.back().author));
        books_.back().author = *iter;
    }

    void Clear() {
        books_.clear();
        authors_.clear();
    }

    std::span<const Book> GetBooks() const { return books_; }
    std::span<Book> GetBooks() { return books_; }
    const AuthorContainer &GetAuthors() { return authors_; }
    const AuthorContainer &GetAuthors() const { return authors_; }

private:  // methods
    void AddBook(Book &&book) {
        auto iter = authors_.find(book.author);
        // We assume that the current author is most probable had been already inserted, tradeoff - excess hashing for new author.
        if (iter == authors_.cend()){
            const auto &[iter, inserted] = authors_.insert(std::string{book.author});
            book.author = *iter;
        }
        books_.push_back(std::move(book));
    }

private:  // data
    BookContainer books_;
    AuthorContainer authors_;
};

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::BookDatabase<std::vector<bookdb::Book>>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<std::vector<bookdb::Book>> &db, FormatContext &fc) const {
        format_to(fc.out(), "BookDatabase (size = {}): ", db.size());

        format_to(fc.out(), "Books:\n");
        for (const auto &book : db.GetBooks()) {
            format_to(fc.out(), "- {}\n", book);
        }

        format_to(fc.out(), "Authors:\n");
        for (const auto &author : db.GetAuthors()) {
            format_to(fc.out(), "- {}\n", author);
        }

        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
}  // namespace std
