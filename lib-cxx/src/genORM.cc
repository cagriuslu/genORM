#include <genORM/genORM.h>
#include <sqlite3.h>

namespace {
	template<class... Ts>
	struct overloaded : Ts... { using Ts::operator()...; };
}

using namespace genORM;

std::expected<void,std::string> object::create_table_if_not_exists(database& db, const std::string_view statement) {
	return db.prepare_bind_execute_statement(statement);
}

std::expected<uint64_t, std::string> object::insert_into_table(database& db, const std::string_view statement, const int value_count, const value_binder& binder) {
	std::expected<void, std::string> bind_result;
	auto execute_result = db.prepare_bind_execute_statement(statement, value_count, [&](void* opaque_sqlite_statement, const int value_index) {
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
			[&](const std::vector<uint8_t>& bytes) {
				if (const auto result = sqlite3_bind_blob64(sqlite_statement, value_index, bytes.data(), bytes.size(), SQLITE_TRANSIENT); result != SQLITE_OK) {
					bind_result = std::unexpected(std::string{sqlite3_errstr(result)});
				}
			}
		}, binder(value_index));
	});

	if (not bind_result) {
		return std::unexpected{std::move(bind_result.error())};
	}
	if (not execute_result) {
		return std::unexpected{std::move(execute_result.error())};
	}
	return db.last_insert_rowid();
}

database::database(void* db_handle) : _db_handle(db_handle) {}

std::expected<database, std::string> database::open(const char* filepath) {
	sqlite3* db{};
	if (const auto result = sqlite3_open_v2(filepath, &db, SQLITE_OPEN_READWRITE, nullptr); result == SQLITE_OK) {
		if (const auto is_read_only = sqlite3_db_readonly(db, "main"); is_read_only == 0) {
			return database{db};
		} else {
			return std::unexpected("Insufficient permissions, database is read-only.");
		}
	} else {
		return std::unexpected(std::string{sqlite3_errstr(result)});
	}
}
std::expected<database, std::string> database::open_or_create(const char* filepath) {
	sqlite3* db{};
	if (const auto result = sqlite3_open_v2(filepath, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr); result == SQLITE_OK) {
		if (const auto is_read_only = sqlite3_db_readonly(db, "main"); is_read_only == 0) {
			return database{db};
		} else {
			return std::unexpected("Insufficient permissions, database is read-only.");
		}
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

std::expected<void, std::string> database::prepare_bind_execute_statement(const std::string_view statement, const int value_count, const std::function<void(void* sqlite_statement, int value_index)>& binder, const std::function<bool()>& op) {
	sqlite3_stmt* sqlite_statement{};
	if (const auto prepare_result = sqlite3_prepare_v2(static_cast<sqlite3*>(_db_handle), statement.data(), static_cast<int>(statement.size()), &sqlite_statement, nullptr); prepare_result != SQLITE_OK) {
		return std::unexpected(std::string{sqlite3_errstr(prepare_result)});
	}

	for (int i = 1; i <= value_count; ++i) {
		binder(sqlite_statement, i);
	}

	int32_t step_result;
	while ((step_result = sqlite3_step(sqlite_statement)) == SQLITE_ROW) {
		if (not op()) { break; }
	}
	sqlite3_finalize(sqlite_statement);
	if (step_result != SQLITE_DONE && step_result != SQLITE_ROW) {
		return std::unexpected(std::string{sqlite3_errstr(step_result)});
	}
	return {};
}
void database::binder_none(void*, int) {}
bool database::op_ignore() { return false; }

uint64_t database::last_insert_rowid() {
	return static_cast<uint64_t>(sqlite3_last_insert_rowid(static_cast<sqlite3*>(_db_handle)));
}
