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
private:

public:
	Xss();
	~Xss();

	static Xss *fromStream(std::istream &in);
	static Xss *fromFile(std::filesystem::path file);
	static Xss *fromString(std::string &str);

	void write(bool pretty = false, std::ostream &out = std::cout);
};
_XELA_XSS_END
#endif

#define XELA_XSS_IMPLEMENTATION
#ifdef XELA_XSS_IMPLEMENTATION

#define XSS_ERR(line, col) std::string("Xss [" + std::to_string(line) + ", " + std::to_string(col) + "]: ") +

_XELA_XSS_START // C style structs and functions

Xss::Xss() {}
Xss::~Xss() {}

Xss *Xss::fromStream(std::istream &in) {
	size_t line = 1, col = 0;
	// return parseXss(in, line, col);
	return nullptr;
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