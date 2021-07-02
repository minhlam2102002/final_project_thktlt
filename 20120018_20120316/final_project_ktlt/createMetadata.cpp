#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include <fstream> 
#include <fcntl.h> //_O_WTEXT
#include <io.h>    //_setmode()
#include <locale>
#include <codecvt> 
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <sstream>
#include <regex>
#include "InOut.h"
#include "stringFunction.h"

#include <ctime>
#include <ratio>
#include <chrono>
using namespace std::chrono;

using namespace std;
namespace fs = std::filesystem;

string extractFileName(string path) {
	string res;
	for (int i = path.size() - 1; i >= 0 && path[i] != '\\'; i--) {
		res += path[i];
	}
	reverse(res.begin(), res.end());
	return res;
}
void getTopic(string path, vector<string>& topic) {
	for (auto& file : fs::directory_iterator(path)) {
		fs::path p = file.path();
		topic.push_back(extractFileName(p.string()));
	}
}
void createIndexFile(string root, string trainPath, vector<string>& topic) {
	ofstream out(root + "\\index.txt");
	out << topic.size() << endl;
	for (string topicName : topic) {
		out << topicName << endl;
		vector<string> files;
		for (auto& file : fs::directory_iterator(trainPath + "\\" + topicName)) {
			fs::path p = file.path();
			files.push_back(extractFileName(p.string()));
		}
		out << files.size() << endl;
		for (string cur : files) {
			out << cur << endl;
		}

	}
	out.close();
}

void extractKeyWord(string path, string& keyWords) { // exctract file at path
	//su dung tien xu ly :vvvv
	wstring w_content = ReadFileUTF16(path);
	string content = XoaDau(w_content);
	lowerCase(content);
	fixWord(content);
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	deleteStopWord(content);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double, std::milli> time_span = t2 - t1;
	cout << "deleteStopWord took " << time_span.count() << " milliseconds.";
	cout << endl << endl;
	t1 = high_resolution_clock::now();
	vector<string> gramWords[3], words;
	extractWord(content, words);
	vector<int> gramRate[3];
	for (int i = 0; i < 3; i++) {
		countAppearance(words, gramWords[i], gramRate[i], i);
		keyWords += to_string(gramWords[i].size()) + '\n';
		for (int j = 0; j < gramWords[i].size(); j++) {
			keyWords += gramWords[i][j] + ' ' + to_string(gramRate[i][j]) + '\n';
		}
	}
	t2 = high_resolution_clock::now();
	cout << "countAppearance took " << time_span.count() << " milliseconds.";
	cout << endl << endl;
}

void createMetadata(string root, string trainPath) {
	vector<string> topic;
	createStopWord();
	getTopic(trainPath, topic);
	createIndexFile(root, trainPath, topic);
	ifstream fin(root + "\\index.txt");
	ofstream fout(root + "\\metadata.txt");
	int numTopics, numFiles;
	fin >> numTopics;
	fin.ignore();
	fout << numTopics << endl;
	string curTopic, curFile;
	while (numTopics--) {
		high_resolution_clock::time_point tt1 = high_resolution_clock::now();
		getline(fin, curTopic);
		cout << "Reading " << curTopic << endl;
		fout << curTopic << endl;
		fin >> numFiles;
		fout << numFiles << endl;
		fin.ignore();
		while (numFiles--) {
			high_resolution_clock::time_point t1 = high_resolution_clock::now();
			getline(fin, curFile);
			cout << curFile << endl;
			fout << curFile << endl;
			string keyWords;
			extractKeyWord(trainPath + "\\" + curTopic + "\\" + curFile, keyWords);
			fout << keyWords;
			high_resolution_clock::time_point t2 = high_resolution_clock::now();
			duration<double, std::milli> time_span = t2 - t1;
			cout << "Process File took " << time_span.count() << " milliseconds.";
			cout << endl << endl;
			return; // run 1 file
		}
		high_resolution_clock::time_point tt2 = high_resolution_clock::now();
		duration<double, std::milli> time_span = tt2 - tt1;
		cout << "Process Topic took " << time_span.count() << " milliseconds.";
		cout << endl << endl;
		//return; // run 1 topic
	}
	fin.close();
	fout.close();
}