//5 лаба
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <fstream>

using namespace std;

//хранение информации о документе
struct Document {
    int id;
    string url;
    string content;
    int word_count;
};

//поискового движка
class SearchEngine {
private:
    unordered_map<string, set<int>> word_index;  
    unordered_map<int, unordered_map<string, int>> word_frequency; //частота слов в документах
    unordered_map<int, Document> documents; 
    int next_id = 1;

    //функция для разделения строки на слова
    vector<string> split_words(const string& text) {
        vector<string> words;
        istringstream iss(text);
        string word;
        while (iss >> word) {
            //удаляем знаки препинания и приводим к нижнему регистру
            word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            if (!word.empty()) {
                words.push_back(word);
            }
        }
        return words;
    }

    //реализация сортировки слиянием по длине слова
    void merge_sort(vector<string>& words, int left, int right) {
        if (left >= right) return;

        int mid = left + (right - left) / 2;
        merge_sort(words, left, mid);
        merge_sort(words, mid + 1, right);

        //слияние
        vector<string> temp;
        int i = left, j = mid + 1;
        while (i <= mid && j <= right) {
            if (words[i].length() > words[j].length()) {
                temp.push_back(words[i++]);
            }
            else {
                temp.push_back(words[j++]);
            }
        }
        while (i <= mid) temp.push_back(words[i++]);
        while (j <= right) temp.push_back(words[j++]);

        for (int k = 0; k < temp.size(); k++) {
            words[left + k] = temp[k];
        }
    }

public:
    //добавление докум в поисковый движок
    void add_document(const string& url, const string& content) {
        Document doc;
        doc.id = next_id++;
        doc.url = url;
        doc.content = content;

        vector<string> words = split_words(content);
        doc.word_count = words.size();

        //индексация слов
        for (const string& word : words) {
            word_index[word].insert(doc.id);
            word_frequency[doc.id][word]++;
        }

        documents[doc.id] = doc;
    }

    //поиск по одному слову
    set<int> find_word(const string& word) {
        string lower_word = word;
        transform(lower_word.begin(), lower_word.end(), lower_word.begin(), ::tolower);

        if (word_index.find(lower_word) != word_index.end()) {
            return word_index[lower_word];
        }
        return {};
    }

    //поиск с AND
    set<int> find_and(const vector<string>& words) {
        if (words.empty()) return {};

        set<int> result = find_word(words[0]);

        for (size_t i = 1; i < words.size(); i++) {
            set<int> current = find_word(words[i]);
            set<int> temp;
            set_intersection(result.begin(), result.end(),
                current.begin(), current.end(),
                inserter(temp, temp.begin()));
            result = temp;
            if (result.empty()) break;
        }

        return result;
    }

    //поиск с OR
    set<int> find_or(const vector<string>& words) {
        set<int> result;

        for (const string& word : words) {
            set<int> current = find_word(word);
            result.insert(current.begin(), current.end());
        }

        return result;
    }

    //и поиск с NOT
    set<int> find_not(const string& word) {
        set<int> all_docs;
        for (const auto& pair : documents) {
            all_docs.insert(pair.first);
        }

        set<int> excluded = find_word(word);
        set<int> result;

        set_difference(all_docs.begin(), all_docs.end(),
            excluded.begin(), excluded.end(),
            inserter(result, result.begin()));

        return result;
    }

    //поиск, если два слова рядом
    set<int> phrase_search(const string& word1, const string& word2) {
        set<int> result;
        string w1 = word1;
        string w2 = word2;
        transform(w1.begin(), w1.end(), w1.begin(), ::tolower);
        transform(w2.begin(), w2.end(), w2.begin(), ::tolower);

        for (const auto& pair : documents) {
            const Document& doc = pair.second;
            vector<string> words = split_words(doc.content);

            for (size_t i = 0; i < words.size() - 1; i++) {
                if (words[i] == w1 && words[i + 1] == w2) {
                    result.insert(doc.id);
                    break;
                }
            }
        }

        return result;
    }

    //сортировка результатов по длине слова (самые длинные сначала)
    vector<string> sort_by_word_length(const vector<string>& words) {
        vector<string> sorted = words;
        merge_sort(sorted, 0, sorted.size() - 1);
        return sorted;
    }

    //получение информации о документе по ID
    Document get_document(int id) {
        return documents[id];
    }

    //получение всех документов
    unordered_map<int, Document> get_all_documents() {
        return documents;
    }
};

//функция для обработки пользовательского ввода
void process_command(SearchEngine& engine, const string& command) {
    istringstream iss(command);
    string cmd;
    iss >> cmd;

    if (cmd == "FIND") {
        string word;
        iss >> word;
        set<int> docs = engine.find_word(word);

        cout << "Documents containing '" << word << "':" << endl;
        for (int id : docs) {
            Document doc = engine.get_document(id);
            cout << "ID: " << id << ", URL: " << doc.url << endl;
        }
    }
    else if (cmd == "AND") {
        vector<string> words;
        string word;
        while (iss >> word) {
            words.push_back(word);
        }
        set<int> docs = engine.find_and(words);

        cout << "Documents containing all of:";
        for (const string& w : words) cout << " " << w;
        cout << ":" << endl;
        for (int id : docs) {
            Document doc = engine.get_document(id);
            cout << "ID: " << id << ", URL: " << doc.url << endl;
        }
    }
    else if (cmd == "OR") {
        vector<string> words;
        string word;
        while (iss >> word) {
            words.push_back(word);
        }
        set<int> docs = engine.find_or(words);

        cout << "Documents containing any of:";
        for (const string& w : words) cout << " " << w;
        cout << ":" << endl;
        for (int id : docs) {
            Document doc = engine.get_document(id);
            cout << "ID: " << id << ", URL: " << doc.url << endl;
        }
    }
    else if (cmd == "NOT") {
        string word;
        iss >> word;
        set<int> docs = engine.find_not(word);

        cout << "Documents not containing '" << word << "':" << endl;
        for (int id : docs) {
            Document doc = engine.get_document(id);
            cout << "ID: " << id << ", URL: " << doc.url << endl;
        }
    }
    else if (cmd == "PHRASE") {
        string word1, word2;
        iss >> word1 >> word2;
        set<int> docs = engine.phrase_search(word1, word2);

        cout << "Documents containing phrase '" << word1 << " " << word2 << "':" << endl;
        for (int id : docs) {
            Document doc = engine.get_document(id);
            cout << "ID: " << id << ", URL: " << doc.url << endl;
        }
    }
    else if (cmd == "SORT") {
        vector<string> words;
        string word;
        while (iss >> word) {
            words.push_back(word);
        }
        vector<string> sorted = engine.sort_by_word_length(words);

        cout << "Words sorted by length (longest first):" << endl;
        for (const string& w : sorted) {
            cout << w << " (" << w.length() << ")" << endl;
        }
    }
    else if (cmd == "LIST") {
        cout << "All documents:" << endl;
        for (const auto& pair : engine.get_all_documents()) {
            cout << "ID: " << pair.first << ", URL: " << pair.second.url << endl;
        }
    }
    else if (cmd == "HELP") {
        cout << "Available commands:" << endl;
        cout << "FIND word - Find documents containing the word" << endl;
        cout << "AND word1 word2 ... - Find documents containing all words" << endl;
        cout << "OR word1 word2 ... - Find documents containing any word" << endl;
        cout << "NOT word - Find documents not containing the word" << endl;
        cout << "PHRASE word1 word2 - Find documents with these words next to each other" << endl;
        cout << "SORT word1 word2 ... - Sort words by length (longest first)" << endl;
        cout << "LIST - List all documents" << endl;
        cout << "HELP - Show this help" << endl;
        cout << "EXIT - Exit the program" << endl;
    }
    else if (cmd == "EXIT") {
        exit(0);
    }
    else {
        cout << "Unknown command. Type HELP for available commands." << endl;
    }
}

//функция для загрузки документов из файлов
void load_documents(SearchEngine& engine) {
    //добавляем несколько документов с разными URL и содержимым
    engine.add_document("https://document.com/doc1",
        "I often think that the night is more alive and more richly colored than the day. Vincent Van Gogh");
    engine.add_document("https://document.com/doc2",
        "But eyes are blind. You have to look with the heart. Antoine de Saint-Exupery");
    engine.add_document("https://document.com/doc3",
        "A lady came up to me one day and said 'Sir! You are drunk', to which I replied 'I am drunk today madam, and tomorrow I shall be sober but you will still be ugly. Winston Churchill");
    engine.add_document("https://document.com/doc4",
        "We can and must write in a language which sows among the masses hate, revulsion, and scorn toward those who disagree with us. Vladimir Lenin");
    engine.add_document("https://document.com/doc5",
        "The darker the night, the brighter the stars, The deeper the grief, the closer is God! Fyodor Dostoevsky");
}

int main() {
    SearchEngine engine;
    load_documents(engine);

    cout << "Simple Search Engine. Type HELP for commands." << endl;

    while (true) {
        cout << "> ";
        string command;
        getline(cin, command);
        process_command(engine, command);
    }

    return 0;
}
