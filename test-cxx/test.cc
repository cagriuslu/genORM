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

TEST(genORM, find_by_rowid) {
	std::filesystem::remove("test.db");
	auto db = genORM::database::open_or_create("test.db");
	const auto objA = testproj::MyObject::create(*db, 15, std::nullopt, {1, 2, 3});
	EXPECT_TRUE(objA);
	const auto objB = testproj::MyObject::find_by_rowid(*db, objA.value().get_rowid());
	EXPECT_TRUE(objB);
	EXPECT_EQ(objA->get_i(), objB->get_i());
	EXPECT_EQ(objA->get_io(), objB->get_io());
	EXPECT_EQ(objA->get_ba(), objB->get_ba());
}
