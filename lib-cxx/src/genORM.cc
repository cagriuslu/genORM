#include <genORM/genORM.h>
#include <sqlite3.h>

namespace {
	template<class... Ts>
	struct overloaded : Ts... { using Ts::operator()...; };

	void binder_none(void*, int) {}

	bool op_ignore(void*) { return false; }

	std::expected<void, std::string> prepare_bind_execute_statement(sqlite3* db, const std::string_view statement, const int value_count = 0,
			const std::function<void(void* sqlite_statement, int value_index)>& binder = binder_none,
			const std::function<bool(void* sqlite_statement)>& op = op_ignore) {
		sqlite3_stmt* sqlite_statement{};
		if (const auto prepare_result = sqlite3_prepare_v2(db, statement.data(), static_cast<int>(statement.size()), &sqlite_statement, nullptr); prepare_result != SQLITE_OK) {
			return std::unexpected(std::string{sqlite3_errstr(prepare_result)});
		}

		for (int i = 1; i <= value_count; ++i) {
			binder(sqlite_statement, i);
		}

		int32_t step_result;
		while ((step_result = sqlite3_step(sqlite_statement)) == SQLITE_ROW) {
			if (not op(sqlite_statement)) { break; }
		}
		sqlite3_finalize(sqlite_statement);
		if (step_result != SQLITE_DONE && step_result != SQLITE_ROW) {
			return std::unexpected(std::string{sqlite3_errstr(step_result)});
		}
		return {};
	}

	uint64_t last_insert_rowid(sqlite3* db) {
		return static_cast<uint64_t>(sqlite3_last_insert_rowid(db));
	}

	struct value_mapper {
		const genORM::value_binder& binder;
		std::expected<void, std::string> bind_result{};

		// This function is repeated called for every value being binded.
		// `binder` is called with each index, which should return a value_variant.
		// This value_variant is then binded into the sqlite statement.
		void operator()(void* opaque_sqlite_statement, int value_index) {
			// Do not continue if an error occurred previously
			if (not bind_result) { return; }
			auto* sqlite_statement = static_cast<sqlite3_stmt*>(opaque_sqlite_statement);

			std::visit(overloaded{
				[&](std::monostate) {
					if (const auto result = sqlite3_bind_null(sqlite_statement, value_index); result != SQLITE_OK) {
						bind_result = std::unexpected(std::string{sqlite3_errstr(result)});
					}
				},
				[&](const int32_t i) {
					if (const auto result = sqlite3_bind_int(sqlite_statement, value_index, i); result != SQLITE_OK) {
						bind_result = std::unexpected(std::string{sqlite3_errstr(result)});
					}
				},
				[&](const int64_t l) {
					if (const auto result = sqlite3_bind_int64(sqlite_statement, value_index, l); result != SQLITE_OK) {
						bind_result = std::unexpected(std::string{sqlite3_errstr(result)});
					}
				},
				[&](const std::vector<uint8_t>& bytes) {
					if (const auto result = sqlite3_bind_blob64(sqlite_statement, value_index, bytes.data(), bytes.size(), SQLITE_TRANSIENT); result != SQLITE_OK) {
						bind_result = std::unexpected(std::string{sqlite3_errstr(result)});
					}
				}
			}, binder(value_index));
		}
	};
}

using namespace genORM;

std::expected<void,std::string> object::create_table_if_not_exists(database& db, const std::string_view statement) {
	return prepare_bind_execute_statement(static_cast<sqlite3*>(db._db_handle), statement);
}

std::expected<void, std::string> object::create_index_if_not_exists(database& db, std::string_view statement) {
	return prepare_bind_execute_statement(static_cast<sqlite3*>(db._db_handle), statement);
}

std::expected<uint64_t, std::string> object::insert_into_table(database& db, const std::string_view statement, const int value_count, const value_binder& binder) {
	auto mapper = value_mapper{.binder = binder};
	auto execute_result = prepare_bind_execute_statement(static_cast<sqlite3*>(db._db_handle), statement, value_count, mapper);
	if (not mapper.bind_result) {
		return std::unexpected{std::move(mapper.bind_result.error())};
	}
	if (not execute_result) {
		return std::unexpected{std::move(execute_result.error())};
	}
	return last_insert_rowid(static_cast<sqlite3*>(db._db_handle));
}
std::expected<std::vector<value_variant>, std::string> object::select_one(database& db, const std::string_view statement, const int value_count, const value_binder& binder, const std::vector<value_variant>& column_value_variants) {
	auto mapper = value_mapper{.binder = binder};
	std::expected<std::vector<value_variant>, std::string> return_value{};
	auto execute_result = prepare_bind_execute_statement(static_cast<sqlite3*>(db._db_handle), statement, value_count, mapper, [&](void* opaque_sqlite_statement) -> bool {
		auto* sqlite_statement = static_cast<sqlite3_stmt*>(opaque_sqlite_statement);
		if (static_cast<size_t>(sqlite3_column_count(sqlite_statement)) != column_value_variants.size()) {
			return_value = std::unexpected{"Select result contains different number of columns than expected"};
		} else {
			for (int i = 0; i < sqlite3_column_count(sqlite_statement); ++i) {
				if (sqlite3_column_type(sqlite_statement, i) == SQLITE_NULL) {
					return_value.value().emplace_back(std::monostate{});
				} else {
					std::visit(overloaded{
						[](std::monostate) { throw std::runtime_error("Column value variant cannot be monostate"); },
						[&](const int32_t) {
							return_value.value().emplace_back(sqlite3_column_int(sqlite_statement, i));
						},
						[&](const int64_t) {
							return_value.value().emplace_back(sqlite3_column_int64(sqlite_statement, i));
						},
						[&](const std::vector<uint8_t>&) {
							auto* begin = static_cast<const uint8_t*>(sqlite3_column_blob(sqlite_statement, i));
							auto* end = begin + sqlite3_column_bytes(sqlite_statement, i);
							return_value.value().emplace_back(std::vector<uint8_t>{begin, end});
						},
					}, column_value_variants[i]);
				}
			}
		}
		return false; // Stop after one row
	});
	if (not mapper.bind_result) {
		return std::unexpected{std::move(mapper.bind_result.error())};
	}
	if (not execute_result) {
		return std::unexpected{std::move(execute_result.error())};
	}
	return return_value;
}

database::database(void* db_handle) : _db_handle(db_handle) {}

std::expected<database, std::string> database::open(const char* filepath) {
	sqlite3* db{};
	if (const auto result = sqlite3_open_v2(filepath, &db, SQLITE_OPEN_READWRITE, nullptr); result == SQLITE_OK) {
		if (const auto is_read_only = sqlite3_db_readonly(db, "main"); is_read_only == 0) {
			return database{db};
		}
		return std::unexpected("Insufficient permissions, database is read-only.");
	} else {
		return std::unexpected(std::string{sqlite3_errstr(result)});
	}
}
std::expected<database, std::string> database::open_or_create(const char* filepath) {
	sqlite3* db{};
	if (const auto result = sqlite3_open_v2(filepath, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr); result == SQLITE_OK) {
		if (const auto is_read_only = sqlite3_db_readonly(db, "main"); is_read_only == 0) {
			return database{db};
		}
		return std::unexpected("Insufficient permissions, database is read-only.");
	} else {
		return std::unexpected(std::string{sqlite3_errstr(result)});
	}
}
database::database(database&& other) noexcept : _db_handle(other._db_handle) {
	other._db_handle = nullptr;
}
database& database::operator=(database&& other) noexcept {
	std::swap(_db_handle, other._db_handle);
	return *this;
}
database::~database() {
	if (_db_handle) {
		sqlite3_close(static_cast<sqlite3*>(_db_handle));
		_db_handle = nullptr;
	}
}
