#include <iostream>
#include <string.h>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <utility>

using namespace std;

string replace_complements(string, string);
bool in_vector(vector<string>, string);
bool is_hamming_distance_is_0(string, string);
vector<string> reduce(vector<string>);
bool vectors_equal(vector<string>, vector<string>);
vector<int> identify_minimal_PI_set(vector<string>, vector<string>);
int the_number_of_transistors(vector<int>, vector<string>);

int main() {

	/*txt파일에서 데이터를 읽어온다*/
	ifstream in("input_minterm5.txt");
	string s;
	vector<string> input_format;
	vector <string> true_for_minterm;
	if (in.is_open()) {
		while (!in.eof()) {
			getline(in, s);
			if (s[0] == 'm' || s[0] == 'M')
				true_for_minterm.push_back(s);
			input_format.push_back(s);
		}
	}
	else
		cout << "input 파일을 찾을 수 없습니다!" << endl;
	in.close();

	/*데이터에서 binary 정보만 따로 빼서 저장한다*/
	vector<string> minterms_a;
	string temp_a;

	for (int i = 1; i < input_format.size(); i++) {
		temp_a = "";

		for (int j = 2; j < input_format[i].length(); j++) {
			char temp_b = input_format[i][j];
			temp_a += temp_b;
		}

		minterms_a.push_back(temp_a);
	}

	/*identify PIs*/
	vector<string> minterms_b;

	do {
		if (0 < minterms_b.size())
		{
			minterms_a.resize(minterms_b.size());
			copy(minterms_b.begin(), minterms_b.end(), minterms_a.begin());
		}
		minterms_b = reduce(minterms_a);

	} while (!vectors_equal(minterms_a, minterms_b));

	/*prime implicants 출력*/
	/*for (int i = 0; i < minterms_b.size(); i++) {
		cout << minterms_b[i] << endl;
	}
	cout << endl;*/

	vector<int> entry_of_PI = identify_minimal_PI_set(true_for_minterm, minterms_b);

	int cost = the_number_of_transistors(entry_of_PI, minterms_b);

	/*결과를 txt로 저장*/
	ofstream out("result.txt");
	for (int i = 0; i < entry_of_PI.size(); i++)
		out << minterms_b[entry_of_PI[i]] << endl;

	out << endl << "Cost(# of transistors): " << cost << endl;

	out.close();

	system("pause");
	return 0;

}

//function to replace complemet terms with don't cares
//ex) 0110 and 0111 becomes 011-
string replace_complements(string a, string b)
{
	string temp = "";
	for (int i = 0; i < a.length(); i++)
		if (a[i] != b[i])
			temp = temp + "-";
		else
			temp = temp + a[i];
	return temp;
}

//function to check if string b exists in vector a
bool in_vector(vector<string> a, string b)
{
	for (int i = 0; i < a.size(); i++)
		if (a[i].compare(b) == 0)
			return true;
	return false;
}

//function to check if two terms differ by just one bit
bool is_hamming_distance_is_0(string a, string b)
{
	int flag = 0;
	for (int i = 0; i < a.length(); i++)
	{
		if (a[i] != b[i])
			flag++;
	}
	return (flag == 1);
}

//function to identify PIs
vector<string> reduce(vector<string> minterms_a)
{
	vector<string> minterms_b;

	int max = minterms_a.size();
	int *checked = new int[max];

	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++) {
			if (is_hamming_distance_is_0(minterms_a[i], minterms_a[j])) {
				checked[i] = 1;
				checked[j] = 1;
				if (!in_vector(minterms_b, replace_complements(minterms_a[i], minterms_a[j])))
					minterms_b.push_back(replace_complements(minterms_a[i], minterms_a[j]));
			}
		}
	}
	for (int i = 0; i < max; i++) {
		if (checked[i] != 1 && !in_vector(minterms_b, minterms_a[i]))
			minterms_b.push_back(minterms_a[i]);
	}
	delete[] checked;

	return minterms_b;
}

//function to check if 2 vectors are equal
bool vectors_equal(vector<string> a, vector<string> b)
{
	if (a.size() != b.size())
		return false;

	sort(a.begin(), a.end());
	sort(b.begin(), b.end());

	for (int i = 0; i < a.size(); i++)
	{
		if (a[i] != b[i])
			return false;
	}
	return true;
}

vector<int> identify_minimal_PI_set(vector<string> true_for_minterm, vector<string> minterms_b)
{
	int **PI = new int *[minterms_b.size()];
	pair <int, string> p;
	vector < pair<int, string> > v;

	for (int i = 0; i < minterms_b.size(); i++) {
		PI[i] = new int[true_for_minterm.size()];
	}

	for (int i = 0; i < minterms_b.size(); i++) {
		for (int j = 0; j < true_for_minterm.size(); j++) {
			PI[i][j] = 0;
		}
	}

	for (int i = 0; i < minterms_b.size(); i++) {
		int cnt = 0;
		for (int j = 0; j < minterms_b[1].length(); j++) {
			if (minterms_b[i][j] != '-')
				cnt++;
		}
		p = make_pair(cnt, minterms_b[i]);
		v.push_back(p);
	}


	for (int i = 0; i < v.size(); i++) {
		for (int j = 0; j < true_for_minterm.size(); j++) {
			int cnt = 0;
			for (int k = 0; k < v[1].second.length(); k++) {
				if (v[i].second[k] != '-') {
					if (true_for_minterm[j][k + 2] == v[i].second[k])
						cnt++;
				}
				if (cnt == v[i].first)
					PI[i][j] = 1;
			}
		}
	}

	vector <int> entry_of_PI;
	int entry = 0;
	for (int i = 0; i < true_for_minterm.size(); i++) {
		int cnt = 0;
		int cnt_2 = 0;
		for (int j = 0; j < v.size(); j++) {
			cnt_2 = cnt;
			cnt = cnt + PI[j][i];

			if (cnt == 1 && cnt_2 == 0)
				entry = j;
			if (j == v.size() - 1) {
				if (cnt != 1)
					entry = -1;
			}
		}
		if (entry >= 0)
			entry_of_PI.push_back(entry);
	}

	return entry_of_PI;

}

int the_number_of_transistors(vector<int> entry_of_PI, vector<string> minterms_b)
{
	int num_of_OR = entry_of_PI.size();
	int num_of_NOT = 0;
	vector <int> num_of_AND;
	for (int i = 0; i < entry_of_PI.size(); i++) {
		int num = 0;
		for (int j = 0; j < minterms_b[0].length(); j++) {
			if (minterms_b[entry_of_PI[i]][j] != '-')
				num++;
			if (minterms_b[entry_of_PI[i]][j] == '0')
				num_of_NOT++;
		}
		num_of_AND.push_back(num);
	}

	int n = 0;
	for (int i = 0; i < num_of_AND.size(); i++) {
		n += (2 * num_of_AND[i] + 2);
	}

	int cost = n + (2 * num_of_NOT) + (2 * num_of_OR + 2);

	return cost;
}

