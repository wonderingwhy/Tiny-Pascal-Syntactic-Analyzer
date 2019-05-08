#include <set>
#include <map>
#include <stack>
#include <queue>
#include <cstdio>
#include <vector>
#include <string>
#include <cstring>
#include <cassert>
#include <sstream>
#include <fstream>
#include <utility>
#include <iostream>
#include <algorithm>

//#define TEST
//#define DEBUG
//#define CH02
const unsigned __int64 INF = ((1ull << 64) - 1);
#ifdef TEST
const int MAXN = 21;
const int MINN = 16;
const int MAXT = 7;
const int SENTC = 8;
#else
const int MAXN = 162;
const int MINN = 128;
const int MAXT = 36;
const int SENTC = 67;
#endif

class Grammar {
private:
	std::map<std::string, int> tMap;
	std::string ref[MAXN];
	std::vector<std::pair<int, std::vector<int> > > sVct;
	std::set <int> head[MAXN];
	std::set <int> first[MAXN];
	std::set <int> follow[MAXN];
	std::set <int> last[MAXN];
	int table[MAXN][MAXT];
	
	void readTerm() {
#ifdef TEST
		std::ifstream fin("testTer.txt");
#else
		std::ifstream fin("terminator.txt");
#endif
		int value;
		std::string key;
		while (fin >> value >> key) {
			ref[value] = key;
			tMap[key] = value;
		}
		ref[0] = "ε";
	}
	void readSentc() {
#ifdef TEST
		std::ifstream fin("testGra.txt");
#else
		std::ifstream fin("grammar.txt");
#endif
		int par, son;
		while (fin >> par) {
			std::vector <int> mVct;
			while (fin >> son) {
				if (son == -1) {
					break;
				}
				mVct.push_back(son);
			}
			sVct.push_back(std::make_pair(par, mVct));
		}
	}
	void getFirst() {
		bool flag = true;
		while (flag) {
			flag = false;
			for (int i = 0; i < sVct.size(); ++i) {
				int par = sVct[i].first;
				std::vector <int> mVct = sVct[i].second;
				for (int j = 0; j < sVct[i].second.size(); ++j) {
					if (head[par].find(mVct[j]) == head[par].end()) {
						head[par].insert(mVct[j]);
						flag = true;
					}
					if (first[mVct[j]].find(0) == first[mVct[j]].end()) {
						break;
					}
				}
			}
			for (int i = MINN; i < MAXN; ++i) {
				int cur = i;
				dfsFirst(i, cur);
			}
			for (int i = 0; i < MAXT; ++i) {
				first[i].insert(i);
			}
		}
	}
	void getFollow() {
		follow[MINN].insert(MAXT - 1);
		for (int i = 0; i < SENTC; ++i) {
			int cur = i, loc = 0;
			dfsFollow(0, cur, loc);
			for (int j = loc; j < sVct[i].second.size(); ++j) {
				last[sVct[i].first].insert(sVct[i].second[j]);
			}
		}
		for (int i = MINN; i < MAXN; ++i) {
			updateFollow(i);
		}
		for (int i = MINN; i < MAXN; ++i) {
			std::set<int>::iterator it = follow[i].find(0);
			if (it != follow[i].end()) {
				follow[i].erase(it);
			}
		}
	}
	void getTable() {
		memset(table, -1, sizeof(table));
		for (int i = 0; i < SENTC; ++i) {
			int par = sVct[i].first;
			std::vector <int> mVct = sVct[i].second;
			for (std::set<int>::iterator it = first[par].begin(); it != first[par].end(); ++it) {
				if (first[mVct[0]].find(*it) != first[mVct[0]].end()) {
					table[par][*it] = i;
				}
			}
			if (first[mVct[0]].find(0) != first[mVct[0]].end()) {
				for (std::set<int>::iterator it = follow[mVct[0]].begin(); it != follow[mVct[0]].end(); ++it) {
					if (table[par][*it] == -1) {
						table[par][*it] = i;
					}
				}
			}
		}
	}
	void dfsFirst(int index, const int &cur) {
		if (index < MAXT) {
			first[cur].insert(index);
			return;
		}
		for (std::set<int>::iterator it = head[index].begin(); it != head[index].end(); ++it) {
			dfsFirst(*it, cur);
		}
	}
	std::set <int> dfsFollow(int index, const int &cur, int &loc) {
		std::set<int> tmp;
		if (index >= sVct[cur].second.size()) {
			return tmp;
		}
		tmp = first[sVct[cur].second[index]];
		std::set<int> ret = dfsFollow(index + 1, cur, loc);
		follow[sVct[cur].second[index]].insert(ret.begin(), ret.end());
		if (first[sVct[cur].second[index]].find(0) != first[sVct[cur].second[index]].end()) {
			tmp.insert(ret.begin(), ret.end());
		}
		else {
			loc = std::max(loc, index);
		}
		return tmp;
	}
	void updateFollow(int index) {
		for (std::set<int>::iterator it = last[index].begin(); it != last[index].end(); ++it) {
			int len = follow[*it].size();
			follow[*it].insert(follow[index].begin(), follow[index].end());
			if (follow[*it].size() > len) {
				updateFollow(*it);
			}
		}
	}

public:
	Grammar() {}
	~Grammar() {}
	std::map<std::string, int> getTerm() {
		return tMap;
	}
	std::string getRef(int index) {
		return ref[index];
	}
	std::set <int> getFirst(int index) {
		return first[index];
	}
	std::set <int> getFollow(int index) {
		return follow[index];
	}
	std::vector<std::pair<int, std::vector<int> > > getSentc() {
		return sVct;
	}
	std::set <int> getHead(int index) {
		return head[index];
	}	
	int getTable(int x, int y) {
		return table[x][y];
	}
	void outTerm() {
		for (std::map<std::string, int>::iterator it = tMap.begin(); it != tMap.end(); ++it) {
			std::cout << it->first << " " << it->second << std::endl;
		}
	}
	void outSentc() {
		for (int i = 0; i < sVct.size(); ++i) {
			printf("%d->", sVct[i].first);
			for (int j = 0; j < sVct[i].second.size(); ++j) {
				printf("%d ", sVct[i].second[j]);
			}
			puts("");
		}
	}	
	void outFirst() {
		std::cout << "-----------------------------FIRST-------------------------------" << std::endl;
#ifdef DEBUG
		for (int i = MINN; i < MAXN; ++i) {
			std::cout << "first " << i << ": ";
			for (std::set<int>::iterator it = first[i].begin(); it != first[i].end(); ++it) {
				std::cout << *it << " ";
			}
			std::cout << std::endl;
		}
#else
		for (int i = MINN; i < MAXN; ++i) {
			std::cout << "FIRST(" << ref[i] << ") = {";
			for (std::set<int>::iterator it = first[i].begin(); it != first[i].end(); ++it) {
				if (it != first[i].begin()) {
					std::cout << ", " << ref[*it];
				}
				else {
					std::cout << ref[*it];
				}
			}
			std::cout << "}" << std::endl;
		}
#endif
	}
	void outFollow() {
		
		std::cout << "-----------------------------FOLLOW------------------------------" << std::endl;
#ifdef DEBUG
		for (int i = MINN; i < MAXN; ++i) {
			std::cout << "follow " << i << ": ";
			for (std::set<int>::iterator it = follow[i].begin(); it != follow[i].end(); ++it) {
				std::cout << *it << " ";
			}
			std::cout << std::endl;
		}
#else
		for (int i = MINN; i < MAXN; ++i) {
			std::cout << "FOLLOW(" << ref[i] << ") = {";
			for (std::set<int>::iterator it = follow[i].begin(); it != follow[i].end(); ++it) {
				if (it != follow[i].begin()) {
					std::cout << ", " << ref[*it];
				}
				else {
					std::cout << ref[*it];
				}
			}
			std::cout << "}" << std::endl;
		}
#endif
	}	
	void outTable() {
		std::cout << "-----------------------------TABLE-------------------------------" << std::endl;
		std::cout << "     ";
		for (int i = 1; i < MAXT; ++i) {
			std::cout << "|    " << ref[i] << "    ";
		}
		std::cout << std::endl;
		std::cout << "-----------------------------------------------------------------" << std::endl;
		for (int i = MINN; i < MAXN; ++i) {
			std::cout << "  " << ref[i];
			for (int j = 0; j < 3 - ref[i].size(); ++j) {
				std::cout << " ";
			}
			for (int j = 1; j < MAXT; ++j) {
				std::cout << "| ";
				int res = table[i][j];
				if (res == -1) {
					std::cout << "        ";
					continue;
				}
				int len = 0;
				std::cout << ref[sVct[res].first] << "->";
				len += ref[sVct[res].first].size() + 2;
				std::vector<int> mVct = sVct[res].second;
				for (int k = 0; k < mVct.size(); ++k) {
					std::cout << ref[mVct[k]];
					len += ref[mVct[k]].size();
					
				}
				for (int i = 0; i < 8 - len; ++i) {
					std::cout << " ";
				}
			}
			std::cout << std::endl;
			std::cout << "-----------------------------------------------------------------" << std::endl;
		}
	}	
	void init() {
		readTerm();
		readSentc();
		getFirst();
		getFollow();
		getTable();
	}

};

class Program {
private:
	std::string input;
	std::vector <int> iVct;
	int lexRes;
	int row, col;
	std::string errStr;
	bool match(const char &a, const char &b) {
		if (a == ':' && b == '=') {
			return true;
		}
		if (a == '>' && b == '=') {
			return true;
		}
		if (a == '<' && (b == '>' || b == '=')) {
			return true;
		}
		return false;
	}
	int getPair(std::string sym, std::map <std::string, int> &mMap) {
		std::map<std::string, int>::iterator it = mMap.find(sym);
		if (it != mMap.end()) {
			if (it->second > 0 && it->second < MAXT) {
				iVct.push_back(it->second);
			}
			else {
				iVct.push_back(34);
			}
		}
		else if (isdigit(sym[0]) != 0) {
			if (getValue(sym) == -1) {
				return -1;
			}
			iVct.push_back(33);
		}
		else {
			iVct.push_back(34);
		}
		return 0;
	}
	int getValue(std::string &str) {
		while (true) {
			if (str.size() <= 1 || str[0] != '0') {
				break;
			}
			str = str.substr(1);
		}
		std::ostringstream oss;
		oss << INF;
		std::string strInf = oss.str();
		if ((int)str.size() >= 21 || ((int)str.size() == 20 && str > strInf)) {
			errStr = str;
			return -1;
		}
		return 0;
	}

public:
	Program() {}
	~Program() {}
	int getLexRes() {
		return lexRes;
	}
	std::string getErrStr() {
		return errStr;
	}
	void output() {
		printf("%d\n", lexRes);
		for (int i = 0; i < iVct.size(); ++i) {
			printf("%d ", iVct[i]);
		}
		puts("");
	}
	void readInput() {
#ifdef TEST
		std::ifstream fin("testPro.txt");
#else
		std::ifstream fin("program.txt");
#endif
		std::string line;
		while (std::getline(fin, line)) {
			input += line;
			input += "\n";
		}
		if (input[input.size() - 1] == '\n') {
			input = input.substr(0, input.size() - 1);
		}
	}
#ifdef TEST
	void lexAnl(std::map<std::string, int> &mMap) {
		if ((int)input.find("#") >= 0) {
			errStr = "#";
			lexRes = -1;
			return;
		}
		for (int i = 0; i < input.size(); ++i) {
			std::map<std::string, int>::iterator it = mMap.find(input.substr(i, 1));
			if (it == mMap.end()) {
				errStr = input[i];
				lexRes = -1;
				return;
			}
			iVct.push_back(it->second);
		}
		iVct.push_back(MAXT - 1);
		lexRes = 0;
	}
#else
	void lexAnl(std::map<std::string, int> &mMap) {
		if ((int)input.find('#') > 0) {
			errStr = "#";
			lexRes = -1;
			return;
		}
		row = col = 1;
		int head, tail, preHead = -1;
		for (head = 0; head < (int)input.size(); head = tail) {
			if (isspace(input[head]) != 0) {
				if (input[head] == '\n') {
					++row;
					preHead = head;
				}
				for (tail = head + 1; tail < (int)input.size(); ++tail) {
					if (input[tail] == '\n') {
						++row;
						preHead = tail;
					}
					if (isspace(input[tail]) == 0) {
						break;
					}
				}
			}
			else if (isalpha(input[head]) != 0) {
				for (tail = head + 1; tail < (int)input.size(); ++tail) {
					if (isalnum(input[tail]) == 0) {
						break;
					}
				}
			}
			else if (isdigit(input[head]) != 0) {
				for (tail = head + 1; tail < (int)input.size(); ++tail) {
					if (isdigit(input[tail]) == 0) {
						break;
					}
				}
			}
			else if (mMap[input.substr(head, 1)] > 0 && mMap[input.substr(head, 1)]<MAXT) {
				if (head != (int)input.size() - 1 && match(input[head], input[head + 1]) == true) {
					tail = head + 2;
					col += 2;
				}
				else {
					tail = head + 1;
				}
			}
			else {
				col = head - preHead;
				errStr = input[head];
				lexRes = -1;
				return;
			}
			if (isspace(input[head]) == 0) {
				std::string tmp = input.substr(head, tail - head);
				if (getPair(tmp, mMap) == -1) {
					col = head - preHead;
					errStr = tmp;
					lexRes = -2;
					return;
				}
			}
		}
		lexRes = 0;
		iVct.push_back(MAXT - 1);
	}
#endif
	std::vector <int> getVct() {
		return iVct;
	}
	std::string getInput() {
		return input;
	}

};

class Match
{
public:
	Match() {}
	~Match() {}
	//ch02递归分析
	bool recurMatch(Grammar &mGra, Program &mPro) {
		std::cout << "-----------------------------最左推导-----------------------------" << std::endl;
		std::cout << mGra.getRef(MINN) << std::endl;
		int cur = 0;
		std::deque <int> left, right;
		int ret = match(mGra, mPro, cur, 0, left, right);
		if (ret == false) {
			return ret;
		}
		else {
			std::cout << mPro.getInput() << std::endl;
			return ret;
		}
	}
	//ch03预测分析
	bool stackMatch(Grammar &mGra, Program &mPro) {
		std::cout << "-----------------------------MATCH-------------------------------" << std::endl;
		std::stack <int> mStk;
		mStk.push(MAXT - 1);
		mStk.push(MINN);
		int cur = 0;
		int step = 1;
		std::vector < std::pair<int, std::vector<int> > > sVct= mGra.getSentc();
		std::vector <int> mVct = mPro.getVct();
		while (mStk.empty() == false) {
			int t = mStk.top();
			mStk.pop();
			if (t == 0) {
				continue;
			}
			printf("%03d: ", step++);
			if (t == mVct[cur]) {
				std::cout << "match " << mGra.getRef(t) << std::endl;
				++cur;
				continue;
			}
			int res = mGra.getTable(t, mVct[cur]);
			if (res >= 0) {
				for (int i = sVct[res].second.size() - 1; i >= 0; --i) {
					mStk.push(sVct[res].second[i]);
				}
				std::cout << "use   " << mGra.getRef(sVct[res].first) << "->";
				for (int i = 0; i < sVct[res].second.size(); ++i) {
					std::cout << mGra.getRef(sVct[res].second[i]);
				}
				std::cout << std::endl;
				continue;
			}
			std::cout << "无法匹配 " << mGra.getRef(mVct[cur]) << std::endl;
			return false;
		}
		return true;
	}

private:
	bool match(Grammar &mGra, Program &mPro, int &cur, int sentc, std::deque <int> &left, std::deque <int> right) {
		std::vector<std::pair<int, std::vector<int> > > sVct = mGra.getSentc();
		int par = sVct[sentc].first;
		std::vector <int> pVct = sVct[sentc].second;
		std::vector <int> mVct = mPro.getVct();
		for (int i = 0; i < left.size(); ++i) {
			std::cout << mGra.getRef(left[i]);
		}
		for (int i = 0; i < pVct.size(); ++i) {
			if (pVct[i] != 0) {
				std::cout << mGra.getRef(pVct[i]);
			}
		}
		for (int i = 0; i < right.size(); ++i) {
			std::cout << mGra.getRef(right[i]);
		}
		std::cout << std::endl;
		for (int i = 0; i < pVct.size(); ++i) {
			if (pVct[i] == 0) {
				continue;
			}
			if (pVct[i] == mVct[cur]) {
				++cur;
				left.push_back(pVct[i]);
				continue;
			}
			int res = mGra.getTable(pVct[i], mVct[cur]);
			if (res == -1) {
				return false;
			}
			for (int j = pVct.size() - 1; j > i; --j) {
				if (pVct[j] != 0) {
					right.push_front(pVct[j]);
				}
			}
			if (match(mGra, mPro, cur, res, left, right) == false) {
				return false;
			}
		}
		return true;
	}

};

int main() {
	Program mPro;
	Grammar mGra;
	Match mMat;
	mGra.init();
	mGra.outFirst();
	mGra.outFollow();
#ifdef TEST
	mGra.outTable();
#else
#endif
	mPro.readInput();
	mPro.lexAnl(mGra.getTerm());
	std::cout << "-----------------------------------------------------------------" << std::endl;
	if (mPro.getLexRes() != 0) {
		std::cout << "词法分析失败 无法识别 " << mPro.getErrStr() << std::endl;
	}
	else {
		puts("词法分析成功");
#ifdef CH02
		bool flag = mMat.recurMatch(mGra, mPro);
#else
		bool flag = true;// mMat.stackMatch(mGra, mPro);
#endif
		if (flag == true) {
			puts("YES");
		}
		else {
			puts("NO");
		}
	}
	std::cout << std::endl;
	system("pause");
	return 0;
}