#include <genORM/genORM.h>
#include "TestProj.orm.h"
#include <gtest/gtest.h>
#include <filesystem>

TEST(genORM, open_or_create) {
	{
		std::filesystem::remove("test.db");
		auto db = genORM::database::open("test.db");
		EXPECT_FALSE(db);
	}

	{
		auto db = genORM::database::open_or_create("test.db");
		EXPECT_TRUE(db);
		auto obj = testproj::MyObject::create(*db, 15, std::nullopt, {1, 2, 3});
	}

	{
		auto db = genORM::database::open("test.db");
		EXPECT_TRUE(db);
	}
}

TEST(genORM, execute_transaction_basic) {
	std::filesystem::remove("test.db");
	auto db = genORM::database::open_or_create("test.db");
	const auto transaction_result = db->execute_transaction<testproj::MyObject>([](genORM::database& db_) {
		return testproj::MyObject::create(db_, 15, 16, {1, 2, 3});
	});
	EXPECT_EQ(transaction_result->get_i(), 15);
	EXPECT_EQ(transaction_result->get_io(), 16);
	EXPECT_EQ(transaction_result->get_ba(), std::vector<uint8_t>({1, 2, 3}));
}

TEST(genORM, execute_transaction_complex) {
	std::filesystem::remove("test.db");
	auto db_1 = genORM::database::open_or_create("test.db");
	auto db_2 = genORM::database::open("test.db");
	const auto transaction_result = db_1->execute_transaction<testproj::MyObject>([&](genORM::database& db_) {
		// Upgrade transaction to a write transaction
		auto obj_1 = testproj::MyObject::create(db_, 15, 16, {1, 2, 3});
		// Try to access the db from another connection
		const auto attempt = testproj::MyObject::create(*db_2, 12, 13, {4, 5, 6});
		EXPECT_FALSE(attempt);
		return obj_1;
	});

	const auto id_1 = testproj::MyObject::find_by_rowid(*db_2, 1);
	EXPECT_EQ((*id_1)->get_i(), 15);
	const auto id_2 = testproj::MyObject::find_by_rowid(*db_2, 2);
	EXPECT_TRUE(id_2); // No errors occurred
	EXPECT_FALSE(*id_2); // But no result found
}

TEST(genORM, find_by_rowid) {
	std::filesystem::remove("test.db");
	auto db = genORM::database::open_or_create("test.db");
	const auto objA = testproj::MyObject::create(*db, 15, std::nullopt, {1, 2, 3});
	EXPECT_TRUE(objA);
	const auto objB = testproj::MyObject::find_by_rowid(*db, objA.value().get_rowid());
	EXPECT_TRUE(objB);
	EXPECT_TRUE(*objB);
	EXPECT_EQ(objA->get_i(), (*objB)->get_i());
	EXPECT_EQ(objA->get_io(), (*objB)->get_io());
	EXPECT_EQ(objA->get_ba(), (*objB)->get_ba());
}

TEST(genORM, find_first) {
	std::filesystem::remove("test.db");
	auto db = genORM::database::open_or_create("test.db");
	const auto objA = testproj::MyObject::create(*db, 15, std::nullopt, {1, 2, 3});
	EXPECT_TRUE(objA);
	const auto objB = testproj::MyObject::find_first_by_i(*db, 15);
	EXPECT_TRUE(objB);
	EXPECT_TRUE(*objB);
	EXPECT_EQ(objA->get_i(), (*objB)->get_i());
	EXPECT_EQ(objA->get_io(), (*objB)->get_io());
	EXPECT_EQ(objA->get_ba(), (*objB)->get_ba());
	const auto objC = testproj::MyObject::find_first_by_i(*db, 14);
	EXPECT_TRUE(objC);
	EXPECT_FALSE(*objC);
}

TEST(genORM, find_all) {
	std::filesystem::remove("test.db");
	auto db = genORM::database::open_or_create("test.db");
	const auto obj1 = testproj::MyObject::create(*db, 15, std::nullopt, {1, 2, 3});
	const auto obj2 = testproj::MyObject::create(*db, 15, 16, {1, 2, 3});
	const auto obj3 = testproj::MyObject::create(*db, 15, 200, {4, 5, 6});
	const auto obj4 = testproj::MyObject::create(*db, 15, 500, {});

	const auto none = testproj::MyObject::find_all_by_i(*db, 16);
	EXPECT_TRUE(none);
	EXPECT_TRUE(none->empty());

	const auto all = testproj::MyObject::find_all_by_i(*db, 15);
	EXPECT_TRUE(all);
	EXPECT_EQ(all->size(), 4);
	EXPECT_EQ((*all)[0].get_i(), 15);
	EXPECT_EQ((*all)[0].get_io(), obj1->get_io());
	EXPECT_EQ((*all)[1].get_i(), 15);
	EXPECT_EQ((*all)[1].get_io(), obj2->get_io());
	EXPECT_EQ((*all)[2].get_i(), 15);
	EXPECT_EQ((*all)[2].get_io(), obj3->get_io());
	EXPECT_EQ((*all)[3].get_i(), 15);
	EXPECT_EQ((*all)[3].get_io(), obj4->get_io());
}
