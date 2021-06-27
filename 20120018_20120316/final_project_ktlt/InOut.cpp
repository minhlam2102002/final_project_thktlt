﻿#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS_
#include <fstream> 
#include <fcntl.h> //_O_WTEXT
#include <io.h>    //_setmode()
#include <locale>
#include <codecvt> 
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "InOut.h"
using namespace std;
void InOut() {
	_setmode(_fileno(stdout), _O_WTEXT); //needed for output
	_setmode(_fileno(stdin), _O_WTEXT); //needed for input
}

std::wstring ReadFileUTF8(string path) {
	std::wstring line, res;
	std::wifstream fin(path);
	std::locale loc(std::locale(), new std::codecvt_utf8<wchar_t>);  // UTF-8
	fin.imbue(loc);
	while (getline(fin, line)) {
		res += line;
		res += L"\n";
	}
	fin.close();
	return res;
}

std::wstring ReadFileUTF16(string path) {
	std::wstring line, res;
	std::wifstream fin(path);
	fin.imbue(std::locale(fin.getloc(), new std::codecvt_utf16<wchar_t, 0x10ffff, std::consume_header>));
	while (getline(fin, line)) {
		res += line;
		res += L"\n";
	}
	fin.close();
	return res;
}
void PrintFileUTF8(string path, std::wstring content) {
	std::locale loc(std::locale(), new std::codecvt_utf8<wchar_t>);  // UTF-8
	std::wofstream fout(path);
	fout.imbue(loc);
	fout << content;
	fout.close();
}
