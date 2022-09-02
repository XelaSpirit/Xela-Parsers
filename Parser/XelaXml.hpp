// Xela Xml
// 
// Author: Alex Morse
// 
// TODO - add description here
// 
// This software is dual-licensed to the public domain and under the following
// license: you are granted a perpetual, irrevocable license to copy, modify,
// publish, and distribute this file as you see fit.

#ifndef _XELA_XML_HPP
#define _XELA_XML_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

#define _XELA_JX_START namespace Xela {  extern "C" {
#define _XELA_JX_END } }

class xml_file_error : public std::runtime_error {
public:
	xml_file_error(const std::string &str) : runtime_error(str) {}
	xml_file_error(const char *str) : runtime_error(str) {}
};
class xml_parse_error : public std::runtime_error {
public:
	xml_parse_error(const std::string &str) : runtime_error(str) {}
	xml_parse_error(const char *str) : runtime_error(str) {}
};

_XELA_JX_START // C style structs and functions
class Xml {
public:
	using AttrMap = std::unordered_map<std::string, std::string>;
	using ChildMap = std::unordered_map<std::string, std::vector<Xml *>>;

private:
	bool comment = false;
	std::string type = "";
	AttrMap attributes{};
	ChildMap children{};

	static void consumeWhitespace(std::istream &in, size_t &line, size_t &col);
	static char getEscapeCharacter(std::istream &in, size_t &line, size_t &col);

	static std::string parseString(std::istream &in, size_t &line, size_t &col);
	static std::string parseIdentifier(std::istream &in, size_t &line, size_t &col);

	static void parseAttribute(std::istream &in, size_t &line, size_t &col, std::pair<std::string, std::string> &out_attr);
	static std::string parseTagName(std::istream &in, size_t &line, size_t &col);

	static std::string parseTagData(std::istream &in, size_t &line, size_t &col, AttrMap &attributes);

	static Xml *parseTag(std::istream &in, size_t &line, size_t &col);
	static Xml *parseComment(std::istream &in, size_t &line, size_t &col);

	static Xml *parseXml(std::istream &in, size_t &line, size_t &col);

	static void writeXml(Xml *xml, std::ostream &out, size_t indent, bool prett);

public:
	Xml();
	~Xml();

	static Xml *fromStream(std::istream &in);
	static Xml *fromFile(std::filesystem::path file);
	static Xml *fromString(std::string &str);

	void write(bool pretty = false, std::ostream &out = std::cout);

	bool &isComment();
	void setComment(bool val);

	std::string &getType();
	void setType(std::string str);

	std::string &getComment();
	void setComment(std::string str);

	AttrMap &getAttributes();
	void addAttribute(std::string key, std::string value);
	void removeAttribute(std::string key);
	AttrMap::iterator findAttribute(std::string key);

	ChildMap &getChildren();
	void addChild(Xml *child);
	void removeChild(Xml *child);
};
_XELA_JX_END

#endif

#define XELA_XML_IMPLEMENTATION
#ifdef XELA_XML_IMPLEMENTATION

#define XML_ERR(line, col) std::string("Xml [" + std::to_string(line) + ", " + std::to_string(col) + "]: ") +

_XELA_JX_START //C style structs and functions

void Xml::consumeWhitespace(std::istream &in, size_t &line, size_t &col) {
	while (std::isspace(in.peek())) {
		char c = in.get();
		col++;
		if (c == '\n') {
			line++;
			col = 0;
		}
	}
}
char Xml::getEscapeCharacter(std::istream &in, size_t &line, size_t &col) {
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
		throw xml_parse_error(XML_ERR(line, col) "Unrecognized escape sequence : \"\\" + c + "\"");
		break;
	}

	return '\0';
}

std::string Xml::parseString(std::istream &in, size_t &line, size_t &col) {
	// '"' CHAR* '""
	
	std::string result = "";

	char c = in.get();
	col++;
	if (c != '"') {
		throw xml_parse_error(XML_ERR(line, col) "Unexpected token while parsing string: " + c + "Expected '\"'");
	}

	for (c = in.get(); c != '\"'; c = in.get()) {
		result += c;
		if (in.eof()) {
			throw xml_parse_error(XML_ERR(line, col) "Unexpected end of file while parsing string");
		}
	}

	return result;
}
std::string Xml::parseIdentifier(std::istream &in, size_t &line, size_t &col) {
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
			throw xml_parse_error(XML_ERR(line, col) "Unexpected token while parsing identifier: " + c);
		}

		// Error check
		if (in.eof()) {
			throw xml_parse_error(XML_ERR(line, col) "Unexpected end of file while parsing identifier");
		}
	}
}

void Xml::parseAttribute(std::istream &in, size_t &line, size_t &col, std::pair<std::string, std::string> &out_attr) {
	// ident WS '=' WS string WS

	std::string key = parseIdentifier(in, line, col);
	if (key == "") {
		return;
	}

	consumeWhitespace(in, line, col);

	char c = in.get();
	col++;
	if (c != '=') {
		throw xml_parse_error(XML_ERR(line, col) "Unexpected token while parsing attribute: " + c + ". Expected '='");
	}

	std::string val = parseString(in, line, col);
	consumeWhitespace(in, line, col);

	out_attr.first = key;
	out_attr.second = val;
}
std::string Xml::parseTagName(std::istream &in, size_t &line, size_t &col) {
	// WS ident WS

	consumeWhitespace(in, line, col);
	std::string name = parseIdentifier(in, line, col);
	consumeWhitespace(in, line, col);

	return name;
}

std::string Xml::parseTagData(std::istream &in, size_t &line, size_t &col, AttrMap &attributes) {
	// tagname WS attribute*

	auto name = parseTagName(in, line, col);
	consumeWhitespace(in, line, col);

	std::pair<std::string, std::string> pair{"", ""};
	for (parseAttribute(in, line, col, pair); pair.first != ""; parseAttribute(in, line, col, pair)) {
		attributes.emplace(pair);

		// Error check
		if (in.eof()) {
			throw xml_parse_error(XML_ERR(line, col) "Unexpected end of file while parsing tag data");
		}
	}

	return name;
}

Xml *Xml::parseTag(std::istream &in, size_t &line, size_t &col) {
	// WS '<' tagdata '>' xml* '</' tagname '>' WS
	// | WS '<' tagdata '/>' WS

	Xml *result = new Xml();

	// Tag must start with '<'
	char c = in.get();
	col++;
	if (c != '<') {
		throw xml_parse_error(XML_ERR(line, col) "Unexpected start of tag character while parsing tag: " + c);
	}

	// Get tag data
	result->setType(parseTagData(in, line, col, result->getAttributes()));

	// Parse end of tag or children
	c = in.get();
	col++;
	if (c == '>') {
		// End of open tag - parse children
		for (Xml *child = parseXml(in, line, col); child != nullptr; child = parseXml(in, line, col)) {
			result->addChild(child);
		}

		// Parse closing tag
		c = in.get();
		col++;
		if (c != '<') {
			throw xml_parse_error(XML_ERR(line, col) "Unexpected end of tag character while parsing tag: " + c + ". Expected '<'");
		}
		c = in.get();
		col++;
		if (c != '/') {
			throw xml_parse_error(XML_ERR(line, col) "Unexpected end of tag character while parsing tag: " + c + ". Expected '/'");
		}

		std::string closingType = parseTagName(in, line, col);
		if (result->getType() != closingType) {
			throw xml_parse_error(XML_ERR(line, col) "Closing tag type does not match open tag: " + closingType + " != " + result->getType());
		}
	}
	else if (c != '/') {
		throw xml_parse_error(XML_ERR(line, col) "Unexpected end of tag character while parsing tag: " + c + ". Expected '/' or '>'");
	}

	// End of tag
	c = in.get();
	col++;
	if (c != '>') {
		throw xml_parse_error(XML_ERR(line, col) "Unexpected end of tag character while parsing tag: " + c + ". Expected '>'");
	}

	return result;
}
Xml *Xml::parseComment(std::istream &in, size_t &line, size_t &col) {
	// '!--' CHAR* '-->'

	Xml *result = new Xml();
	result->setComment(true);

	// Verify '!'
	char c = in.get();
	col++;
	if (c != '!') {
		throw xml_parse_error(XML_ERR(line, col) "Unexpected character while parsing comment: " + c + ". Expected '!'");
	}
	// Verify '-'
	c = in.get();
	col++;
	if (c != '-') {
		throw xml_parse_error(XML_ERR(line, col) "Unexpected character while parsing comment: " + c + ". Expected '-'");
	}
	// Verify '-'
	c = in.get();
	col++;
	if (c != '-') {
		throw xml_parse_error(XML_ERR(line, col) "Unexpected character while parsing comment: " + c + ". Expected '-'");
	}

	std::string text = "";
	while (true) {
		c = in.get();
		col++;

		if (c == '-') {
			if (in.peek() == '-') {
				in.ignore();
				col++;

				if (in.peek() == '>') {
					in.ignore();
					col++;
					result->setComment(text);
					return result;
				}

				in.unget();
				col--;
			}
		}

		text += c;

		// Error check
		if (in.eof()) {
			throw xml_parse_error(XML_ERR(line, col) "Unexpected end of file while parsing comment");
		}
	}
}

Xml *Xml::parseXml(std::istream &in, size_t &line, size_t &col) {
	// WS '<' tagdata '>' xml* '</' tagname '>' WS | WS '<' tagdata '/>' WS | WS comment WS

	// Leading whitespace
	consumeWhitespace(in, line, col);

	// Error check
	if (in.eof()) {
		throw xml_parse_error(XML_ERR(line, col) "Unexpected end of file while parsing xml");
	}

	// Get first character
	char c = in.get();
	col++;
	// first character must be '<'
	if (c != '<') {
		throw xml_parse_error(XML_ERR(line, col) "Unexpected character while parsing xml: " + c + ". Expected '<'");
	}

	// Determine whether this is a comment, tag, or closing tag
	Xml *result = nullptr;
	c = in.peek();
	if (c == '!') {
		// Comment
		result = parseComment(in, line, col);
	}
	else if (c == '/') {
		// This is a closing tag
		in.unget();	// Go back to '<'
		col--;
		return nullptr;
	}
	else {
		// Tag
		in.unget(); // Go back to '<'
		col--;
		result = parseTag(in, line, col);
	}

	// Trailing whitespace
	consumeWhitespace(in, line, col);

	return result;
}

void Xml::writeXml(Xml *xml, std::ostream &out, size_t indent, bool prett) {
	// TODO
}

Xml::Xml() {}
Xml::~Xml() {
	for (auto &child : children) {
		for (auto &xml : child.second) {
			delete xml;
		}
	}
}

Xml *Xml::fromStream(std::istream &in) {
	size_t line = 1, col = 0;
	return parseXml(in, line, col);
}
Xml *Xml::fromFile(std::filesystem::path file) {
	std::ifstream in;
	in.open(file);

	if (!in.is_open()) {
		throw xml_file_error("Xml: Failed to open file: " + file.string());
	}

	return fromStream(in);
}
Xml *Xml::fromString(std::string &str) {
	std::istringstream in(str);
	return fromStream(in);
}

void Xml::write(bool pretty, std::ostream &out) {
	writeXml(this, out, 0, pretty);
}

bool &Xml::isComment() {
	return comment;
}
void Xml::setComment(bool val) {
	comment = val;
}

std::string &Xml::getType() {
	return type;
}
void Xml::setType(std::string str) {
	type = str;
	comment = false;
}

std::string &Xml::getComment() {
	return type;
}
void Xml::setComment(std::string str) {
	type = str;
	comment = true;
}

Xml::AttrMap &Xml::getAttributes() {
	return attributes;
}
void Xml::addAttribute(std::string key, std::string value) {
	attributes.emplace(key, value);
}
void Xml::removeAttribute(std::string key) {
	attributes.erase(key);
}
Xml::AttrMap::iterator Xml::findAttribute(std::string key) {
	return attributes.find(key);
}

Xml::ChildMap &Xml::getChildren() {
	return children;
}
void Xml::addChild(Xml *child) {
	std::string type = child->isComment() ? "" : child->getType();
	auto it = children.find(type);
	if (it == children.end()) {
		children.emplace(type, std::vector<Xml *>{ child });
	}
	else {
		it->second.push_back(child);
	}
}
void Xml::removeChild(Xml *child) {
	auto it = children.find(child->getType());
	if (it != children.end()) {
		for (size_t i = 0; i < it->second.size(); i++) {
			if (it->second[i] == child) {
				it->second.erase(it->second.begin() + i);
			}
		}
	}
}

_XELA_JX_END
#endif