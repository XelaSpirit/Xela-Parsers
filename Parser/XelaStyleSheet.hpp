// Xela Style Sheet
// 
// Author: Alex Morse
// 
// TODO - add description here
// 
// This software is dual-licensed to the public domain and under the following
// license: you are granted a perpetual, irrevocable license to copy, modify,
// publish, and distribute this file as you see fit.

#ifndef _XELA_STYLE_SHEET_HPP
#define _XELA_STYLE_SHEET_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#define _XELA_XSS_START namespace Xela {  extern "C" {
#define _XELA_XSS_END } }

_XELA_XSS_START // C style structs and functions

class xss_file_error : public std::runtime_error {
public:
	xss_file_error(const std::string &str) : runtime_error(str) {}
	xss_file_error(const char *str) : runtime_error(str) {}
};
class xss_parse_error : public std::runtime_error {
public:
	xss_parse_error(const std::string &str) : runtime_error(str) {}
	xss_parse_error(const char *str) : runtime_error(str) {}
};

class Xss {
public:
	struct Value {
		enum Type {
			STRING, NUMBER
		};
		union {
			std::string str;
			std::uint32_t num = 0;
		};
		Type type;

		Value() {
			new (&num) std::uint32_t{ 0 };
			type = NUMBER;
		}
		Value(const Value &other) {
			type = other.type;

			switch (other.type) {
			case STRING:
				str = other.str;
				break;
			case NUMBER:
				num = other.num;
				break;
			}
		}

		Value(std::string s) {
			str = s;
			type = STRING;
		}

		Value(std::uint32_t i) {
			num = i;
			type = NUMBER;
		}

		Value &operator=(std::string s) {
			str = s;
			type = STRING;
		}

		~Value() {
			if (type == STRING) {
				str.~basic_string();
			}
		}
	};

	using ChildArr = std::vector<Xss *>;
	using StyleMap = std::unordered_map<std::string, Value>;

private:
	std::string name;
	StyleMap style;

	ChildArr children;

	static void consumeWhitespace(std::istream &in, size_t &line, size_t &col);
	static char getEscapeCharacter(std::istream &in, size_t &line, size_t &col);

	static std::string parseIdentifier(std::istream &in, size_t &line, size_t &col);
	static std::string parseString(std::istream &in, size_t &line, size_t &col);
	static std::uint32_t parseHex(std::istream &in, size_t &line, size_t &col);

	static Value parseValue(std::istream &in, size_t &line, size_t &col);

	static Xss *parseStyle(std::istream &in, std::string &key, size_t &line, size_t &col);
	static Value parseSpec(std::istream &in, size_t &line, size_t &col);

	static Xss *parseXss(std::istream &in, size_t &line, size_t &col, Xss *xss);

public:
	Xss();
	~Xss();

	static Xss *fromStream(std::istream &in);
	static Xss *fromFile(std::filesystem::path file);
	static Xss *fromString(std::string &str);

	//void write(bool pretty = false, std::ostream &out = std::cout);
};
_XELA_XSS_END
#endif

#define XELA_XSS_IMPLEMENTATION
#ifdef XELA_XSS_IMPLEMENTATION

#define XSS_ERR(line, col) std::string("Xss [" + std::to_string(line) + ", " + std::to_string(col) + "]: ") +

_XELA_XSS_START // C style structs and functions

void Xss::consumeWhitespace(std::istream &in, size_t &line, size_t &col) {
	while (std::isspace(in.peek())) {
		char c = in.get();
		col++;
		if (c == '\n') {
			line++;
			col = 0;
		}
	}
}
char Xss::getEscapeCharacter(std::istream &in, size_t &line, size_t &col) {
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
		throw xss_parse_error(XSS_ERR(line, col) "Unrecognized escape sequence : \"\\" + c + "\"");
		break;
	}

	return '\0';
}

std::string Xss::parseIdentifier(std::istream &in, size_t &line, size_t &col) {
	// ([a-z] | [A-Z] | [0-9] | '_')*

	std::string ident = "";

	while (true) {
		char c = in.get();
		col++;

		if (std::isspace(c) || c == '<' || c == '>' || c == '/' || c == '=') {
			in.unget();
			col--;
			return ident;
		}

		if ((c >= 'a' && c <= 'z')
			|| (c >= 'A' && c <= 'Z')
			|| (c >= '0' && c <= '9')
			|| c == '_') {
			ident += c;
		}
		else {
			throw xss_parse_error(XSS_ERR(line, col) "Unexpected token while parsing identifier: " + c);
		}

		// Error check
		if (in.eof()) {
			throw xss_parse_error(XSS_ERR(line, col) "Unexpected end of file while parsing identifier");
		}
	}
}
std::string Xss::parseString(std::istream &in, size_t &line, size_t &col) {
	// '"' CHAR* '""

	std::string result = "";

	char c = in.get();
	col++;
	if (c != '"') {
		throw xss_parse_error(XSS_ERR(line, col) "Unexpected token while parsing string: " + c + ". Expected '\"'");
	}

	for (c = in.get(); c != '\"'; c = in.get()) {
		result += c;
		if (in.eof()) {
			throw xss_parse_error(XSS_ERR(line, col) "Unexpected end of file while parsing string");
		}
	}

	return result;
}
std::uint32_t Xss::parseHex(std::istream &in, size_t &line, size_t &col) {
	// '#' ([0-9] | [a-f] | [A-F])+

	std::uint32_t result = 0;
	std::string str = "";

	// Ensure first character is '#'
	char c = in.get();
	col++;
	if (c != '#') {
		throw xss_parse_error(XSS_ERR(line, col) "Unexpected token while parsing hex: " + c + ". Expected '#'");
	}

	// Read hex string
	size_t len = 0;
	for (c = in.get(); (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); c = in.get()) {
		str += c;
		col++;

		len++;
		if (len > 8) {
			throw xss_parse_error(XSS_ERR(line, col) "Hex string longer than 8 characters");
		}
	}

	// Parse hex value
	size_t shift = 0;
	for (size_t idx = str.length() - 1; idx >= 0; idx--) {
		char &curr = str[idx];
		std::uint32_t uint = 0;

		if (curr >= '0' && curr <= '9') {
			uint = (std::uint32_t)(curr - '0');
		}
		else if (curr >= 'a' && curr <= 'f') {
			uint = (std::uint32_t)(curr - 'a');
		}
		else if (curr >= 'A' && curr <= 'F') {
			uint = (std::uint32_t)(curr - 'A');
		}

		result += (uint << shift);
		shift += 4;
	}

	return result;
}

Xss::Value Xss::parseValue(std::istream &in, size_t &line, size_t &col) {
	Value result{};

	// Leading whitespace
	consumeWhitespace(in, line, col);

	// Determine whether this is a string, hex, or ident
	char c = in.peek();
	switch (c) {
	case '\"':
		result = parseString(in, line, col);
		break;
	case '#':
		result.num = parseHex(in, line, col);
		result.type = Value::NUMBER;
		break;
	default:
		result.str = parseIdentifier(in, line, col);
		result.type = Value::STRING;
		break;
	}

	// Trailing whitespace
	consumeWhitespace(in, line, col);

	return result;
}

Xss *Xss::parseStyle(std::istream &in, std::string &key, size_t &line, size_t &col) {
	// WS key WS '{' xss* '}' WS

	// Leading whitespace
	consumeWhitespace(in, line, col);

	// Next character should be '{'
	char c = in.get();
	col++;
	if (c != '{') {
		throw xss_parse_error(XSS_ERR(line, col) "Unexpected token while parsing style: " + c + ". Expected '{'");
	}

	// Parse xss
	Xss *result = new Xss();
	result->name = key;

	while (in.peek() != '}') {
		// Error check
		if (in.eof()) {
			throw xss_parse_error(XSS_ERR(line, col) "Reached unexpecred end of file while parsing style. Expected style to end with '}'");
		}

		// Leading whitespace
		consumeWhitespace(in, line, col);

		// Check for end of style
		if (in.peek() == '}') {
			continue;
		}

		parseXss(in, line, col, result);
	}

	// Trailing whitespace
	consumeWhitespace(in, line, col);

	return result;
}
Xss::Value Xss::parseSpec(std::istream &in, size_t &line, size_t &col) {
	// WS ident WS ':' value ';' WS

	// Leading whitespace
	consumeWhitespace(in, line, col);

	// Next character should be ':'
	char c = in.get();
	col++;
	if (c != ':') {
		throw xss_parse_error(XSS_ERR(line, col) "Unexpected token while parsing spec: " + c + ". Expected ':'");
	}

	// Get value
	Value result = parseValue(in, line, col);

	// Next character should be ';'
	c = in.get();
	col++;
	if (c != ';') {
		throw xss_parse_error(XSS_ERR(line, col) "Unexpected token while parsing spec: " + c + ". Expected ';'");
	}

	return result;
}

Xss *Xss::parseXss(std::istream &in, size_t &line, size_t &col, Xss *xss) {
	// (spec | style)*

	if (xss == nullptr) {
		xss = new Xss();
	}
	
	// Leading whitespace
	consumeWhitespace(in, line, col);

	// Error check
	if (in.eof()) {
		throw xss_parse_error(XSS_ERR(line, col) "Unexpected end of file while parsing xss");
	}

	// Get first key/ident. This may be for either a spec or a style so we have to parse it here
	// Beginning of style: WS key WS '{' ...
	// Beginning of spec: WS ident WS ':' ...
	std::string name = "";

	// Key accepts '#' or '.' as a first character, followed by an identifier, so make sure that character may be accepted.
	char c = in.peek();
	if (c == '#' || c == '.') {
		in.ignore();
		col++;

		name += c;
	}
	// Parse rest of key/ident
	name += parseIdentifier(in, line, col);
	
	// Whitespace before next character
	consumeWhitespace(in, line, col);

	// Determine whether this is for a spec or a style
	c = in.peek();
	switch (c) {
	case ':':
		// This is a spec
		xss->style.emplace(name, parseSpec(in, line, col));
		break;
	case '{':
		// This is a style
		xss->children.push_back(parseStyle(in, name, line, col));
		break;
	default:
		throw xss_parse_error(XSS_ERR(line, col) "Unexpected token while parsing xss: " + c + ". Expected ':' or '{'");
		break;
	}

	// Trailing whitespace
	consumeWhitespace(in, line, col);

	return xss;
}

Xss::Xss() {}
Xss::~Xss() {}

Xss *Xss::fromStream(std::istream &in) {
	size_t line = 1, col = 0;
	return parseXss(in, line, col, nullptr);
}
Xss *Xss::fromFile(std::filesystem::path file) {
	std::ifstream in;
	in.open(file);

	if (!in.is_open()) {
		throw xss_file_error("Xss: Failed to open file: " + file.string());
	}

	return fromStream(in);
}
Xss *Xss::fromString(std::string &str) {
	std::istringstream in(str);
	return fromStream(in);
}

_XELA_XSS_END
#endif