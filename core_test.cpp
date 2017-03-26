//
// Created by AIdancer on 16/10/4.
//
#include "base_data.h"
#include "gtest/gtest.h"
#include "rapidjson/document.h"

#include <iostream>
#include <string>
#include <cassert>

using namespace std;

char const* greet() {
	return "Hello, World.\n";
}

TEST(TestHello, Hello) {
	string str = "hello";
	EXPECT_EQ(str, "hello");
}

TEST(RapidJsonTest, TestDocument) {
	using namespace rapidjson;
	Document document;
}


