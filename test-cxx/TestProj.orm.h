// Auto-generated file. Changes will be overridden.
#pragma once
#include <genORM/genORM.h>
#include <vector>
#include <string>
#include <optional>

namespace testproj {
    /// This is an object that's created for test purposes.
    class MyObject final : public genORM::object {
        /// This is some integer
        int32_t _i;
        /// This is some optional integer
        std::optional<int32_t> _io;
        /// This is some byte array
        std::vector<uint8_t> _ba;

        explicit MyObject(genORM::database& __db, uint64_t __id, int32_t i, std::optional<int32_t> io, std::vector<uint8_t> ba);

    public:
        static std::expected<MyObject, std::string> create(genORM::database& __db, int32_t i, std::optional<int32_t> io, std::vector<uint8_t> ba);
        static std::expected<std::optional<MyObject>, std::string> find_by_rowid(genORM::database& __db, uint64_t);
        static std::expected<std::optional<MyObject>, std::string> find_first_by_i(genORM::database& __db, int32_t i);
        static std::expected<std::vector<MyObject>, std::string> find_all_by_i(genORM::database& __db, int32_t i);
        static std::expected<std::optional<MyObject>, std::string> find_first_by_io(genORM::database& __db, std::optional<int32_t> io);
        static std::expected<std::vector<MyObject>, std::string> find_all_by_io(genORM::database& __db, std::optional<int32_t> io);

        [[nodiscard]] int32_t get_i() const { return _i; }
        [[nodiscard]] std::optional<int32_t> get_io() const { return _io; }
        [[nodiscard]] const std::vector<uint8_t>& get_ba() const { return _ba; }
    };

    class MySecondObject final : public genORM::object {
        int64_t _l;
        std::optional<int64_t> _lo;

        explicit MySecondObject(genORM::database& __db, uint64_t __id, int64_t l, std::optional<int64_t> lo);

    public:
        static std::expected<MySecondObject, std::string> create(genORM::database& __db, int64_t l, std::optional<int64_t> lo);
        static std::expected<std::optional<MySecondObject>, std::string> find_by_rowid(genORM::database& __db, uint64_t);
        static std::expected<std::optional<MySecondObject>, std::string> find_first_by_lo(genORM::database& __db, std::optional<int64_t> lo);
        static std::expected<std::vector<MySecondObject>, std::string> find_all_by_lo(genORM::database& __db, std::optional<int64_t> lo);

        [[nodiscard]] int64_t get_l() const { return _l; }
        [[nodiscard]] std::optional<int64_t> get_lo() const { return _lo; }
    };

}
