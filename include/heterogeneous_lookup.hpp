#pragma once

#include <string>
#include <string_view>

namespace bookdb {

struct TransparentStringLess {
    using is_transparent = std::true_type;

    bool operator()(const std::string &lhs, const std::string &rhs) const { return lhs < rhs; }
    bool operator()(const std::string_view &lhs, const std::string &rhs) const { return lhs < rhs; }
    bool operator()(const std::string &lhs, const std::string_view &rhs) const { return lhs < rhs; }
    bool operator()(const std::string_view &lhs, const std::string_view &rhs) const { return lhs < rhs; }
};

struct TransparentStringEqual {
    using is_transparent = std::true_type;

    bool operator()(const std::string &lhs, const std::string &rhs) const { return lhs == rhs; }
    bool operator()(const std::string_view &lhs, const std::string &rhs) const { return lhs == rhs; }
    bool operator()(const std::string &lhs, const std::string_view &rhs) const { return lhs == rhs; }
    bool operator()(const std::string_view &lhs, const std::string_view &rhs) const { return lhs == rhs; }
};

struct TransparentStringHash {
    using is_transparent = std::true_type;

    std::size_t operator()(std::string_view sv) const noexcept { return std::hash<std::string_view>{}(sv); }
};

}  // namespace bookdb
