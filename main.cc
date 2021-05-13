#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
#include <chrono>

#include "dice.h"
#include "exception.h"
#include "player.h"
#include "board.h"

bool isNumber(const std::string& s) {       // !!!!!if s is negative number, then this function returns false!!!!!!!
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](unsigned char c) { 

            return !std::isdigit(c); }) == s.end();
}

bool isInt(const std::string& s) {
   return !s.empty() && s.find_first_not_of("-.0123456789") == std::string::npos;
}

void randomFunction(std::vector<std::pair<int, int>>& boardInfo, int seed) {
    std::vector<int> resource = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5};
    std::vector<int> value =    {2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12};
    std::default_random_engine reg{seed};
    std::shuffle( resource.begin(), resource.end(), reg );
    std::shuffle( value.begin(), value.end(), reg );
    int fiveIndex;
    std::vector<int>::iterator it1 = find(resource.begin(), resource.end(), 5);
    fiveIndex = it1 - resource.begin();
    std::vector<int>::iterator it2 = find(value.begin(), value.end(), 7);
    // we erase the 5 and 7 in resource and value respectively
    resource.erase(it1);
    value.erase(it2);
    std::pair<int, int> pair57 = std::make_pair(5, 7);
    int time = 0;
    for (int i = 0; i < 18; i++) {  // note we must make a pair <5, 7>, so we just erase the sevenIndex in value;
        if (i == fiveIndex && time != -1) {
            time = -1;
            boardInfo.emplace_back(pair57);
            i--;
            continue;
        } else {
            boardInfo.emplace_back(std::make_pair(resource.at(i), value.at(i)));
        }
    }
   // std::cout << "boardsize" << boardInfo.size() << std::endl;
   // for (auto& n : boardInfo) {
      //  std::cout << n.first << " " << n.second << std::endl;
   // }
}

void BoardHelper(std::vector<std::pair<int, int>>& boardInfo, std::string fileName = "layout.txt") {
    std::string s;
    std::ifstream f {fileName};
    if ( f.fail() ) throw UnableOpen{fileName};      // cannot open in this case 
    getline(f, s);      // get the only one line that specifies the layout
    std::istringstream iss{s};
    std::vector<int> layoutContent;
    for (int i = 0; i < 38; i++) {      // check if has non-positive integer or non-integer
        int stringToInt;
        iss >> stringToInt;
        if ( iss.fail() || (stringToInt < 0) ) {
            throw LoadingWrong { };
        } else {
            layoutContent.emplace_back(stringToInt);    // transform these pairs to a vector of int
        }
    }
    for (int i = 0; i <= 36; i += 2) {
        int content = layoutContent.at(i);    // i is always an even number, representing the source type
        if (content == 5 && layoutContent.at(i+1) != 7) {
            throw LoadingWrong {};
        } else if (content > 5) {   // sourceType cannot beyond 5 (PARK)
            throw LoadingWrong {};
        } else if (layoutContent.at(i+1) == 7 && content != 5) {
            throw LoadingWrong {};
        } else if (layoutContent.at(i+1) > 12) {  // cannot have a tile value exceeding 12
            throw LoadingWrong {};
        }
    }
    /// if we reach here, then it means the layout is valid, we need to mutate boardInfo
    for (int i = 0; i <= 36; i += 2) {
        int resource = layoutContent.at(i);
        int value = layoutContent.at(i+1);
        boardInfo.emplace_back(std::make_pair(resource, value));
    }
   // for (auto& n : boardInfo) {
      //  std::cout << n.first << " " << n.second << " ";
    //}
   // std::cout << std::endl;
}

void loadHelper(std::vector<std::pair<int, int>>& boardInfo, std::vector<int>& BInfo, int& geese, bool& whetherIni, std::string fileName,
            std::vector<int>& TInfo,  std::vector<int>& HInfo, std::vector<int>& edgeInfo, std::vector<std::vector<int>>& playerInfo, int& currTurn) {
    std::string c;
    std::string everyLine;
    FormatIncorrect formatIncorrect{fileName};      // called FormatIncorrect exception constructor
    UnableOpen openfailure{fileName};   // called UnableOpen exception constructor
    unsigned int curTurn;
    std::ifstream f{fileName};
    //  std::cout << argv[i] << std::endl;
    // std::cout << "open ok" << std::endl;
    if (f.fail()) throw openfailure;        // cannot the target Openfile
    std::string s;
    // std::cout << "try read fist line" << std::endl;
    getline(f, s);     // read the first line of the saving file(current Turn)
    // f >> s
    // std::cout << s << std::endl;
    std::istringstream iss{s};
    iss >> curTurn;
    // std::cout << "currTurn: " << curTurn << std::endl;
    if (iss.fail()) {      // currTurn is an unsigned int, so throws by negative int, or non-integer format
        throw formatIncorrect; 
    }
    currTurn = curTurn;
    iss.str(std::string());     // clear iss

    int times = 0;
   //  std::cerr << "before getline" << std::endl;
    while (std::getline(f, everyLine)) {
        
        std::istringstream iss {everyLine};        // set stringstream
        // f >> everyLine;
        //std::cout << everyLine << std::endl;
        // std::cout << iss.str() << std::endl;
        if (times <= 3) { 
            //  std::cout << "less than 3: " << times << std::endl;
            int sourceNum;
            for (int j = 0; j < 5; ++j) {  // we read the first five resourceInfo in each player row
            // std::cout << "enter sourceloop" << std::endl;
                iss >> sourceNum;
                // std::cout << "sources: " << sourceNum << std::endl;
                if (iss.fail()) {
                    //  std::cout << "times: " << times << std::endl;
                    // std::cout << "sources: " << sourceNum << std::endl;
                    throw formatIncorrect;
                } else {
                    //  std::cout << "mutate playerInfo" << std::endl;
                    playerInfo.at(times).at(j) = sourceNum;
                    // std::cout << "increase times" << std::endl;     
                }
                // times++;
            }
            //// if we reach here, we successfully mutate playerInfo, and we need to look at edge, basement, house, tower info
            // std::cout << "begin" << std::endl;
            iss >> s;
            // std::cout << "search road" << std::endl;
            if (s == "r") {
                //std::cout << "there is a r type" << std::endl;
                iss >> s;     // read next string
                while (s != "h") {      // if s = "h", then we look for house pairs, don't enter this while loop
                    int road;
                    std::istringstream ss{s};
                    ss >> road;
                    if (road < 0 || !isNumber(s)) throw formatIncorrect;
                    // if (ss.fail()) throw formatIncorrect;
                    else {
                        // std::cout << "h times: " << times << std::endl;
                        edgeInfo.at(road) = times;      // mutate the edge[road] as the player(owner)
                    }
                    iss >> s;   
                }
                // s = "h"      // try to read the next string (after 'h', maybe oneof B, H, T)
                unsigned int bhtPos;
                // std::cout << "enter BHT" << std::endl;
                while (iss >> s) {
                    // std::cout << "s value " << s << std::endl;
                    iss >> c;
                    // std::cout << "c value " << c << std::endl;
                    if (c != "B" && c != "H" && c!= "T") {
                        throw formatIncorrect;
                    }
                    else if (c == "B") {      // Basement
                        std::istringstream sss{s};
                        sss >> bhtPos;
                        if (bhtPos < 0 || !isNumber(s)) {       // not a valid positive integer
                            throw formatIncorrect;
                        } else {
                            BInfo.at(bhtPos) = times;
                        }
                    } else if (c == "H") {
                        std::istringstream sss{s};
                        sss >> bhtPos;
                        if (bhtPos < 0 || !isNumber(s)) {       // not a valid positive integer
                            throw formatIncorrect;
                        } else {
                            HInfo.at(bhtPos) = times;
                        }
                    } else if (c == "T") {
                        std::istringstream sss{s};
                        sss >> bhtPos;
                        if (bhtPos < 0 || !isNumber(s)) {       // not a valid positive integer
                            throw formatIncorrect;
                        } else {
                            TInfo.at(bhtPos) = times;
                        }
                    }
                    c = " ";
                }
            }
            times++;
            //  std::cout << "times: " << times << std::endl;
            continue;
        }
        // std::cout << "now : " << times << std::endl;
        else if (times == 4) {
            times ++;       /****************/
            std::string ss;
            std::vector<std::string> boardRow;
            while (iss >> ss) {
                boardRow.emplace_back(ss);
            }
            // std::cout << boardRow.size() << std::endl;
            if (boardRow.size() != 38) throw formatIncorrect;   // must exactly 38 string, 19 pairs
            //  std::cout << "how " << std::endl;
            /// now we check the entire 19 pairs if valid
            /* std::cout << std::endl;
            for(auto& n : boardRow) {
                std::cout << n << " ";
            } 
            std::cout << std::endl; */
            for (int i = 0; i < 38; i++) {      // check if has non-positive integer or non-integer
                std::istringstream iss{boardRow.at(i)};
                int stringToInt;
                iss >> stringToInt;
                if ( iss.fail() || (stringToInt < 0) ) {
                    times++;
                    throw formatIncorrect;
                }
            }
           
            for (int i = 0; i <= 36; i += 2) {
            // std::cout << "enter loop" << std::endl;
                int content = std::stoi(boardRow.at(i));    // i is always an even number, representing the source type
                if (content == 5 && std::stoi(boardRow.at(i+1)) != 7) {
                    throw LoadingWrong {};
                } else if (content > 5) {   // sourceType cannot beyond 5 (PARK)
                    throw formatIncorrect;
                } else if (std::stoi(boardRow.at(i+1)) == 7 && content != 5) {
                    throw formatIncorrect;
                } else if (std::stoi(boardRow.at(i+1)) > 12) {  // cannot have a tile value exceeding 12
                    throw formatIncorrect;
                }
            }
            
            // if we reach here, it means the boardRow is valid, thus we need to mutate our boardInfo
            for (int i = 0; i <= 36; i += 2) {
                int resource = std::stoi(boardRow.at(i));
                int value = std::stoi(boardRow.at(i+1));
                boardInfo.emplace_back(std::make_pair(resource, value));
                // std::cout << " suceesfully mutate boardInfo" << std::endl;
            }
            // std::cout << "everyLine2: " << everyLine << std::endl;
            // continue;
        } else {
            //std::cout << "everyLine1: " << everyLine << std::endl;
            //std::cout << "times are: " << times << std::endl;
            //std::cout << "enter geese " << std::endl;
            int actualGeese = geese;
            iss >> actualGeese;
            //std::cout << "actual geese: " << actualGeese << std::endl;
            //std::cout << "geese: " << geese << std::endl;
            if (actualGeese < 0 || actualGeese > 18) throw formatIncorrect; 
            else geese = actualGeese;
        // std::cout << "geese" << std::endl;
            //std::cout << geese << std::endl;
            break;
        }
    }
    if (times != 5) throw formatIncorrect;
  //  std::cout << "timesssss" << times << std::endl;
    whetherIni = true;      /*************/
  //  std::cout << "why" << std::endl;
  //  std::cout << "boardinfo.size: " << boardInfo.size() << std::endl;
   // for (auto& n : boardInfo) {
   //     std::cout << n.first << " " << n.second << " ";
  //  }
  //  std::cout << std::endl;
}

//ERROR: try.txt has an invalid format.
int main(int argc, char* argv[]) {
    int loadNum = 0;
    int boardNum = 0;

    bool haveBoard = false;
   // bool haveBoardArgs = false;

    bool haveRandomBoard = false;

    bool haveSeed = false;
    int seedNumber = 0;     // set default seed number, may mutate if -seed is used
    bool haveSeedArgs = false;
    int haveSeedNum = 0; 

    bool haveLoad = false;      // stores four boolean values indicating whether previously have read //
   // bool haveLoadArgs = false;

    
    bool whetherIni = false;        // when load, mutate as true;
    int currTurn = 0; // who will start the next round
    std::vector<int> BInfo; // if there is a basement in the position,
    std::vector<int> TInfo; // if there is a tower in the position,
    std::vector<int> HInfo; // if there is a house in the position.
    std::vector<int> edgeInfo;
    std::vector<std::pair<int, int>> boardInfo; 
    std::vector<std::vector<int>> playerInfo;   // every player's resource
    int geese = -1; 
    for (int i = 0; i < 54; ++i){
        BInfo.emplace_back(-1);
        TInfo.emplace_back(-1);
        HInfo.emplace_back(-1);
    }

    for (int i = 0; i < 72; ++i){
        edgeInfo.emplace_back(-1);
    }

    for (int length = 0; length < 4; ++length) {
        std::vector<int> temp;
        for (int j = 0; j < 5; ++j){
            temp.emplace_back(0);
        }
        playerInfo.emplace_back(temp);
    }
    //////////////////////////
    int i = 1;      // 0 index is the executable file name
   // bool haveArgs = false;
    if (argc == 1) {
        BoardHelper(boardInfo);
    } else {
        try {
            while (i < argc) {
                std::string temp = argv[i];
                if (temp == "-load") {
                  //  std::cout << "enter -load if" << std::endl;
                   // std::cout << "when enter load if, i value: " << i << std::endl;
                    loadNum++;
                    haveLoad = true;

                    if (haveBoard == true) {    // previously have -board argument
                        throw BoardAlreadySpecify{temp};
                    }
                    if (loadNum >= 2) throw AlreadySpecify{temp};
                    if (haveRandomBoard == true) {
                        std::cerr << "ERROR: previous -random-board being ignored in favour of -load" << std::endl;
                        if (loadNum >= 2) throw AlreadySpecify{temp};
                        if (i + 1 == argc) throw LoadMissFile{};    // -load with no following argument
                    }

                    if (argc <= 2 || i == argc - 1) throw LoadMissFile{};
                    if (loadNum >= 2) throw AlreadySpecify{temp};
                   // std::cout << "111111" << std::endl;
                   // std::cout << "i value with load " << i << std::endl;
                    std::string fileName = argv[i+1];
                    loadHelper(boardInfo, BInfo, geese, whetherIni, fileName, TInfo, HInfo, edgeInfo, playerInfo, currTurn);
                   // haveLoadArgs = true;        // if we reach here, then the file is able to open
                  // std::cout << "load file nice" << std::endl;
                           ////
                    if (haveSeed == true && haveSeedArgs == false) {
                        throw SeedMissNumber{};
                    }
                    i++;
                } else if (temp == "-board") {
                  //  std::cout << "when enter -board if, i value: " << i << std::endl;
                  //  std::cout << "enter -board if" << std::endl;
                    boardNum++;
                    haveBoard = true;
                    if (haveLoad == true) {     // previously have load
                        throw LoadAlreadySpecify{temp};
                    }
                    if (argc <= 2 || i + 1 == argc) throw BoardMissFile{};
                    if (boardNum >= 2) throw AlreadySpecify{temp};

                    if (haveRandomBoard == true) {
                        std::cerr << "ERROR: previous -random-board being ignored in favour of -board" << std::endl;
                       // std::cout << "i value " << i << std::endl;
                        if (boardNum >= 2) throw AlreadySpecify{temp};
                        if (i + 1 == argc) throw BoardMissFile{}; 
                    }

                     if (haveSeed == true && haveSeedArgs == false) {
                        throw SeedMissNumber{};
                    }
                    std::string fileName = argv[i+1];
                    BoardHelper(boardInfo, fileName);
                  //  haveBoardArgs = true;   // if we reach here, then file is able to open

                    i++;
                } else if (temp == "-random-board") {
                    haveRandomBoard = true;
                 //   std::cout << "enter -random-board if" << std::endl;
                    if (argc == 2) {        // just one -random-board as command-line argument
                        unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();    ////
                        randomFunction(boardInfo, seed);
                    }
                    if (haveLoad == true) {
                        std::cerr << "ERROR: -random-board being ignored in favour of previous -load" << std::endl; 
                    }

                    if (haveBoard == true) {
                         std::cerr << "ERROR: -random-board being ignored in favour of previous -board" << std::endl;
                    }

                    if (haveSeed == true && haveSeedArgs == false) {
                        throw SeedMissNumber{};
                    } else if (haveSeed == true && haveSeedArgs == true) {
                        randomFunction(boardInfo, seedNumber);
                    } else {
                        unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();    ////
                        randomFunction(boardInfo, seed);
                    }
                } else if (temp == "-seed") {
                    haveSeed = true;
                    haveSeedNum++;
                    if (argc <= 2) {        // just one -seed but followed by nothing
                        throw SeedMissNumber{};
                    }
                    if (haveSeedNum >= 2) throw AlreadySpecify{temp};

                    std::string seedstring = argv[i+1];
                  //  std::cout << "catch next arg of -seed" << std::endl;
                    if (isInt(seedstring)) {
                       // std::cout << " vavlid numeber" << std::endl;
                        seedNumber = std::stoi(seedstring);
                        haveSeedArgs = true;
                        i += 1;
                       // std::cout << "i value " << i << std::endl;
                    } else {
                        throw SeedMissNumber{};
                    }

                    if (haveSeedArgs == true && argc == 3) {    // for case ** -seed 10 **
                       /// std::cout << "generate seed board" << std::endl;
                        randomFunction(boardInfo, seedNumber);
                    }

                    if (haveRandomBoard == true && haveSeedArgs == true) {
                        randomFunction(boardInfo, seedNumber);
                    }
                } else {        // the command-line option is invalid
                    throw RandomBoardMiss{temp};
                }
                i += 1;     /////
            }
        } catch (BaseException& b) {
            b.print();
            return 1;
        }
    }

    bool keepPlay = true;
    bool firstTime = true;
    while (keepPlay){
        if (! firstTime){                                      // set everything to default
            whetherIni = false;
            currTurn = 0;
            for (int i = 0; i <= 53; ++i){
                BInfo[i] = -1;
                TInfo[i] = -1;
                HInfo[i] = -1;
            }

            for (int i = 0; i <= 71; ++i){
                edgeInfo[i] = -1;
            }

            for (int length = 0; length <= 3; ++length){
                for (int j = 0; j <= 4; ++j){
                    playerInfo[length][j] = 0;
                }
            }
            std::vector<std::pair<int, int>> newboardInfo;
            BoardHelper(newboardInfo);
            geese = -1;
            boardInfo = newboardInfo;
        }
        
        auto game = std::make_unique<Board>(whetherIni, currTurn, BInfo, TInfo, HInfo, edgeInfo, boardInfo, playerInfo, geese);

        
        if (game.get()->findWhetherIni() == false){
            if (game.get()->helpIni()){  // control + d
                return 0;
            }
        }
        std::string input;
        bool someWin = false;
        game.get()->printBoard();
        std::cout << "Builder " << changePlayerName(currTurn % 4) << "'s turn." << std::endl;
        std::cout << "> ";
        while (std::cin >> input && (someWin == false)){
            game.get()->readInput(input, someWin);
        }

        if (someWin == false){   // control + D ends
            std::cout << "End of file reached." << std::endl;
            std::cout << std::endl;
            game.get()->save();
            keepPlay = false;
        } else {                 // someone wins
            std::string answer;
            std::cin >> answer;
            while (true) {
                std::cout << "Would you like to play again?" << std::endl;
                if (answer == "yes" || answer == "y"){
                    keepPlay = true;
                    firstTime = false;
                    break;
                } else if (answer == "no" || answer == "n"){
                    keepPlay = false;
                    break;
                } else {
                    std::cout << "Invalid command." << std::endl;
                    std::cout << std::endl;
                }
            }
        }
    }
}
