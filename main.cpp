#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include <regex>

class WordInfo {
private:
    std::vector<int> repeatLines;
public:
    void addRepeat(int line) {
        repeatLines.push_back(line);
    }

    int getCounter() {
        return  repeatLines.size();
    }

    std::vector<int> getRepeats() {
        return repeatLines;
    }
};
typedef std::map<std::string, WordInfo> WCCollection;

bool isLink(std::string str);
inline std::string trim(const std::string &s);
void countWords(std::string fileName, WCCollection &wcc, std::vector<std::string> &links);

const std::string Symbols = ",;.:-()\t!¡¿?\"[]{}&<>+-*/=#'„“–";

int main() {
    WCCollection wordsMap;
    std::vector<std::string> links;

    countWords("../article.txt", wordsMap, links);

    std::ofstream output("output.txt");

    for(auto elem : wordsMap)
    {
        if (elem.second.getCounter() > 1) {
            output <<  elem.first << " repeats " << elem.second.getCounter() << " times in these lines: " ;
            for (auto line : elem.second.getRepeats()) {
                output << line << " ";
            }
            output << std::endl;
        }
    }

    if (links.size() > 0 ) {
        output << "Found " << links.size() << " links:" << std::endl;
        for (auto link : links) {
            output << link << std::endl;
        }
    } else {
        output << "No links found.";
    }

    return 0;
}

inline std::string trim(const std::string &s)
{
    auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
    auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
    return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}
void countWords(const std::string fileName, WCCollection &wcc, std::vector<std::string> &links)
{
    std::ifstream input( fileName.c_str() );

    if ( input.is_open() ) {
        std::string line;
        std::string word;
        unsigned int lineIndex = 0;
        while( std::getline( input, line ) ) {
            std::istringstream filter( line );

            while( filter >> word ) {
                word = trim(word);
                if (isLink(word)) {
                    links.push_back(word);
                }
                else {
                    for (auto &it : word) { // process word
                        if ( Symbols.find(it) != std::string::npos ) {
                            it = ' ';
                        }
                    }

                    word = trim(word);

                    wcc[word].addRepeat(lineIndex);
                }

            }

            lineIndex++;
        }
    }

}

bool isLink(std::string str) {
    return std::regex_match(str, std::regex("https?:\\/\\/(www\\.)?[-a-zA-Z0-9@:%._\\+~#=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%_\\+.~#?&//=]*)")) ||
           std::regex_match(str, std::regex ("[-a-zA-Z0-9@:%._\\+~#=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%_\\+.~#?&//=]*)"));
}
