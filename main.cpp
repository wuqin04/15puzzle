#include <iostream>
#include <array>
#include <cassert>

#include "random.h"

constexpr int g_consoleLines{ 25 };

class Direction{
public:
    enum Type{
        up,
        down,
        left,
        right,
        maxDirections
    };

    Direction(Type type): m_type{ type }
    {}

    Direction operator-() const{
        switch (m_type) {
            case up:
                return Direction{ down };
            case down:
                return Direction{ up };
            case left:
                return Direction{ right };
            case right:
                return Direction{ left }; 

            default:
                break;
        }

        assert(0 && "Unsupported direction was passed!");
        return Direction{ up };
    }

    friend std::ostream& operator<<(std::ostream& out, Direction dir) {
        switch (dir.getType()) {
            case up:
                return (out << "up");
            case down:
                return (out << "down");
            case left:
                return (out << "left");
            case right:
                return (out << "right");

            default:
                break;
        }

        assert(0 && "Unsupported direction was passed!");
        return (out << "Unknown direction");
    }

    static Direction getRandomDirection() {
        Type random{static_cast<Type>(Random::get(0, maxDirections - 1)) };

        return Direction{ random };
    }

    Type getType() const{
        return m_type;
    }

private:
    Type m_type{};
};

struct Point{
    int x{};
    int y{};

    friend bool operator==(Point p1, Point p2) {
        return p1.x == p2.x && p1.y == p2.y;
    }

    friend bool operator!=(Point p1, Point p2) {
        return !(p1 == p2);
    }

    Point getAdjacentPoint(Direction dir) const{
        switch (dir.getType()) {
            case Direction::up:
                return Point{ x, y - 1 };
            case Direction::down:
                return Point{ x, y + 1 };
            case Direction::left:
                return Point{ x - 1, y };
            case Direction::right:
                return Point{ x + 1, y };
            
            default:
                break;
        }

        assert(0 && "Unsupported direction was passed!");
        return *this;
    }
};

namespace UserInput{

    bool isValidInput(char ch) {
        return ch == 'w' 
            || ch == 'a' 
            || ch == 's' 
            || ch == 'd' 
            || ch == 'q';
    }

    void ignoreLine() {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    char getCharacter() {
        char ch{};
        std::cin >> ch;
        ignoreLine();
        return ch;
    }
    
    char getCommandFromUser() {
        char input{};
        
        while (!isValidInput(input))
            input = getCharacter();

        return input;
    }

    Direction charToDirection(char ch) {
        switch (ch) {
            case 'w':
                return Direction{ Direction::up };
            case 'a':
                return Direction{ Direction::left };
            case 's':
                return Direction{ Direction::down };
            case 'd':
                return Direction{ Direction::right };
        }

        assert(0 && "Unsupported direction was passed!");
        return Direction{ Direction::up };
    }
}

class Tile{
private:
    int m_tileNum{ 0 };

public:
    Tile() = default;
    Tile(int tileNum)
        : m_tileNum{ tileNum } {}

    friend std::ostream& operator<<(std::ostream& out, Tile t) {
        if(t.getNum() > 9)
            out << " " << t.getNum() << " ";
        else if (t.getNum() > 0) 
            out << "  " << t.getNum() << " ";
        else if (t.getNum() == 0) 
            out << "    ";

        return out;
    }

    bool isEmpty() const{ return m_tileNum == 0; }

    int getNum() const{ return m_tileNum; }

};

class Board{
private:
    static constexpr int s_gridSize{ 4 };

    Tile m_tile[s_gridSize][s_gridSize]{
        { { 1 }, { 2 }, { 3 }, { 4 } },
        { { 5 }, { 6 }, { 7 }, { 8 } },
        { { 9 }, { 10 }, { 11 }, { 12 } },
        { { 13 }, { 14 }, { 15 }, { 0 } }
    };

public:
    Board() = default;

    friend std::ostream& operator<<(std::ostream& out, const Board& b) {

        for (int i{ 0 }; i < g_consoleLines; ++i) {
            out << '\n';
        }
        
        for (std::size_t i{ 0 }; i < s_gridSize; ++i) {
            for (std::size_t k{ 0 }; k < s_gridSize; ++k)    
                out << b.m_tile[i][k];
            out << '\n';
        }

        return out;
    }

    friend bool operator==(const Board& b1, const Board& b2) {
        for (int i{ 0 }; i < s_gridSize; ++i) {
            for (int k{ 0 }; k < s_gridSize; ++k) {
                if (b1.m_tile[i][k].getNum() != b2.m_tile[i][k].getNum())
                    return false;
            }
        }

        return true;
    }

    static bool isValidPoint(Point p) {
        return (p.x >= 0 && p.x < s_gridSize) 
            && (p.y >= 0 && p.y < s_gridSize);
    }

    Point emptyTile() const{
        for (int i{ 0 }; i < s_gridSize; ++i) 
            for (int k{ 0 }; k < s_gridSize; ++k) 
                if (m_tile[i][k].isEmpty()) 
                    return Point{ k, i };

        assert(0 && "There is no empty tiles!!");
        return Point{ -1, -1 };
    }

    void swapTile(Point p1, Point p2) {
        std::swap(m_tile[p1.y][p1.x], m_tile[p2.y][p2.x]);
    }

    bool moveTile(Direction dir) {
        Point getEmptyTile{ emptyTile() };
        Point adj{ getEmptyTile.getAdjacentPoint(-dir) };

        if (!isValidPoint(adj)) 
            return false;

        swapTile(adj, getEmptyTile);
        return true;
    }

    bool playerWon() const{
        static Board s_solved{};
        return s_solved == *this;
    }

    void randomize() {
        for (int i{ 0 }; i < 1000; ) {
            if (moveTile(Direction::getRandomDirection()))
                ++i;
        }
    }
};

int main() {
    Board board{};
    board.randomize();
    std::cout << board;

    std::cout << "Enter your command: ";

    while (!board.playerWon()) {
        char input{ UserInput::getCommandFromUser() };

        if (input == 'q') {
            std::cout << "\n\nBye!\n\n";
            break;
        }

        Direction dir{ UserInput::charToDirection(input) };

        bool userMoved{ board.moveTile(dir) };

        if (userMoved) 
            std::cout << board;
    }
}