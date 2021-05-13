
#ifndef __BOARD_H__
#define __BOARD_H__

#include "player.h"
#include "vertex.h"
#include "tile.h"
#include "edge.h"
#include "exception.h"
#include "textdisplay.h"
#include <string>
#include <vector>
#include <queue>

class Board{
    //std::queue<int> order;
    int order;
    std::shared_ptr<TextDisplay> td;
    std::vector<std::shared_ptr<Vertex>> v;
    std::vector<std::shared_ptr<Edge>> e;
    std::vector<std::shared_ptr<Tile>> t;
    std::vector<std::shared_ptr<Player>> player;
    bool whetherIni;
    bool afterRoll;
    int geese;

    public:
    Board(bool whetherIni, int curTurn, std::vector<int> BInfo, std::vector<int> TInfo, std::vector<int> HInfo,
          std::vector<int> edgeInfo, std::vector<std::pair<int, int>> BoardInfo,
          std::vector<std::vector<int>> playerInfo, int geese);
    void buildRes(int housing);
    void improve(int housing);
    void buildRoad(int road);
    void trade(int color, int give, int take) noexcept;
    void checkTrade(int color, int give, int take);
    int roll() noexcept;
    void printBoard() noexcept;
    void printStatus() noexcept;
    void printResidence() noexcept;
    bool next() noexcept;
    void setDice(std::string method, int value = 2);
    void save(std::string file = "backup.sv") noexcept;
    void help(bool afterRoll) noexcept;

    void readInput(const std::string  string, bool& afterRoll);
    bool helpIni();
    bool findWhetherIni();
    bool mutateGeese(int loc);
    void steal(int loc, int victim);

    void market(int sell, int buy);
};

std::string lowerCase(std::string s);

int playerstoi(std::string s);

int sourcestoi(std::string s);
#endif
