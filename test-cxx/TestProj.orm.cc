// Auto-generated file. Changes will be overridden.
#include "TestProj.orm.h"

testproj::MyObject::MyObject(genORM::database& __db, uint64_t __id, int32_t i, std::optional<int32_t> io, std::vector<uint8_t> ba)
    : object(__db, __id), _i(i), _io(io), _ba(std::move(ba)) {}

std::expected<testproj::MyObject, std::string> testproj::MyObject::create(genORM::database& __db, int32_t i, std::optional<int32_t> io, std::vector<uint8_t> ba) {
    static constexpr std::string_view create_table_statement = "CREATE TABLE IF NOT EXISTS MyObject (__id INTEGER PRIMARY KEY NOT NULL, i INTEGER NOT NULL DEFAULT 0, io INTEGER  DEFAULT NULL, ba BLOB) STRICT;";
    if (auto create_table_result = create_table_if_not_exists(__db, create_table_statement); not create_table_result) { return std::unexpected{std::move(create_table_result.error())}; }
    {
        static constexpr std::string_view create_index_statement = "CREATE INDEX IF NOT EXISTS Index_MyObject_i ON MyObject(i);";
        if (auto create_index_result = create_index_if_not_exists(__db, create_index_statement); not create_index_result) { return std::unexpected{std::move(create_index_result.error())}; }
    }
    {
        static constexpr std::string_view create_index_statement = "CREATE INDEX IF NOT EXISTS Index_MyObject_io ON MyObject(io);";
        if (auto create_index_result = create_index_if_not_exists(__db, create_index_statement); not create_index_result) { return std::unexpected{std::move(create_index_result.error())}; }
    }
    static constexpr std::string_view insert_statement = "INSERT INTO MyObject VALUES (NULL, ?, ?, ?);";
    const auto binder = [&](int value_index) -> genORM::value_variant {
        if (value_index == 1) { return i; }
        if (value_index == 2) { return io ? genORM::value_variant{*io} : std::monostate{}; }
        if (value_index == 3) { return ba; }
        throw std::logic_error("Implementation error");
    };
    if (auto create_table_result = insert_into_table(__db, insert_statement, 3, binder)) {
        return MyObject{__db, *create_table_result, i, io, std::move(ba)};
    } else {
        return std::unexpected{std::move(create_table_result.error())};
    }
}
std::expected<std::optional<testproj::MyObject>, std::string> testproj::MyObject::find_by_rowid(genORM::database& __db, const uint64_t __id) {
    static constexpr std::string_view select_statement = "SELECT * FROM MyObject WHERE __id = ? LIMIT 1;";
    if (auto select_result = select_one(__db, select_statement, 1, [=](int) -> genORM::value_variant { return static_cast<int64_t>(__id); },
            std::vector<genORM::value_variant>{{int64_t{}, int32_t{}, int32_t{}, std::vector<uint8_t>{}}})) {
        if (*select_result) {
            return MyObject{__db, __id,
                std::get<int32_t>((**select_result)[1]),
                std::holds_alternative<int32_t>((**select_result)[2]) ? std::get<int32_t>((**select_result)[2]) : std::optional<int32_t>{},
                std::holds_alternative<std::vector<uint8_t>>((**select_result)[3]) ? std::move(std::get<std::vector<uint8_t>>((**select_result)[3])) : std::vector<uint8_t>{}
            };
        } else {
            return std::nullopt;
        }
    } else {
        return std::unexpected{std::move(select_result.error())};
    }
}
std::expected<std::optional<testproj::MyObject>, std::string> testproj::MyObject::find_first_by_i(genORM::database& __db, const int32_t i) {
    static constexpr std::string_view select_statement = "SELECT * FROM MyObject WHERE i = ? LIMIT 1;";
    if (auto select_result = select_one(__db, select_statement, 1, [=](int) -> genORM::value_variant { return i; },
            std::vector<genORM::value_variant>{{int64_t{}, int32_t{}, int32_t{}, std::vector<uint8_t>{}}})) {
        if (*select_result) {
            return MyObject{__db, static_cast<uint64_t>(std::get<int64_t>((**select_result)[0])),
                std::get<int32_t>((**select_result)[1]),
                std::holds_alternative<int32_t>((**select_result)[2]) ? std::get<int32_t>((**select_result)[2]) : std::optional<int32_t>{},
                std::holds_alternative<std::vector<uint8_t>>((**select_result)[3]) ? std::move(std::get<std::vector<uint8_t>>((**select_result)[3])) : std::vector<uint8_t>{}
            };
        } else {
            return std::nullopt;
        }
    } else {
        return std::unexpected{std::move(select_result.error())};
    }
}
std::expected<std::optional<testproj::MyObject>, std::string> testproj::MyObject::find_first_by_io(genORM::database& __db, const std::optional<int32_t> io) {
    static constexpr std::string_view select_statement = "SELECT * FROM MyObject WHERE io = ? LIMIT 1;";
    if (auto select_result = select_one(__db, select_statement, 1, [=](int) -> genORM::value_variant { return io ? genORM::value_variant{*io} : std::monostate{}; },
            std::vector<genORM::value_variant>{{int64_t{}, int32_t{}, int32_t{}, std::vector<uint8_t>{}}})) {
        if (*select_result) {
            return MyObject{__db, static_cast<uint64_t>(std::get<int64_t>((**select_result)[0])),
                std::get<int32_t>((**select_result)[1]),
                std::holds_alternative<int32_t>((**select_result)[2]) ? std::get<int32_t>((**select_result)[2]) : std::optional<int32_t>{},
                std::holds_alternative<std::vector<uint8_t>>((**select_result)[3]) ? std::move(std::get<std::vector<uint8_t>>((**select_result)[3])) : std::vector<uint8_t>{}
            };
        } else {
            return std::nullopt;
        }
    } else {
        return std::unexpected{std::move(select_result.error())};
    }
}
std::expected<std::vector<testproj::MyObject>, std::string> testproj::MyObject::find_all_by_i(genORM::database& __db, const int32_t i) {
    static constexpr std::string_view select_statement = "SELECT * FROM MyObject WHERE i = ?;";
    if (auto select_result = select_all(__db, select_statement, 1, [=](int) -> genORM::value_variant { return i; },
            std::vector<genORM::value_variant>{{int64_t{}, int32_t{}, int32_t{}, std::vector<uint8_t>{}}})) {
        std::vector<MyObject> found_objects;
        if (auto& rows = *select_result; not rows.empty()) {
            for (auto& row : rows) {
                found_objects.emplace_back(MyObject{__db, static_cast<uint64_t>(std::get<int64_t>(row[0])),
                    std::get<int32_t>(row[1]),
                    std::holds_alternative<int32_t>(row[2]) ? std::get<int32_t>(row[2]) : std::optional<int32_t>{},
                    std::holds_alternative<std::vector<uint8_t>>(row[3]) ? std::move(std::get<std::vector<uint8_t>>(row[3])) : std::vector<uint8_t>{}
                });
            }
        }
        return found_objects;
    } else {
        return std::unexpected{std::move(select_result.error())};
    }
}
std::expected<std::vector<testproj::MyObject>, std::string> testproj::MyObject::find_all_by_io(genORM::database& __db, const std::optional<int32_t> io) {
    static constexpr std::string_view select_statement = "SELECT * FROM MyObject WHERE io = ?;";
    if (auto select_result = select_all(__db, select_statement, 1, [=](int) -> genORM::value_variant { return io ? genORM::value_variant{*io} : std::monostate{}; },
            std::vector<genORM::value_variant>{{int64_t{}, int32_t{}, int32_t{}, std::vector<uint8_t>{}}})) {
        std::vector<MyObject> found_objects;
        if (auto& rows = *select_result; not rows.empty()) {
            for (auto& row : rows) {
                found_objects.emplace_back(MyObject{__db, static_cast<uint64_t>(std::get<int64_t>(row[0])),
                    std::get<int32_t>(row[1]),
                    std::holds_alternative<int32_t>(row[2]) ? std::get<int32_t>(row[2]) : std::optional<int32_t>{},
                    std::holds_alternative<std::vector<uint8_t>>(row[3]) ? std::move(std::get<std::vector<uint8_t>>(row[3])) : std::vector<uint8_t>{}
                });
            }
        }
        return found_objects;
    } else {
        return std::unexpected{std::move(select_result.error())};
    }
}
testproj::MySecondObject::MySecondObject(genORM::database& __db, uint64_t __id, int64_t l, std::optional<int64_t> lo)
    : object(__db, __id), _l(l), _lo(lo) {}

std::expected<testproj::MySecondObject, std::string> testproj::MySecondObject::create(genORM::database& __db, int64_t l, std::optional<int64_t> lo) {
    static constexpr std::string_view create_table_statement = "CREATE TABLE IF NOT EXISTS MySecondObject (__id INTEGER PRIMARY KEY NOT NULL, l INTEGER NOT NULL DEFAULT 0, lo INTEGER  DEFAULT NULL) STRICT;";
    if (auto create_table_result = create_table_if_not_exists(__db, create_table_statement); not create_table_result) { return std::unexpected{std::move(create_table_result.error())}; }
    {
        static constexpr std::string_view create_index_statement = "CREATE INDEX IF NOT EXISTS Index_MySecondObject_lo ON MySecondObject(lo);";
        if (auto create_index_result = create_index_if_not_exists(__db, create_index_statement); not create_index_result) { return std::unexpected{std::move(create_index_result.error())}; }
    }
    static constexpr std::string_view insert_statement = "INSERT INTO MySecondObject VALUES (NULL, ?, ?);";
    const auto binder = [&](int value_index) -> genORM::value_variant {
        if (value_index == 1) { return l; }
        if (value_index == 2) { return lo ? genORM::value_variant{*lo} : std::monostate{}; }
        throw std::logic_error("Implementation error");
    };
    if (auto create_table_result = insert_into_table(__db, insert_statement, 2, binder)) {
        return MySecondObject{__db, *create_table_result, l, lo};
    } else {
        return std::unexpected{std::move(create_table_result.error())};
    }
}
std::expected<std::optional<testproj::MySecondObject>, std::string> testproj::MySecondObject::find_by_rowid(genORM::database& __db, const uint64_t __id) {
    static constexpr std::string_view select_statement = "SELECT * FROM MySecondObject WHERE __id = ? LIMIT 1;";
    if (auto select_result = select_one(__db, select_statement, 1, [=](int) -> genORM::value_variant { return static_cast<int64_t>(__id); },
            std::vector<genORM::value_variant>{{int64_t{}, int64_t{}, int64_t{}}})) {
        if (*select_result) {
            return MySecondObject{__db, __id,
                std::get<int64_t>((**select_result)[1]),
                std::holds_alternative<int64_t>((**select_result)[2]) ? std::get<int64_t>((**select_result)[2]) : std::optional<int64_t>{}
            };
        } else {
            return std::nullopt;
        }
    } else {
        return std::unexpected{std::move(select_result.error())};
    }
}
std::expected<std::optional<testproj::MySecondObject>, std::string> testproj::MySecondObject::find_first_by_lo(genORM::database& __db, const std::optional<int64_t> lo) {
    static constexpr std::string_view select_statement = "SELECT * FROM MySecondObject WHERE lo = ? LIMIT 1;";
    if (auto select_result = select_one(__db, select_statement, 1, [=](int) -> genORM::value_variant { return lo ? genORM::value_variant{*lo} : std::monostate{}; },
            std::vector<genORM::value_variant>{{int64_t{}, int64_t{}, int64_t{}}})) {
        if (*select_result) {
            return MySecondObject{__db, static_cast<uint64_t>(std::get<int64_t>((**select_result)[0])),
                std::get<int64_t>((**select_result)[1]),
                std::holds_alternative<int64_t>((**select_result)[2]) ? std::get<int64_t>((**select_result)[2]) : std::optional<int64_t>{}
            };
        } else {
            return std::nullopt;
        }
    } else {
        return std::unexpected{std::move(select_result.error())};
    }
}
std::expected<std::vector<testproj::MySecondObject>, std::string> testproj::MySecondObject::find_all_by_lo(genORM::database& __db, const std::optional<int64_t> lo) {
    static constexpr std::string_view select_statement = "SELECT * FROM MySecondObject WHERE lo = ?;";
    if (auto select_result = select_all(__db, select_statement, 1, [=](int) -> genORM::value_variant { return lo ? genORM::value_variant{*lo} : std::monostate{}; },
            std::vector<genORM::value_variant>{{int64_t{}, int64_t{}, int64_t{}}})) {
        std::vector<MySecondObject> found_objects;
        if (auto& rows = *select_result; not rows.empty()) {
            for (auto& row : rows) {
                found_objects.emplace_back(MySecondObject{__db, static_cast<uint64_t>(std::get<int64_t>(row[0])),
                    std::get<int64_t>(row[1]),
                    std::holds_alternative<int64_t>(row[2]) ? std::get<int64_t>(row[2]) : std::optional<int64_t>{}
                });
            }
        }
        return found_objects;
    } else {
        return std::unexpected{std::move(select_result.error())};
    }
}
