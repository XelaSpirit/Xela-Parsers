// Xela Json
// 
// Author: Alex Morse
// 
// TODO - add description here
// 
// Limitations:
//		String parser does not support the following escape sequences:
//			Octal values, hex values, and unicode values
// 
// This software is dual-licensed to the public domain and under the following
// license: you are granted a perpetual, irrevocable license to copy, modify,
// publish, and distribute this file as you see fit.

#ifndef _XELA_JSON_HPP
#define _XELA_JSON_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#define _XELA_JSON_START namespace Xela {  extern "C" {
#define _XELA_JSON_END } }

_XELA_JSON_START // C style structs and functions

class json_type_error : public std::runtime_error {
public:
	json_type_error(const std::string &str) : runtime_error(str) {}
	json_type_error(const char *str) : runtime_error(str) {}
};
class json_key_error : public std::runtime_error {
public:
	json_key_error(const std::string &str) : runtime_error(str) {}
	json_key_error(const char *str) : runtime_error(str) {}
};
class json_null_error : public std::runtime_error {
public:
	json_null_error(const std::string &str) : runtime_error(str) {}
	json_null_error(const char *str) : runtime_error(str) {}
};
class json_parse_error : public std::runtime_error {
public:
	json_parse_error(const std::string &str) : runtime_error(str) {}
	json_parse_error(const char *str) : runtime_error(str) {}
};
class json_file_error : public std::runtime_error {
public:
	json_file_error(const std::string &str) : runtime_error(str) {}
	json_file_error(const char *str) : runtime_error(str) {}
};

struct Json {
public:
	using Object = std::unordered_map<std::string, Json *>;
	using Array = std::vector<Json *>;
	enum class Type {
		Object, Array, String, Integer, Float, Bool, Null
	};

private:
	union {
		Object *map;
		Array *arr;
		std::string *str;
		long long *i;
		float *f;
		bool *b;
		void *ptr = nullptr;
	};
	Type dataType = Type::Null;

	void initMap();
	void initArray();
	void initString();
	void initInt();
	void initFloat();
	void initBool();

	void delData();

	static bool isNumeric(char c);
	static bool isEndOfValue(char c);

	static void consumeWhitespace(std::istream &in, size_t &line, size_t &col);
	static void consumeComment(std::istream &in, size_t &line, size_t &col);

	static char getEscapeCharacter(std::istream &in, size_t &line, size_t &col);
	static std::string readString(std::istream &in, size_t &line, size_t &col);

	static Json *parseObject(std::istream &in, size_t &line, size_t &col);
	static Json *parseArray(std::istream &in, size_t &line, size_t &col);
	static Json *parseString(std::istream &in, size_t &line, size_t &col);
	static Json *parseNumber(std::istream &in, size_t &line, size_t &col);
	static Json *parseKeyword(std::istream &in, size_t &line, size_t &col);

	static Json *parseValue(std::istream &in, size_t &line, size_t &col);

	static void writeTab(std::ostream &out, size_t indent);

	static void writeObject(Json *val, std::ostream &out, size_t indent, bool pretty);
	static void writeArray(Json *val, std::ostream &out, size_t indent, bool pretty);
	static void writeString(Json *val, std::ostream &out, size_t indent, bool pretty);
	static void writeInteger(Json *val, std::ostream &out, size_t indent, bool pretty);
	static void writeFloat(Json *val, std::ostream &out, size_t indent, bool pretty);
	static void writeBool(Json *val, std::ostream &out, size_t indent, bool pretty);
	static void writeNull(Json *val, std::ostream &out, size_t indent, bool pretty);

	static void writeValue(Json *val, std::ostream &out, size_t indent, bool pretty);

public:
	Json();
	~Json();

	static Json *fromStream(std::istream &in);
	static Json *fromFile(std::filesystem::path file);
	static Json *fromString(std::string &str);
	static Json *fromType(Type type);

	void write(bool pretty = false, std::ostream &out = std::cout);

	Object &asObject();
	Array &asArray();
	std::string &asString();
	long long &asInt();
	float &asFloat();
	bool &asBool();

	explicit operator Object &();
	explicit operator Array &();
	explicit operator std::string &();
	explicit operator long long &();
	explicit operator float &();
	explicit operator bool &();

	const Json &operator()(std::string &key);
	const Json &operator[](size_t idx);

	Object::iterator find(std::string &key);
	const Json &at(size_t idx);

	inline bool valid();
	inline Type type();
	inline size_t size();
};
_XELA_JSON_END

#endif

// #define XELA_JSON_IMPLEMENTATION
#ifdef XELA_JSON_IMPLEMENTATION

#define JSON_ERR(line, col) std::string("Json [" + std::to_string(line) + ", " + std::to_string(col) + "]: ") +

_XELA_JSON_START //C style structs and functions

// Parsing utilities
bool Json::isNumeric(char c) {
	return (c >= '0' && c <= '9') || c == '-' || c == '+' || c == 'e' || c == 'E' || c == 'x' || c == 'X' || c == 'p' || c == 'P' || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || c == '.';
}
bool Json::isEndOfValue(char c) {
	return c == '}' || c == ']' || c == ',' || c == ':' || c == EOF || std::isspace(c) || c == '/';
}

void Json::consumeWhitespace(std::istream &in, size_t &line, size_t &col) {
	while (std::isspace(in.peek())) {
		char c = in.get();
		col++;
		if (c == '\n') {
			line++;
			col = 0;
		}
	}
	if (in.peek() == '/') {
		if ((in.get(), in.peek()) == '/') {
			consumeComment(in, line, col);
			consumeWhitespace(in, line, col);
		}
		else {
			in.unget();
			col--;
		}
	}
}
void Json::consumeComment(std::istream &in, size_t &line, size_t &col) {
	while (in.get() != '\n') { col++; }
	line++;
}

char Json::getEscapeCharacter(std::istream &in, size_t &line, size_t &col) {
	char c = in.get();
	col++;

	switch (c) {
	case '\'':
		return '\'';
		break;
	case '"':
		return '\"';
		break;
	case '?':
		return '\?';
		break;
	case '\\':
		return '\\';
		break;
	case 'a':
		return '\a';
		break;
	case 'b':
		return '\b';
		break;
	case 'f':
		return '\f';
		break;
	case 'n':
		return '\n';
		break;
	case 'r':
		return '\r';
		break;
	case 't':
		return '\t';
		break;
	case 'v':
		return '\v';
		break;
	case '0':
		return '\0';
	default:
		throw json_parse_error(JSON_ERR(line, col) "Unrecognized escape sequence : \"\\" + c + "\"");
		break;
	}

	return '\0';
}
std::string Json::readString(std::istream &in, size_t &line, size_t &col) {
	// '"' _* '"'
	char c = in.get();
	col++;

	if (c != '"') {
		throw json_parse_error(JSON_ERR(line, col) "Strings must be enclosed in quotes");
	}
	c = in.get();
	col++;

	std::string ret = "";

	while (c != '"') {
		if (c == '\\') {
			c = getEscapeCharacter(in, line, col);
		}

		ret += c;

		c = in.get();
		col++;
		if (c == EOF) {
			throw json_parse_error(JSON_ERR(line, col) "Unexpected end of file parsing string");
		}
	}

	return ret;
}

// Private parsing functions
Json *Json::parseObject(std::istream &in, size_t &line, size_t &col) {
	// '{' [String ':' Value] ',' ... '}'
	char c = in.get();
	col++;
	if (c != '{') {
		throw json_parse_error(JSON_ERR(line, col) "Unexpected start of object: \"" + c + "\"");
	}

	Json *ret = new Json();
	ret->initMap();

	// Whitespace may appear at start of object
	consumeWhitespace(in, line, col);

	for (c = in.peek(); c != '}'; c = in.peek()) {
		if (c == ',') {
			// Comma indicates another key/value is coming
			in.ignore();
			col++;
			continue;
		}
		else {
			// Whitespace may appear here
			consumeWhitespace(in, line, col);

			// Read name
			std::string name = readString(in, line, col);

			// Whitespace may appear here
			consumeWhitespace(in, line, col);

			// Colon should split key/value
			char c = in.get();
			col++;
			if (c != ':') {
				throw json_parse_error(JSON_ERR(line, col) "Unexpected token while parsing key/value pair: \"" + c + "\"");
			}

			// Read value
			Json *value = parseValue(in, line, col);

			// Store key/value
			ret->map->emplace(name, value);
		}
	}

	//Ignore '}'
	in.ignore();
	col++;

	return ret;
}
Json *Json::parseArray(std::istream &in, size_t &line, size_t &col) {
	// '[' [Value] ',' ... ']'
	char c = in.get();
	col++;
	if (c != '[') {
		throw json_parse_error(JSON_ERR(line, col) "Unexpected start of array: \"" + c + "\"");
	}

	Json *ret = new Json();
	ret->initArray();

	for (c = in.peek(); c != ']'; c = in.peek()) {
		if (std::isspace(c)) {
			// Whitespace may appear at start or end of array
			consumeWhitespace(in, line, col);
		}
		else if (c == ',') {
			// Comma indicates another value is coming
			in.ignore();
			col++;
			continue;
		}
		else {
			// Read value
			Json *value = parseValue(in, line, col);

			// Store value
			ret->arr->push_back(value);
		}
	}

	//Ignore ']'
	in.ignore();
	col++;

	return ret;
}
Json *Json::parseString(std::istream &in, size_t &line, size_t &col) {
	// '"' _* '"'
	std::string str = readString(in, line, col);

	Json *ret = new Json();
	ret->initString();
	*ret->str = str;

	return ret;
}
Json *Json::parseNumber(std::istream &in, size_t &line, size_t &col) {
	// ['-'] ('0'-'9')* ['.' ('0'-'9')*]
	std::string res = "";

	do {
		char c = in.get();
		col++;

		if (!Json::isNumeric(c)) {
			throw json_parse_error(JSON_ERR(line, col) "Unexpected token reading number: \"" + c + "\"");
		}

		res += c;
	} while (!isEndOfValue(in.peek()));

	Json *ret = new Json();
	float f;
	try {
		f = std::stof(res);
	}
	catch (std::invalid_argument err) {
		throw json_parse_error(JSON_ERR(line, col) "Could not convert to number: " + res + "\n" + err.what());
	}
	catch (std::out_of_range err) {
		throw json_parse_error(JSON_ERR(line, col) "Number out of range: " + res + "\n" + err.what());
	}

	if (std::fabsf(std::truncf(f) - f) < 0.000001f) {
		ret->initInt();
		*ret->i = (long long)f;
	}
	else {
		ret->initFloat();
		*ret->f = f;
	}

	return ret;
}
Json *Json::parseKeyword(std::istream &in, size_t &line, size_t &col) {
	// 'True' | 'False' | 'Null'
	std::string res = "";

	do {
		char c = in.get();
		col++;

		if ((c < 'a' || c > 'z') && (c < 'A' || c > 'Z')) {
			throw json_parse_error(JSON_ERR(line, col) "Unexpected token reading keyword: \"" + c + "\"");
		}

		res += std::tolower(c);
	} while (!isEndOfValue(in.peek()));

	Json *ret = new Json();

	if (res == "true") {
		ret->initBool();
		*ret->b = true;
	}
	else if (res == "false") {
		ret->initBool();
		*ret->b = false;
	}
	else if (res != "null") {
		throw json_parse_error(JSON_ERR(line, col) "Unrecognized keyword: " + res);
	}

	return ret;
}

Json *Json::parseValue(std::istream &in, size_t &line, size_t &col) {
	//	Object | Array | String | Number | Keyword

	// TODO - Support for read references

	// Get rid of leading whitespace
	consumeWhitespace(in, line, col);

	// Error check
	if (in.eof()) {
		throw json_parse_error(JSON_ERR(line, col) "Unexpected end of file while parsing value");
	}

	// Determine what type of value to parse
	char first = in.peek();
	Json *ret = nullptr;
	if (first == '{') {
		// Object
		ret = parseObject(in, line, col);
	}
	else if (first == '[') {
		// Array
		ret = parseArray(in, line, col);
	}
	else if (first == '"') {
		// String
		ret = parseString(in, line, col);
	}
	else if (first == '-' || first == '+' || (first >= '0' && first <= '9')) {
		// Number
		ret = parseNumber(in, line, col);
	}
	else {
		// Keyword
		ret = parseKeyword(in, line, col);
	}
	
	// Remove trailing whitespace
	consumeWhitespace(in, line, col);

	return ret;
}

// Read JX
Json *Json::fromStream(std::istream &in) {
	size_t line = 1, col = 0;
	return parseValue(in, line, col);
}
Json *Json::fromFile(std::filesystem::path file) {
	std::ifstream in;
	in.open(file);

	if (!in.is_open()) {
		throw json_file_error("Json: Failed to open file: " + file.string());
	}

	return fromStream(in);
}
Json *Json::fromString(std::string &str) {
	std::istringstream in(str);
	return fromStream(in);
}
Json *Json::fromType(Type type) {
	Json *json = new Json();

	switch (type) {
	case Type::Object:
		json->initMap();
		break;
	case Type::Array:
		json->initArray();
		break;
	case Type::String:
		json->initString();
		break;
	case Type::Integer:
		json->initInt();
		break;
	case Type::Float:
		json->initFloat();
		break;
	case Type::Bool:
		json->initBool();
		break;
	}

	return json;
}

// Write JX
void Json::writeTab(std::ostream &out, size_t indent) {
	for (size_t i = 0; i < indent; i++) {
		out << "\t";
	}
}

void Json::writeObject(Json *val, std::ostream &out, size_t indent, bool pretty) {
	writeTab(out, indent);
	out << "{";

	Object map = val->asObject();
	bool first = true;
	for (auto &pair : map) {
		if (!first) {
			out << ",";
		}
		else {
			first = false;
		}

		if (pretty) {
			out << "\n";
			writeTab(out, indent);
			out << "\"" << pair.first << "\" : ";
			writeValue(pair.second, out, indent + 1, pretty);
		}
		else {
			out << "\"" << pair.first << "\":";
			writeValue(pair.second, out, 0, pretty);
		}
	}
	if (pretty) {
		out << "\n";
	}

	writeTab(out, indent);
	out << "}";

	if (pretty) {
		out << "\n";
	}
}
void Json::writeArray(Json *val, std::ostream &out, size_t indent, bool pretty) {
	writeTab(out, indent);
	out << "[";

	Array &vec = val->asArray();
	bool first = true;
	for (Json *ptr : vec) {
		if (!first) {
			out << ",";
		}
		else {
			first = false;
		}

		if (pretty) {
			out << "\n";
			writeValue(ptr, out, indent + 1, pretty);
		}
		else {
			writeValue(ptr, out, 0, pretty);
		}
	}
	if (pretty) {
		out << "\n";
	}

	writeTab(out, indent);
	out << "]";
}
void Json::writeString(Json *val, std::ostream &out, size_t indent, bool pretty) {
	writeTab(out, indent);
	out << "\"" << val->asString() << "\"";
}
void Json::writeInteger(Json *val, std::ostream &out, size_t indent, bool pretty) {
	writeTab(out, indent);
	out << std::to_string(val->asInt());
}
void Json::writeFloat(Json *val, std::ostream &out, size_t indent, bool pretty) {
	writeTab(out, indent);
	out << std::to_string(val->asFloat());
}
void Json::writeBool(Json *val, std::ostream &out, size_t indent, bool pretty) {
	writeTab(out, indent);
	out << (val->asBool() ? "True" : "False");
}
void Json::writeNull(Json *val, std::ostream &out, size_t indent, bool pretty) {
	writeTab(out, indent);
	out << "Null";
}

void Json::writeValue(Json *val, std::ostream &out, size_t indent, bool pretty) {
	// TODO - Support for write references

	// Determine what type of value to write
	Type t = val->type();
	switch (t) {
	case Type::Object:
		writeObject(val, out, indent, pretty);
		break;
	case Type::Array:
		writeArray(val, out, indent, pretty);
		break;
	case Type::String:
		writeString(val, out, indent, pretty);
		break;
	case Type::Integer:
		writeInteger(val, out, indent, pretty);
		break;
	case Type::Float:
		writeFloat(val, out, indent, pretty);
		break;
	case Type::Bool:
		writeBool(val, out, indent, pretty);
		break;
	case Type::Null:
		writeNull(val, out, indent, pretty);
		break;
	}
}

void Json::write(bool pretty, std::ostream &out) {
	writeValue(this, out, 0, pretty);
}

// Initialize data
void Json::initMap() {
	if (valid()) {
		delData();
	}

	map = new Object();
	dataType = Type::Object;
}
void Json::initArray() {
	if (valid()) {
		delData();
	}

	arr = new Array();
	dataType = Type::Array;
}
void Json::initString() {
	if (valid()) {
		delData();
	}

	str = new std::string();
	dataType = Type::String;
}
void Json::initInt() {
	if (valid()) {
		delData();
	}

	i = new long long();
	dataType = Type::Integer;
}
void Json::initFloat() {
	if (valid()) {
		delData();
	}

	f = new float();
	dataType = Type::Float;
}
void Json::initBool() {
	if (valid()) {
		delData();
	}

	b = new bool();
	dataType = Type::Bool;
}

// Delete data
void Json::delData() {
	switch (dataType) {
	case Type::Object:
		delete map;
		break;
	case Type::Array:
		delete arr;
		break;
	case Type::String:
		delete str;
		break;
	case Type::Integer:
		delete i;
		break;
	case Type::Float:
		delete f;
		break;
	case Type::Bool:
		delete b;
		break;
	} 
	dataType = Type::Null;
}

Json::Json() {}
Json::~Json() {
	delData();
}

// Conversion functions
Json::Object &Json::asObject() {
	if (dataType != Type::Object) {
		throw json_type_error("Json: type is not object");
	}

	return *map;
}
Json::Array &Json::asArray() {
	if (dataType != Type::Array) {
		throw json_type_error("Json: type is not array");
	}

	return *arr;
}
std::string &Json::asString() {
	if (dataType != Type::String) {
		throw json_type_error("Json: type is not string");
	}

	return *str;
}
long long &Json::asInt() {
	if (dataType != Type::Integer) {
		throw json_type_error("Json: type is not int");
	}

	return *i;
}
float &Json::asFloat() {
	if (dataType != Type::Float) {
		throw json_type_error("Json: type is not float");
	}

	return *f;
}
bool &Json::asBool() {
	if (dataType != Type::Bool) {
		throw json_type_error("Json: type is not bool");
	}

	return *b;
}

// Conversion operators
Json::operator Json::Object &() {
	if (!valid()) {
		initMap();
	}
	else if (dataType != Type::Object) {
		throw json_type_error("Json: type is not object");
	}
	return *map;
}
Json::operator Json::Array &() {
	if (!valid()) {
		initArray();
	}
	else if (dataType != Type::Array) {
		throw json_type_error("Json: type is not array");
	}
	return *arr;
}
Json::operator std::string &() {
	if (!valid()) {
		initString();
	}
	else if (dataType != Type::String) {
		throw json_type_error("Json: type is not string");
	}
	return *str;
}
Json::operator long long &() {
	if (!valid()) {
		initInt();
	}
	else if (dataType != Type::Integer) {
		throw json_type_error("Json: type is not int");
	}
	return *i;
}
Json::operator float &() {
	if (!valid()) {
		initFloat();
	}
	else if (dataType != Type::Float) {
		throw json_type_error("Json: type is not float");
	}
	return *f;
}
Json::operator bool &() {
	if (!valid()) {
		initBool();
	}
	else if (dataType != Type::Bool) {
		throw json_type_error("Json: type is not bool");
	}
	return *b;
}

// Member access operators
const Json &Json::operator()(std::string &key) {
	if (ptr == nullptr) {
		throw json_null_error("Json: Data is null");
	}
	if (dataType != Type::Object) {
		throw json_type_error("Json: type is not object");
	}

	auto it = map->find(key);
	if (it == map->end()) {
		throw json_key_error("Json: Key does not exist: " + key);
	}

	return *(it->second);
}
const Json &Json::operator[](size_t idx) {
	if (ptr == nullptr) {
		throw json_null_error("Json: Data is null");
	}

	return *(arr[0][idx]);
}

// Member access functions
Json::Object::iterator Json::find(std::string &key) {
	if (!valid()) {
		initMap();
	}
	else if (dataType != Type::Object) {
		throw json_type_error("Json: type is not object");
	}
	return map->find(key);
}
const Json &Json::at(size_t idx) {
	if (ptr == nullptr) {
		throw json_null_error("Json: Data is null");
	}

	return *arr->at(idx);
}

// Data
bool Json::valid() {
	return ptr != nullptr;
}
Json::Type Json::type() {
	return dataType;
}
size_t Json::size() {
	switch (dataType) {
	case Type::Object:
		return map->size();
		break;
	case Type::Array:
		return arr->size();
		break;
	case Type::String:
		return str->size();
		break;
	case Type::Integer:
		return 1;
		break;
	case Type::Float:
		return 1;
		break;
	case Type::Bool:
		return 1;
		break;
	default:
		return 0;
		break;
	}
}

_XELA_JSON_END
#endif