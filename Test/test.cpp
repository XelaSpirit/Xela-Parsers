#include "pch.h"

#include <filesystem>

#define XELA_JSON_IMPLEMENTATION
#include "XelaJson.hpp"

#define XELA_XML_IMPLEMENTATION
#include "XelaXml.hpp";

TEST(Json, String) {
	std::string str = "\"Hello\"";
	std::string expect = "Hello";

	Xela::Json *val = Xela::Json::fromString(str);

	ASSERT_NE(val, nullptr);
	ASSERT_TRUE(val->valid());
	ASSERT_EQ(val->type(), Xela::Json::Type::String);

	EXPECT_EQ(val->asString(), expect);
	EXPECT_EQ((std::string)*val, expect);
	EXPECT_EQ(val->size(), expect.size());
}
TEST(Json, Integer) {
	std::string strInt = "01234";
	std::string strPos = "+65";
	std::string strNeg = "-52";
	std::string strExp = "5e2";

	long long Int = 1234;
	long long Pos = +65;
	long long Neg = -52;
	long long Exp = (long long)5e2;

	Xela::Json *valInt = Xela::Json::fromString(strInt);
	Xela::Json *valPos = Xela::Json::fromString(strPos);
	Xela::Json *valNeg = Xela::Json::fromString(strNeg);
	Xela::Json *valExp = Xela::Json::fromString(strExp);

	EXPECT_NE(valInt, nullptr);
	if (valInt != nullptr) {
		EXPECT_TRUE(valInt->valid());
		EXPECT_EQ(valInt->type(), Xela::Json::Type::Integer);

		if (valInt->type() == Xela::Json::Type::Integer) {
			EXPECT_EQ(valInt->asInt(), Int);
			EXPECT_EQ((long long)*valInt, Int);
			EXPECT_EQ(valInt->size(), 1);
		}
	}
	EXPECT_NE(valPos, nullptr);
	if (valPos != nullptr) {
		EXPECT_TRUE(valPos->valid());
		EXPECT_EQ(valPos->type(), Xela::Json::Type::Integer);

		if (valPos->type() == Xela::Json::Type::Integer) {
			EXPECT_EQ(valPos->asInt(), Pos);
			EXPECT_EQ((long long)*valPos, Pos);
			EXPECT_EQ(valPos->size(), 1);
		}
	}
	EXPECT_NE(valNeg, nullptr);
	if (valNeg != nullptr) {
		EXPECT_TRUE(valNeg->valid());
		EXPECT_EQ(valNeg->type(), Xela::Json::Type::Integer);

		if (valNeg->type() == Xela::Json::Type::Integer) {
			EXPECT_EQ(valNeg->asInt(), Neg);
			EXPECT_EQ((long long)*valNeg, Neg);
			EXPECT_EQ(valNeg->size(), 1);
		}
	}
	EXPECT_NE(valExp, nullptr);
	if (valExp != nullptr) {
		EXPECT_TRUE(valExp->valid());
		EXPECT_EQ(valExp->type(), Xela::Json::Type::Integer);

		if (valExp->type() == Xela::Json::Type::Integer) {
			EXPECT_EQ(valExp->asInt(), Exp);
			EXPECT_EQ((long long)*valExp, Exp);
			EXPECT_EQ(valExp->size(), 1);
		}
	}
}
TEST(Json, Float) {
	std::string strFloat = "15.185";
	std::string strPos = "+2.76";
	std::string strNeg = "-3.45";
	std::string strExp = "1.34e-1";

	float Float = 15.185f;
	float Pos = +2.76f;
	float Neg = -3.45f;
	float Exp = 1.34e-1f;

	Xela::Json *valFloat = Xela::Json::fromString(strFloat);
	Xela::Json *valPos = Xela::Json::fromString(strPos);
	Xela::Json *valNeg = Xela::Json::fromString(strNeg);
	Xela::Json *valExp = Xela::Json::fromString(strExp);

	EXPECT_NE(valFloat, nullptr);
	if (valFloat != nullptr) {
		EXPECT_TRUE(valFloat->valid());
		EXPECT_EQ(valFloat->type(), Xela::Json::Type::Float);

		if (valFloat->type() == Xela::Json::Type::Float) {
			EXPECT_EQ(valFloat->asFloat(), Float);
			EXPECT_EQ((float)*valFloat, Float);
			EXPECT_EQ(valFloat->size(), 1);
		}
	}
	EXPECT_NE(valPos, nullptr);
	if (valPos != nullptr) {
		EXPECT_TRUE(valPos->valid());
		EXPECT_EQ(valPos->type(), Xela::Json::Type::Float);

		if (valPos->type() == Xela::Json::Type::Float) {
			EXPECT_EQ(valPos->asFloat(), Pos);
			EXPECT_EQ((float)*valPos, Pos);
			EXPECT_EQ(valPos->size(), 1);
		}
	}
	EXPECT_NE(valNeg, nullptr);
	if (valNeg != nullptr) {
		EXPECT_TRUE(valNeg->valid());
		EXPECT_EQ(valNeg->type(), Xela::Json::Type::Float);

		if (valNeg->type() == Xela::Json::Type::Float) {
			EXPECT_EQ(valNeg->asFloat(), Neg);
			EXPECT_EQ((float)*valNeg, Neg);
			EXPECT_EQ(valNeg->size(), 1);
		}
	}
	EXPECT_NE(valExp, nullptr);
	if (valExp != nullptr) {
		EXPECT_TRUE(valExp->valid());
		EXPECT_EQ(valExp->type(), Xela::Json::Type::Float);

		if (valExp->type() == Xela::Json::Type::Float) {
			EXPECT_EQ(valExp->asFloat(), Exp);
			EXPECT_EQ((float)*valExp, Exp);
			EXPECT_EQ(valExp->size(), 1);
		}
	}
}
TEST(Json, Hex) {
	std::string strHex = "0x1F";
	std::string strPos = "+0x2C";
	std::string strNeg = "-0xe";
	std::string strExp = "0x1Dp-3";

	long long Hex = 0x1F;
	long long Pos = +0x2C;
	long long Neg = -0xe;
	float Exp = 0x1Dp-3f;

	Xela::Json *valHex = Xela::Json::fromString(strHex);
	Xela::Json *valPos = Xela::Json::fromString(strPos);
	Xela::Json *valNeg = Xela::Json::fromString(strNeg);
	Xela::Json *valExp = Xela::Json::fromString(strExp);

	EXPECT_NE(valHex, nullptr);
	if (valHex != nullptr) {
		EXPECT_TRUE(valHex->valid());
		EXPECT_EQ(valHex->type(), Xela::Json::Type::Integer);

		if (valHex->type() == Xela::Json::Type::Integer) {
			EXPECT_EQ(valHex->asInt(), Hex);
			EXPECT_EQ((long long)*valHex, Hex);
			EXPECT_EQ(valHex->size(), 1);
		}
	}
	EXPECT_NE(valPos, nullptr);
	if (valPos != nullptr) {
		EXPECT_TRUE(valPos->valid());
		EXPECT_EQ(valPos->type(), Xela::Json::Type::Integer);

		if (valPos->type() == Xela::Json::Type::Integer) {
			EXPECT_EQ(valPos->asInt(), Pos);
			EXPECT_EQ((long long)*valPos, Pos);
			EXPECT_EQ(valPos->size(), 1);
		}
	}
	EXPECT_NE(valNeg, nullptr);
	if (valNeg != nullptr) {
		EXPECT_TRUE(valNeg->valid());
		EXPECT_EQ(valNeg->type(), Xela::Json::Type::Integer);

		if (valNeg->type() == Xela::Json::Type::Integer) {
			EXPECT_EQ(valNeg->asInt(), Neg);
			EXPECT_EQ((long long)*valNeg, Neg);
			EXPECT_EQ(valNeg->size(), 1);
		}
	}
	EXPECT_NE(valExp, nullptr);
	if (valExp != nullptr) {
		EXPECT_TRUE(valExp->valid());
		EXPECT_EQ(valExp->type(), Xela::Json::Type::Float);

		if (valExp->type() == Xela::Json::Type::Float) {
			EXPECT_EQ(valExp->asFloat(), Exp);
			EXPECT_EQ((float)*valExp, Exp);
			EXPECT_EQ(valExp->size(), 1);
		}
	}
}
TEST(Json, Bool) {
	std::string strTrue = "TruE";
	std::string strFalse = "fAlse";

	Xela::Json *valTrue = Xela::Json::fromString(strTrue);
	Xela::Json *valFalse = Xela::Json::fromString(strFalse);

	EXPECT_NE(valTrue, nullptr);
	if (valTrue != nullptr) {
		EXPECT_TRUE(valTrue->valid());
		EXPECT_EQ(valTrue->type(), Xela::Json::Type::Bool);

		if (valTrue->type() == Xela::Json::Type::Bool) {
			EXPECT_EQ(valTrue->asBool(), true);
			EXPECT_EQ((bool)*valTrue, true);
			EXPECT_EQ(valTrue->size(), 1);
		}
	}
	EXPECT_NE(valFalse, nullptr);
	if (valFalse != nullptr) {
		EXPECT_TRUE(valFalse->valid());
		EXPECT_EQ(valFalse->type(), Xela::Json::Type::Bool);

		if (valFalse->type() == Xela::Json::Type::Bool) {
			EXPECT_EQ(valFalse->asBool(), false);
			EXPECT_EQ((bool)*valFalse, false);
			EXPECT_EQ(valFalse->size(), 1);
		}
	}
}
TEST(Json, Null) {
	std::string str = "Null";

	Xela::Json *val = Xela::Json::fromString(str);

	ASSERT_NE(val, nullptr);
	ASSERT_FALSE(val->valid());
	ASSERT_EQ(val->type(), Xela::Json::Type::Null);
}
TEST(Json, Array) {
	std::string String = "Hello";
	long long Int = 46;
	float Float = 2.5f;
	long long Hex = 0xA;
	bool Bool = true;

	std::string str =
		"[\n"
			"\"" + String + "\",\n"
			+ std::to_string(Int) + ",\n"
			+ std::to_string(Float) + ",\n"
			+ std::to_string(Hex) + ",\n"
			/* to_string(Bool) would return 1 so I hard-coded this */ "True,\n"
			"[\n"
				+ std::to_string(Int) + "\n"
			"]\n"
		"]";

	Xela::Json *val = Xela::Json::fromString(str);

	ASSERT_NE(val, nullptr);
	ASSERT_TRUE(val->valid());
	ASSERT_EQ(val->type(), Xela::Json::Type::Array);

	Xela::Json::Array &vec = val->asArray();

	ASSERT_EQ(vec.size(), 6);
	EXPECT_EQ(val->size(), vec.size());

	EXPECT_EQ(vec[0]->asString(), String);
	EXPECT_EQ(vec[1]->asInt(), Int);
	EXPECT_EQ(vec[2]->asFloat(), Float);
	EXPECT_EQ(vec[3]->asInt(), Hex);
	EXPECT_EQ(vec[4]->asBool(), Bool);

	ASSERT_EQ(vec[5]->type(), Xela::Json::Type::Array);
	ASSERT_EQ(vec[5]->asArray().size(), 1);
	EXPECT_EQ(vec[5]->asArray()[0]->asInt(), Int);
}
TEST(Json, Object) {
	std::string str =
		"{"
			"\"One\":\"OneVal\",\n"
			"\"Two\" : 2"
		"}";

	Xela::Json *val = Xela::Json::fromString(str);

	ASSERT_NE(val, nullptr);
	ASSERT_TRUE(val->valid());
	ASSERT_EQ(val->type(), Xela::Json::Type::Object);

	Xela::Json::Object &map = val->asObject();

	ASSERT_EQ(map.size(), 2);
	EXPECT_EQ(val->size(), map.size());

	ASSERT_NE(map.find("One"), map.end());
	ASSERT_NE(map.find("Two"), map.end());
	EXPECT_EQ(map.find("one"), map.end());

	EXPECT_EQ(map.find("One")->second->asString(), "OneVal");
	EXPECT_EQ(map.find("Two")->second->asInt(), 2);
}
TEST(Json, Comments) {
	std::string str =
		"{"
			"\"One\": \"Hello\",\n"
			"//This is a comment\n"
			"//This is a second comment\n"
			"\"Two\": 2"
		"}";

	Xela::Json *val = Xela::Json::fromString(str);

	ASSERT_NE(val, nullptr);
	ASSERT_TRUE(val->valid());
	ASSERT_EQ(val->size(), 2);
}
TEST(Json, File) {
	std::filesystem::path file = std::filesystem::current_path() / "in.jx";

	Xela::Json *val = Xela::Json::fromFile(file);

	ASSERT_NE(val, nullptr);
	ASSERT_TRUE(val->valid());
	ASSERT_EQ(val->type(), Xela::Json::Type::Object);

	auto &map = val->asObject();
	auto itOne = map.find("One");

	EXPECT_NE(map.find("One"), map.end());
	EXPECT_EQ(map.find("One")->second->type(), Xela::Json::Type::String);
	EXPECT_EQ(map.find("One")->second->asString(), std::string("hi"));

	EXPECT_NE(map.find("Two"), map.end());
	EXPECT_EQ(map.find("Two")->second->type(), Xela::Json::Type::Integer);
	EXPECT_EQ(map.find("Two")->second->asInt(), 2);
}
TEST(Json, Write) {
	std::string str = "{ \"one\": [ 1, 2, 3, 4 ], \"two\": 2 }";
	Xela::Json *val = Xela::Json::fromString(str);

	std::stringstream stream = std::stringstream();
	val->write(false, stream);

	ASSERT_EQ(stream.str(), std::string("{\"one\":[1,2,3,4],\"two\":2}"));
}
TEST(Json, Empty) {
	Xela::Json *obj = Xela::Json::fromType(Xela::Json::Type::Object);
	Xela::Json *arr = Xela::Json::fromType(Xela::Json::Type::Array);
	Xela::Json *str = Xela::Json::fromType(Xela::Json::Type::String);
	Xela::Json *num = Xela::Json::fromType(Xela::Json::Type::Integer);
	Xela::Json *dec = Xela::Json::fromType(Xela::Json::Type::Float);
	Xela::Json *bin = Xela::Json::fromType(Xela::Json::Type::Bool);
	Xela::Json *nul = Xela::Json::fromType(Xela::Json::Type::Null);

	ASSERT_NE(obj, nullptr);
	ASSERT_NE(arr, nullptr);
	ASSERT_NE(str, nullptr);
	ASSERT_NE(num, nullptr);
	ASSERT_NE(dec, nullptr);
	ASSERT_NE(bin, nullptr);
	ASSERT_NE(nul, nullptr);

	ASSERT_TRUE(obj->valid());
	ASSERT_TRUE(arr->valid());
	ASSERT_TRUE(str->valid());
	ASSERT_TRUE(num->valid());
	ASSERT_TRUE(dec->valid());
	ASSERT_TRUE(bin->valid());
	ASSERT_FALSE(nul->valid());

	ASSERT_EQ(obj->type(), Xela::Json::Type::Object);
	ASSERT_EQ(arr->type(), Xela::Json::Type::Array);
	ASSERT_EQ(str->type(), Xela::Json::Type::String);
	ASSERT_EQ(num->type(), Xela::Json::Type::Integer);
	ASSERT_EQ(dec->type(), Xela::Json::Type::Float);
	ASSERT_EQ(bin->type(), Xela::Json::Type::Bool);
	ASSERT_EQ(nul->type(), Xela::Json::Type::Null);

	ASSERT_EQ(obj->asObject().size(), 0);
	ASSERT_EQ(arr->asArray().size(), 0);
	ASSERT_EQ(str->asString().size(), 0);
	ASSERT_EQ(num->asInt(), 0);
	ASSERT_EQ(dec->asFloat(), 0.0f);
	ASSERT_EQ(bin->asBool(), false);
}

// TODO - Test XML read/write
TEST(Xml, Root) {
	// xml
	std::string str = "<xml></xml>";

	Xela::Xml *val = Xela::Xml::fromString(str);

	ASSERT_NE(val, nullptr);
	
	// Root
	ASSERT_EQ(val->getType(), "xml");
	ASSERT_FALSE(val->isComment());
	ASSERT_EQ(val->getAttributes().size(), 0);
	ASSERT_EQ(val->getChildren().size(), 0);
}
TEST(Xml, Children) {
	// xml
	// - a1
	//   - b1
	// - a2
	std::string str = "<xml><a1><b1></b1></a1><a2></a2></xml>";

	Xela::Xml *val = Xela::Xml::fromString(str);

	ASSERT_NE(val, nullptr);

	// Root
	ASSERT_EQ(val->getType(), "xml");
	ASSERT_FALSE(val->isComment());
	ASSERT_EQ(val->getAttributes().size(), 0);
	ASSERT_EQ(val->getChildren().size(), 2);
	ASSERT_NE(val->getChildren().find("a1"), val->getChildren().end());
	ASSERT_NE(val->getChildren().find("a2"), val->getChildren().end());

	Xela::Xml *a1 = val->getChildren().find("a1")->second[0];
	Xela::Xml *a2 = val->getChildren().find("a2")->second[0];;

	ASSERT_NE(a1, nullptr);
	ASSERT_NE(a2, nullptr);

	// a1
	ASSERT_EQ(a1->getType(), "a1");
	ASSERT_FALSE(a1->isComment());
	ASSERT_EQ(a1->getAttributes().size(), 0);
	ASSERT_EQ(a1->getChildren().size(), 1);
	ASSERT_NE(a1->getChildren().find("b1"), a1->getChildren().end());

	// a2
	ASSERT_EQ(a2->getType(), "a2");
	ASSERT_FALSE(a2->isComment());
	ASSERT_EQ(a2->getAttributes().size(), 0);
	ASSERT_EQ(a2->getChildren().size(), 0);

	Xela::Xml *b1 = a1->getChildren().find("b1")->second[0];

	ASSERT_NE(b1, nullptr);
	
	// b1
	ASSERT_EQ(b1->getType(), "b1");
	ASSERT_FALSE(b1->isComment());
	ASSERT_EQ(b1->getAttributes().size(), 0);
	ASSERT_EQ(b1->getChildren().size(), 0);
}
TEST(Xml, Comment) {
	// xml
	// - child
	// - comment
	std::string str = "<xml><child></child><!-- comment --></xml>";

	Xela::Xml *val = Xela::Xml::fromString(str);

	ASSERT_NE(val, nullptr);

	// Root
	ASSERT_EQ(val->getType(), "xml");
	ASSERT_FALSE(val->isComment());
	ASSERT_EQ(val->getAttributes().size(), 0);
	ASSERT_EQ(val->getChildren().size(), 2);
	ASSERT_NE(val->getChildren().find("child"), val->getChildren().end());
	ASSERT_NE(val->getChildren().find(""), val->getChildren().end());

	Xela::Xml *child = val->getChildren().find("child")->second[0];
	Xela::Xml *comment = val->getChildren().find("")->second[0];

	ASSERT_NE(child, nullptr);
	ASSERT_NE(comment, nullptr);

	// Child
	ASSERT_EQ(child->getType(), "child");
	ASSERT_FALSE(child->isComment());
	ASSERT_EQ(child->getAttributes().size(), 0);
	ASSERT_EQ(child->getChildren().size(), 0);

	// Comment
	ASSERT_EQ(comment->getComment(), " comment ");
	ASSERT_TRUE(comment->isComment());
	ASSERT_EQ(comment->getAttributes().size(), 0);
	ASSERT_EQ(comment->getChildren().size(), 0);
}