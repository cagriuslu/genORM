#pragma once
#include <functional>
#include <expected>
#include <string>
#include <variant>

namespace genORM {
	class database;

	class object {
		database& _db;
		uint64_t _id;

	protected:
		explicit object(database& db, const uint64_t id) : _db(db), _id(id) {}

		static std::expected<void, std::string> create_table_if_not_exists(database&, std::string_view statement);

		static std::expected<void, std::string> create_index_if_not_exists(database&, std::string_view statement);

		using value_variant = std::variant<std::monostate, int32_t, int64_t, std::vector<uint8_t>>;
		using value_binder = std::function<value_variant(int value_index)>;
		static std::expected<uint64_t, std::string> insert_into_table(database&, std::string_view statement, int value_count, const value_binder& binder);
	};

	class database final {
		friend object;
		void* _db_handle;

		explicit database(void*);

	public:
		static std::expected<database, std::string> open(const char* filepath);
		static std::expected<database, std::string> open_or_create(const char* filepath);
		database(const database& other) = delete;
		database& operator=(const database& other) = delete;
		database(database&& other) noexcept;
		database& operator=(database&& other) noexcept;
		~database();
	};
}
