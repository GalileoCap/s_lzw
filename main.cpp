#include <fstream>
#include <streambuf>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

string read_decompressed_file(const string& file) {
   ifstream fin(file); fin >> noskipws;
   string str;
   fin.seekg(0, std::ios::end); str.reserve(fin.tellg()); fin.seekg(0, std::ios::beg);
   str.assign((istreambuf_iterator<char>(fin)), istreambuf_iterator<char>());
   return str;
}

void write_decompressed_file(const string& file, const string& out) {
    ofstream fout(file); fout << out;
}

vector<int> read_coded_file(const string& file) {
    vector<int> coded; int code;
    ifstream fin(file);
    while (!fin.eof()) { fin >> code; coded.push_back(code); }
    return coded;
}

void write_coded_file(const string& file, const vector<int>& coded) {
    ofstream fout(file);
    for (const int& code : coded) { fout << code << ' '; }
}

struct CompressDict {
    vector<string> keys; vector<int> values;
    int get(const string& key) { //U: Gets the value corresponding to key
        if (key.size() == 1) { //A: It's a char
            return key[0];
        } else { //A: It's in keys
            for (int i = 0; i < keys.size(); ++i) { //TODO: Binary search
                if (keys[i] == key) { return values[i]; }
            }
        }
        return -1;
    }
    void add(const string& key) { //U: Sets a value for key
        keys.push_back(key); values.push_back(128 + keys.size());
    }
};

vector<int> compress(const string& s) {
    vector<int> res;
    struct CompressDict dict;
    string w, wk;
    for (const char& k : s) {
        wk = ""; wk.append(w); wk.push_back(k);
        if (wk.size() == 1 || dict.get(wk) != -1) { //A: wk a char or in the dict
            w = wk;
        } else { //A: wk not in the dict
            res.push_back(dict.get(w));
            dict.add(wk); //A: Adds wk to the dictionary
            w = k;
        }
    }

    return res;
}

struct DecompressDict {
    vector<int> keys; vector<string> values;
    bool in(const int& key) { return key < (128 + keys.size()); } //U: Checks if the key is already in the dictionary
    string get(const int& key) { //U: Gets the value corresponding to key
        if (key < 128) { //A: It's a char
            return string(1, key);
        } else { //A: It's in keys
            for (int i = 0; i < keys.size(); ++i) { //TODO: Binary search
                if (keys[i] == key) { return values[i]; }
            }
        }
    }
    void add(const string& value) { //U: Sets a value for key
        values.push_back(value); keys.push_back(128 + values.size());
    }
};

string decompress(const vector<int>& coded) {
    string res;

    if (!coded.empty()) { //A: There's at least one code
        DecompressDict dict;
        int prev_code = coded[0], code; string c = dict.get(prev_code), pc = c, chain; res.append(c);
        for (int i = 1; i < coded.size(); ++i) {
            code = coded[i];
            chain = (dict.in(code) ? dict.get(code) : (pc + c));
            pc = c; c = chain[0]; dict.add(pc + c);
            res.append(chain);
        }
    }

    return res;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        cerr << "INVALID INPUT\nUSE -h FOR HELP" << endl;
        return 0;
    }

    bool extras = false, file = false, to_decompress = false, out = false, help = false;
    if (argc > 2) {
        for (const char &c : string(argv[1])) {
            extras |= c == '-';
            file |= c == 'f';
            to_decompress |= c == 'd';
            out |= c == 'o';
            help |= c == 'h';
        }
    }

    if (help) {
        //TODO: Help
    }

    if (!to_decompress) { //A: Compress
        string message;
        if (file) { //A: Read from file
            message = read_decompressed_file(argv[2]);
        } else { //A: Read from command line
            if (argc > 2) {
                message = argv[2];
            } else {
                message = argv[1];
            }
        }
        vector<int> coded = compress(message);

        if (out) {
            write_coded_file(argv[3], coded);
        } else {
            for (const int& code : coded) { cout << code << ' '; }
        }
    } else { //A: Decompress
        vector<int> coded;
        coded = read_coded_file(argv[2]);
        string message = decompress(coded);
        if (out) {
            write_decompressed_file(argv[3], message);
        } else {
            cout << message << endl;
        }
    }

    return 1;

}
