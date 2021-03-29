#include <fstream>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

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
    res.push_back(dict.get(w));

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

int main() {

    vector<int> test = compress("compadre no compro coco");
    string ing = decompress(test);
    cout << ing;

    return 0;

}
