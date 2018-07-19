#include <iostream>
#include<string>
#include <fstream>
#include <sstream>  
#include <vector>
#include <map>

using namespace std;
class Token {
public:
	string T_type;
	string T_expresion;
	vector<Token> TokenList;
	void tokenizer(string line);
	void print() {
		for (int i = 0; i<TokenList.size(); ++i) {
			cout << "type:" << TokenList[i].T_type << "  " << "expresion: " << TokenList[i].T_expresion << endl;
		}
	}
};
class Variable {
public:
	string V_name;
	string V_type;
	string V_value;
	map<string, Variable> VcatalogList;
	vector<Variable> VariableList;
	void print() {
		for (map<string, Variable>::iterator it = VcatalogList.begin(); it != VcatalogList.end(); ++it) {
		cout << it->first << " => " << it->second.V_value << '\n';
		}
	}
};

string checktType(string tok);
void openFile(const char *filename, ifstream &data);
void execution(Token &obj, Variable &db);
void parser(Token &obj, Variable &tmp);

void Token::tokenizer(string line) {
	string tok;
	string type;
	stringstream s(line);
	while (s >> tok) {
		type = checktType(tok);
		T_type = type;
		T_expresion = tok;
		TokenList.push_back(*this);
	}
}
int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cout << " There is no file name to read!" << std::endl;
		return -1;
	}
	ifstream data;
	openFile(argv[1], data);
	Variable db;
	string line;
	int i = 1;
	for (std::string line; getline(data, line); )
	{
		if (line == "")
			continue;
		Token tmp;
		tmp.tokenizer(line);
		execution(tmp, db);
		++i;
	}
	
}
void parser(Token &obj, Variable &db) {
	if (obj.TokenList[3].T_expresion[0] != '"' && obj.TokenList[3].T_expresion[obj.TokenList[3].T_expresion.size() - 1] != '"') {
		for (int i = 0; i < obj.TokenList[3].T_expresion.size(); ++i) {
			if (obj.TokenList[3].T_expresion[i] == '.') {
				db.V_type = "double";
				db.V_name = obj.TokenList[1].T_expresion;
				db.V_value = obj.TokenList[3].T_expresion;
				return;
			}
		}
		db.V_type = "integer";
		db.V_name = obj.TokenList[1].T_expresion;
		db.V_value = obj.TokenList[3].T_expresion;
	}
	else {
		db.V_type = "string";
		db.V_name = obj.TokenList[1].T_expresion;
		int size= obj.TokenList[3].T_expresion.size();
		string tmpstring= obj.TokenList[3].T_expresion.substr(1, (size-2));
		db.V_value = tmpstring;
	}
}

void parserupdate(Token &obj, Variable &db) {
	for (map<string, Variable>::iterator it = db.VcatalogList.begin(); it != db.VcatalogList.end(); ++it) {
		if (it->second.V_name == obj.TokenList[0].T_expresion) {
			if (obj.TokenList[2].T_expresion[0] != '"' && obj.TokenList[2].T_expresion[obj.TokenList[2].T_expresion.size() - 1] != '"') {
				for (int i = 0; i < obj.TokenList[2].T_expresion.size(); ++i) {
					if (obj.TokenList[2].T_expresion[i] == '.') {
						db.VcatalogList[it->second.V_name].V_type = "double";
						db.VcatalogList[it->second.V_name].V_value = obj.TokenList[2].T_expresion;
						return;
					}
				}
				db.VcatalogList[it->second.V_name].V_type = "integer";
				db.VcatalogList[it->second.V_name].V_value = obj.TokenList[2].T_expresion;
			}
			else {
				db.VcatalogList[it->second.V_name].V_type = "string";
				int size = obj.TokenList[2].T_expresion.size();
				string tmpstring = obj.TokenList[2].T_expresion.substr(1, (size - 2));
				db.VcatalogList[it->second.V_name].V_value = tmpstring;
			}
		}
	}
}

void execution(Token &obj, Variable &db) {
	Variable tmp;
	if (obj.TokenList[0].T_type == "keyword") {
		if (obj.TokenList[0].T_expresion == "pghvar") {
			parser(obj, tmp);
			db.VcatalogList[tmp.V_name] = tmp;
			db.VariableList.push_back(tmp);
			//db.print();
		}
		else if (obj.TokenList[0].T_expresion == "print") {
			for (map<string, Variable>::iterator it = db.VcatalogList.begin(); it != db.VcatalogList.end(); ++it) {
				if (it->first == obj.TokenList[1].T_expresion) {
					cout << it->second.V_value << '\n';
				}
			}
		}
		else {
			cout << "eror" << endl;
		}
	}
	else if (obj.TokenList[0].T_type == "variable") {
		parserupdate(obj , db);
	}
}
void openFile(const char *filename, ifstream &data) {
	std::string  fname(filename);
	std::string ext(".pgh");
	std::size_t found = fname.rfind(ext);
	if (found != fname.length() - 4) {
		std::cout << "Not correct file format!" << std::endl;
		return;
	}
	data.open(filename);
	if (data.fail())
	{
		cout << "FileError " << filename << " could not be found" << endl;
	}
}
string checktType(string tok) {
	if (tok == "pghvar" || tok == "print") {
		return "keyword";
	}
	else if (tok == "=" || tok == "+" || tok == "*" || tok == "/") {
		return "operator";
	}
	else if (tok == ";") {
		return "semicolon";
	}
	else {
		for (int i = 0; i < tok.size(); ++i) {
			if (tok[i] == '.' || (tok[0] == '"' && tok[tok.size() - 1] == '"') || ((int)tok[i] <= 57 && tok[i] >= 49)) {
				return "literal";
			}
		}
	}
	return "variable";
}
