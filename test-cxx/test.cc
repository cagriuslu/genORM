#include <genORM/genORM.h>
#include "TestProj.orm.h"
#include <gtest/gtest.h>
#include <filesystem>

TEST(Dummy, dummy) {
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
