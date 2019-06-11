#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include <regex>
#include <algorithm>
#include <string>
#include <cctype>
#include <set>
#include <iomanip>

class WordInfo {
private:
    unsigned int counter = 0;
    std::set<int> repeatLines;
public:
    void addRepeat(int line) {
        counter++;
        repeatLines.insert(line);
    }

    int getCounter() const {
        return  counter;
    }

    std::set<int> getRepeats() const {
        return repeatLines;
    }
};
typedef std::map<std::string, WordInfo> WCCollection;

bool isLink(std::string str);
inline std::string trim(const std::string &s);
void countWords(std::string fileName, WCCollection &wcc, std::vector<std::string> &links);
void outputData(WCCollection map, std::vector<std::string> links);
int getMaxCounter(WCCollection map);
unsigned int getMaxElementsCounter(WCCollection map);

const std::string Symbols = ",;.:-()\t!¡¿?\"[]{}&<>+-*/=#'„“–";

int main() {
    WCCollection wordsMap;
    std::vector<std::string> links;

    countWords("article.txt", wordsMap, links);
    outputData(wordsMap, links);

    return 0;
}

unsigned int getMaxElementsCounter(WCCollection map) {
    unsigned int max = 0;
    for (auto &pair : map) {
        if(pair.second.getRepeats().size() > max) {
            max = pair.second.getRepeats().size();
        }
    }

    return  max;
}

int getMaxCounter(WCCollection map) {
    int max = -9999999;
    for (auto &pair : map) {
        if(pair.second.getCounter() > max) {
            max = pair.second.getCounter();
        }
    }

    return  max;
}

inline std::string trim(const std::string &s)
{
    auto wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
    auto wsback=std::find_if_not(s.rbegin(),s.rend(),[](int c){return std::isspace(c);}).base();
    return (wsback<=wsfront ? std::string() : std::string(wsfront,wsback));
}

void toLowerCase(std::string &str)
{
    const int length = str.length();
    for(int i=0; i < length; ++i)
    {
        str[i] = std::tolower(str[i]);
    }
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
                toLowerCase(word);

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

void outputData(WCCollection wordsMap, std::vector<std::string> links) {
    std::ofstream output("output.txt");

    int maxCounter = getMaxCounter(wordsMap);
    unsigned int maxElementsCounter = getMaxElementsCounter(wordsMap);

    output << std::left << std::setw(40) << "Word" << std::setw(maxElementsCounter + 10) << "Repeats" << std::endl;
    output << std::string(50 + maxElementsCounter, '-') << std::endl;

    for ( int counter = 2; counter <= maxCounter; counter++) {
        WCCollection subMap;

        for(auto it = wordsMap.cbegin(); it != wordsMap.cend();)
        {
            if ((*it).second.getCounter() == counter) {
                subMap.insert(std::pair<std::string, WordInfo>((*it).first, (*it).second));

                wordsMap.erase(it++);

            } else {
                ++it;
            }

        }

        for (auto &val :subMap) {
            output  << std::left << std::setw(40) <<  val.first  + " (" + std::to_string(val.second.getCounter()) + ") repeats" ;
            for (auto line : val.second.getRepeats()) {
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
}

bool isLink(std::string str) {
    return std::regex_match(str, std::regex("https?:\\/\\/(www\\.)?[-a-zA-Z0-9@:%._\\+~#=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%_\\+.~#?&//=]*)")) ||
           std::regex_match(str, std::regex ("[-a-zA-Z0-9@:%._\\+~#=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%_\\+.~#?&//=]*)"));
}
