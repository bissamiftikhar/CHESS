#include<iostream>
#include"SFML/Graphics.hpp"
#include <SFML/Audio.hpp>

using namespace std;
bool piececapture;
bool kingincheck;
int arr[8][8];

bool isstalemate(int x2, int y2, int side);
bool ischeckmate(int x2, int y2, int side);
void initialize();
void display();
void findking(int& x, int& y, int side);
bool iskingundercheck(int x, int y, int x2, int y2);
bool checkmoves(int x, int y, int x2, int y2);
bool piecemove(int x, int y, int x2, int y2);
void promotingPawn( int x2, int y2);

class Pieces {
public:
    virtual bool Moves(int x, int y, int x2, int y2) = 0;
};

class Pawn : public Pieces {
public:
    bool Moves(int x, int y, int x2, int y2) {
        if (arr[x][y] == -2) //black
        {
            // Moving forward
            if (x2 == x + 1 && y2 == y && arr[x2][y2] == 0) {
                return true;
            }
            // Moving two steps from initial position
            if (x == 1 && x2 == x + 2 && y2 == y && arr[x + 1][y] == 0 && arr[x2][y2] == 0) {
                return true;
            }
            // Capturing diagonally
            if (x2 == x + 1 && (y2 == y + 1 || y2 == y - 1) && arr[x2][y2] > 0) {
                return true;
            }
        }
        else if (arr[x][y] == 2) // white
        {
            // Moving forward
            if (x2 == x - 1 && y2 == y && arr[x2][y2] == 0) {
                return true;
            }
            // Moving two steps from initial position
            if (x == 6 && x2 == x - 2 && y2 == y && arr[x - 1][y] == 0 && arr[x2][y2] == 0) {
                return true;
            }
            // Capturing diagonally
            if (x2 == x - 1 && (y2 == y + 1 || y2 == y - 1) && arr[x2][y2] < 0) {
                return true;
            }
        }
        return false;
    }
};
class Rook :public Pieces {
public:
    bool Moves(int x, int y, int x2, int y2) {
        // Rook can move either vertically or horizontally
        // Check if the move is vertical
        if ((arr[x][y] == -3 && arr[x2][y2] >= 0) || (arr[x][y] == 3 && arr[x2][y2] <= 0))
        {
            if (x == x2) {
                // Check if there are any pieces in between
                int minY = min(y, y2);
                int maxY = max(y, y2);
                for (int i = minY + 1; i < maxY; ++i) {
                    if (arr[x][i] != 0) {
                        // Piece blocking the path
                        return false;
                    }
                }
                // No pieces blocking the path, legal move
                return true;
            }
            // Check if the move is horizontal
            else if (y == y2) {
                // Check if there are any pieces in between
                int minX = min(x, x2);
                int maxX = max(x, x2);
                for (int i = minX + 1; i < maxX; ++i) {
                    if (arr[i][y] != 0) {
                        // Piece blocking the path
                        return false;
                    }
                }
                // No pieces blocking the path, legal move
                return true;
            }
        }
        // If neither vertical nor horizontal, invalid move
        return false;
    }

};
class Knight :public Pieces {
public:
    bool Moves(int x, int y, int x2, int y2) {
        if ((arr[x][y] == -4 && arr[x2][y2] >= 0) || (arr[x][y] == 4 && arr[x2][y2] <= 0)) {
            // Knight moves in an L shape: two squares in one direction and one square in a perpendicular direction

            // Calculate the absolute differences in x and y coordinates
            int dx = abs(x - x2);
            int dy = abs(y - y2);

            // Check if the move is a valid knight move
            if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) {
                // Check if the destination square contains a piece
                if ((arr[x2][y2] < 0 && arr[x][y] > 0) || (arr[x2][y2] > 0 && arr[x][y] < 0)) {
                    return true; // Legal move if destination square contains an enemy piece
                }
                else if (arr[x2][y2] == 0) {
                    return true; // Legal move if destination square is empty
                }

                return false; // Invalid move
            }
        }return false;
    }
};
class Bishop :public Pieces {
public:
    bool Moves(int x, int y, int x2, int y2) {
        if ((arr[x][y] == -5 && arr[x2][y2] >= 0) || (arr[x][y] == 5 && arr[x2][y2] <= 0)) {

            // Calculate the absolute differences in x and y coordinates
            int dx = abs(x - x2);
            int dy = abs(y - y2);

            // Check if the move is along a diagonal (dx must equal dy)
            if (dx == dy) {
                // Determine the direction of movement (up-left, up-right, down-left, down-right)
                int dirX = (x2 - x) > 0 ? 1 : -1;
                int dirY = (y2 - y) > 0 ? 1 : -1;

                // Check if there are any pieces in the bishop's path
                for (int i = 1; i < dx; ++i) {
                    if (arr[x + i * dirX][y + i * dirY] != 0) {
                        // Piece blocking the path
                        return false;
                    }
                }
                // Check if the destination square is either empty or contains an enemy piece
                if ((arr[x][y] > 0 && arr[x2][y2] < 0) || (arr[x][y] < 0 && arr[x2][y2] > 0) || arr[x2][y2] == 0) {
                    return true; // Legal move
                }
            }
            return false; // Invalid move
        }return false;
    }
};
class Queen :public Pieces {
public:
    bool Moves(int x, int y, int x2, int y2) {
        if ((arr[x][y] == -6 && arr[x2][y2] >= 0) || (arr[x][y] == 6 && arr[x2][y2] <= 0)) {
            // Check if the move is along a vertical or horizontal path (like a rook)
            if (x == x2 || y == y2) {
                // Perform rook-like move validation
                int minY = min(y, y2);
                int maxY = max(y, y2);
                int minX = min(x, x2);
                int maxX = max(x, x2);

                if (x == x2) {
                    // Check if there are any pieces in between (vertical move)
                    for (int i = minY + 1; i < maxY; ++i) {
                        if (arr[x][i] != 0) {
                            // Piece blocking the path
                            return false;
                        }
                    }
                }
                else {
                    // Check if there are any pieces in between (horizontal move)
                    for (int i = minX + 1; i < maxX; ++i) {
                        if (arr[i][y] != 0) {
                            // Piece blocking the path
                            return false;
                        }
                    }
                }
                // No pieces blocking the path, legal move
                return true;
            }

            // Check if the move is along a diagonal path (like a bishop)
            if (abs(x - x2) == abs(y - y2)) {
                // Perform bishop-like move validation
                int dx = (x2 - x) > 0 ? 1 : -1;
                int dy = (y2 - y) > 0 ? 1 : -1;

                for (int i = 1; i < abs(x - x2); ++i) {
                    if (arr[x + i * dx][y + i * dy] != 0) {
                        // Piece blocking the path
                        return false;
                    }
                }
                // Check if the destination square is either empty or contains an enemy piece
                if ((arr[x][y] > 0 && arr[x2][y2] < 0) || (arr[x][y] < 0 && arr[x2][y2] > 0) || arr[x2][y2] == 0) {
                    return true; // Legal move
                }
            }
            return false; // Invalid move
        }
        return false;
    }
};
class King :public Pieces {
public:
    bool Moves(int x, int y, int x2, int y2) {
        // Check if the move is within one square distance horizontally, vertically, or diagonally
        if (abs(x - x2) <= 1 && abs(y - y2) <= 1) {
            // Check if the destination square is either empty or contains an enemy piece
            if ((arr[x][y] > 0 && arr[x2][y2] < 0) || (arr[x][y] < 0 && arr[x2][y2] > 0) || arr[x2][y2] == 0) {
                return true; // Legal move
            }
        }
        return false; // Invalid move
    }
};


int main()
{
    int turn = 1;
    initialize();
    display(); cout << endl;
    cout << "*************** HOW TO PLAY **************" << endl;
    cout << "IN MENU use arrow keys to chooose between play and exit and press enter key" << endl;
    cout << " white moves first" << endl;
    cout << "  RIGHT CLICK to select the piece and LEFT CLICK where you want to move it " << endl << endl;
    cout << "*************** FEATURES **************" << endl;
    cout << "CHECK  ->  other pieces and king cannot move if king is under attack by any other opponent's piece" << endl;
    cout << "PIN  ->  it wont allow the movemnt of any piece if king comes in direct check from any opponent's piece after movement" << endl;
    cout << "CHECKMATE ->  if king is under check and it cannot move anywhere" << endl;
    cout << "STALEMATE ->  if king is not under check and there is no possible of move of king and any other piece,its a draw" << endl;
    cout << "PAWNPROMOTION ->  if pawn of any color reaches last row it can be changed into queen,bishop,rook or knight which is selected on choice at console" << endl << endl;
    cout << "*************** ADDITIONAL FEATURES **************" << endl;
    cout << "TIMER ->  different timer is set for black and white pieces,if time of any player is up window closes automatically.TIMER is set at 90 seconds which can be changed " << endl;
    cout << "SOUNDS ->  different souds for move,capture,check,checkmate and stale mate are played" << endl << endl;
    int check = 2;
    sf::RenderWindow window(sf::VideoMode(900, 600), "MENU");

    sf::Texture bg, start, exit, arrow;
    bg.loadFromFile("menubg.jpg");
    start.loadFromFile("start.png");
    exit.loadFromFile("exit.png");
    arrow.loadFromFile("arrow.png");

    sf::Sprite bgsprite(bg), startsprite(start), exitsprite(exit), arrowsprite(arrow);

    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = bg.getSize();
    float scX = static_cast<float>(windowSize.x) / textureSize.x;
    float scY = static_cast<float>(windowSize.y) / textureSize.y;
    bgsprite.setScale(scX, scY);

    sf::SoundBuffer capturebuffer;
    if (!capturebuffer.loadFromFile("capture.mp3")) {
        std::cerr << "Failed to load sound file." << std::endl;
        return 1;
    }
    // Create a sound and set its buffer
    sf::Sound capture;
    capture.setBuffer(capturebuffer);

    sf::SoundBuffer movebuffer;
    if (!movebuffer.loadFromFile("move.mp3")) {
        std::cerr << "Failed to load sound file." << std::endl;
        return 1;
    }
    // Create a sound and set its buffer
    sf::Sound move;
    move.setBuffer(movebuffer);

    sf::SoundBuffer checkmmatebuffer;
    if (!checkmmatebuffer.loadFromFile("checkmate.mp3")) {
        std::cerr << "Failed to load sound file." << std::endl;
        return 1;
    }
    // Create a sound and set its buffer
    sf::Sound checkmate;
    checkmate.setBuffer(checkmmatebuffer);

    sf::SoundBuffer drawbuffer;
    if (!drawbuffer.loadFromFile("draw.mp3")) {
        std::cerr << "Failed to load sound file." << std::endl;
        return 1;
    }
    // Create a sound and set its buffer
    sf::Sound draw;
    draw.setBuffer(drawbuffer);

    sf::SoundBuffer checkbuffer;
    if (!checkbuffer.loadFromFile("check.mp3")) {
        std::cerr << "Failed to load sound file." << std::endl;
        return 1;
    }
    // Create a sound and set its buffer
    sf::Sound checksound;
    checksound.setBuffer(checkbuffer);

    startsprite.setScale(0.4, 0.4);
    exitsprite.setScale(0.388, 0.32);
    arrowsprite.setScale(0.2, 0.2);
    startsprite.setPosition(400, 250);
    exitsprite.setPosition(400, 380);
    arrowsprite.setPosition(320, 300);

    sf::Font font;
    if (!font.loadFromFile("times new roman italic.ttf")) {
        // Handle font loading failure
        std::cerr << "Failed to load font" << std::endl;
        return 1;
    }

    sf::Text player1TimerText, player2TimerText;
    player1TimerText.setFont(font);
    player1TimerText.setCharacterSize(24);
    player1TimerText.setFillColor(sf::Color::Red);
    player1TimerText.setPosition(10, 10); // Adjust position as needed

    player2TimerText.setFont(font);
    player2TimerText.setCharacterSize(24);
    player2TimerText.setFillColor(sf::Color::Red);
    player2TimerText.setPosition(10, 100); // Adjust position as needed

    sf::Text Wwon;
    Wwon.setFont(font);
    Wwon.setString("CHECKMATE white won!!");
    Wwon.setCharacterSize(50);
    Wwon.setFillColor(sf::Color::Magenta);
    Wwon.setPosition(150, 300);

    sf::Text Bwon;
    Bwon.setFont(font);
    Bwon.setString("CHECKMATE black won!!");
    Bwon.setCharacterSize(50);
    Bwon.setFillColor(sf::Color::Magenta);
    Bwon.setPosition(150, 300);

    sf::Text stale;
    stale.setFont(font);
    stale.setString("STALEMATE draw!!");
    stale.setCharacterSize(50);
    stale.setFillColor(sf::Color::Magenta);
    stale.setPosition(150, 300);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //   Check boundary before moving the arrow sprite
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) && check % 2 == 0 && arrowsprite.getPosition().y < 380) {
            arrowsprite.move(0, 100);
            check++;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) && check % 2 == 1 && arrowsprite.getPosition().y > 250) {
            arrowsprite.move(0, -100);
            check++;
        }
        //   Handle Enter key to close the window
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
            window.close();
        }

        window.clear();
        window.draw(bgsprite);
        window.draw(exitsprite);
        window.draw(startsprite);
        window.draw(arrowsprite);
        window.display();
    }
    if (check % 2 == 0) {
        bool blu = false;
        bool isright = false; int xr, yr; int xl, yl;
        bool whitewon = false;
        bool blackwon = false;
        bool gamedraw = false;
        sf::RenderWindow newwindow(sf::VideoMode(800, 800), "MENU");
        sf::Texture board;
        board.loadFromFile("board.png");
        sf::Sprite boardsprite(board);

        sf::Texture blue;
        blue.loadFromFile("blue.png");
        sf::Sprite blues(blue);

        sf::Texture bp;
        bp.loadFromFile("bp.png");
        sf::Sprite bps(bp);

        sf::Texture wp;
        wp.loadFromFile("wp.png");
        sf::Sprite wps(wp);

        sf::Texture bk;
        bk.loadFromFile("bk.png");
        sf::Sprite bks(bk);

        sf::Texture bq;
        bq.loadFromFile("bq.png");
        sf::Sprite bqs(bq);

        sf::Texture br;
        br.loadFromFile("br.png");
        sf::Sprite brs(br);

        sf::Texture bn;
        bn.loadFromFile("bn.png");
        sf::Sprite bns(bn);

        sf::Texture bb;
        bb.loadFromFile("bb.png");
        sf::Sprite bbs(bb);

        sf::Texture wk;
        wk.loadFromFile("wk.png");
        sf::Sprite wks(wk);

        sf::Texture wq;
        wq.loadFromFile("wq.png");
        sf::Sprite wqs(wq);

        sf::Texture wn;
        wn.loadFromFile("wn.png");
        sf::Sprite wns(wn);

        sf::Texture wr;
        wr.loadFromFile("wr.png");
        sf::Sprite wrs(wr);

        sf::Texture wb;
        wb.loadFromFile("wb.png");
        sf::Sprite wbs(wb);

        sf::Vector2u wndowSize = newwindow.getSize();
        sf::Vector2u txtureSize = board.getSize();
        float saleX = static_cast<float>(wndowSize.x) / txtureSize.x;
        float saleY = static_cast<float>(wndowSize.y) / txtureSize.y;
        boardsprite.setScale(saleX, saleY);

        sf::Vector2u boardSize = board.getSize();
        int tileSize = boardSize.x / 8;
        float xx = 0.35, yy = 0.35;
        bps.setScale(xx, yy); // Black pawn sprite
        brs.setScale(xx, yy); // Black rook sprite
        bns.setScale(xx, yy); // Black knight sprite
        bbs.setScale(xx, yy); // Black bishop sprite
        bks.setScale(xx, yy); // Black king sprite
        bqs.setScale(xx, yy); // Black queen sprite

        wks.setScale(xx, yy); // White king sprite
        wqs.setScale(xx, yy); // White queen sprite
        wps.setScale(xx, yy); // White pawn sprite
        wrs.setScale(xx, yy); // White rook sprite
        wns.setScale(xx, yy); // White knight sprite
        wbs.setScale(xx, yy); // White bishop sprite

        blues.setScale(0.2, 0.2);

        // Initialize timers
        sf::Clock whiteTimer, blackTimer;
        sf::Time turnTime = sf::seconds(90); // 100 seconds per turn

        // Remaining time for each player's turn
        sf::Time player1RemainingTime = turnTime;
        sf::Time player2RemainingTime = turnTime;

        while (newwindow.isOpen())
        {
            sf::Event event;
            while (newwindow.pollEvent(event))
            {
                isright = false;

                if (event.type == sf::Event::Closed)
                {
                    newwindow.close();
                }
                //rightmouseclick
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Right) {
                        kingincheck = false;
                        piececapture = false;
                        blu = false;
                        sf::Vector2i mousePosR = sf::Mouse::getPosition(newwindow);
                        xr = mousePosR.x / 100;
                        yr = mousePosR.y / 100;
                        if (arr[yr][xr] != 0) { blu = true; }
                    }
                }
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {

                        sf::Vector2i mousePosL = sf::Mouse::getPosition(newwindow);
                        xl = mousePosL.x / 100;
                        yl = mousePosL.y / 100;
                        if (turn == 1 && arr[yr][xr] > 0) {
                                if (piecemove(yr, xr, yl, xl)) {
                                    if (ischeckmate(yl, xl, -1)) {
                                        cout << "CHECKMATE...WHITE WON" << endl;
                                        checkmate.play();
                                        whitewon = true;
                                        break;
                                    }
                                    if (isstalemate(yl, xl, -1)) {
                                        cout << "STALEMATE (draw) " << endl;
                                        draw.play();
                                        gamedraw = true;
                                        break;
                                    }
                                    if (yl == 0) {
                                        // Perform promotion for white pawn
                                        promotingPawn(yl, xl);
                                    }
                                    if (kingincheck) {
                                        checksound.play();
                                    }
                                    else if (piececapture) {
                                        capture.play();
                                    }
                                    else move.play();
                                    turn = -1;
                                }
                        }
                        else if (turn == -1 && arr[yr][xr] < 0) {
                                if (piecemove(yr, xr, yl, xl)) {
                                    if (ischeckmate(yl, xl, 1)) {
                                        cout << "CHECKMATE.....BLACK WON" << endl;
                                        checkmate.play();
                                        blackwon = true;
                                        break;
                                    }
                                    if (isstalemate(yl, xl, 1)) {
                                        cout << "STALEMATE (draw) " << endl;
                                        draw.play();
                                        gamedraw = true;
                                        break;
                                    }
                                    if (yl == 7) {
                                        promotingPawn( yl, xl);
                                    }
                                    if (kingincheck) {
                                        checksound.play();
                                    }
                                    else if (piececapture) {
                                        capture.play();
                                    }
                                    else move.play();
                                    turn = 1;
                                }
                            }
                        }
                    }
                }

                newwindow.clear();
                newwindow.draw(boardsprite);
                if (blu) {
                    blues.setPosition(100 * xr, 100 * yr);
                    newwindow.draw(blues);
                }
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        float posX = j * 100;
                        float posY = i * 100;
                        switch (arr[i][j]) {
                        case -2:
                            bps.setPosition(posX, posY);
                            newwindow.draw(bps);
                            break;
                        case -3:
                            brs.setPosition(posX, posY);
                            newwindow.draw(brs);
                            break;
                        case -4:
                            bns.setPosition(posX, posY);
                            newwindow.draw(bns);
                            break;
                        case -5:
                            bbs.setPosition(posX, posY);
                            newwindow.draw(bbs);
                            break;
                        case -7:
                            bks.setPosition(posX, posY);
                            newwindow.draw(bks);
                            break;
                        case -6:
                            bqs.setPosition(posX, posY);
                            newwindow.draw(bqs);
                            break;

                        case 7:
                            wks.setPosition(posX, posY);
                            newwindow.draw(wks);
                            break;
                        case 6:
                            wqs.setPosition(posX, posY);
                            newwindow.draw(wqs);
                            break;
                        case 2:
                            wps.setPosition(posX, posY);
                            newwindow.draw(wps);
                            break;
                        case 3:
                            wrs.setPosition(posX, posY);
                            newwindow.draw(wrs);
                            break;
                        case 4:
                            wns.setPosition(posX, posY);
                            newwindow.draw(wns);
                            break;
                        case 5:
                            wbs.setPosition(posX, posY);
                            newwindow.draw(wbs);
                            break;
                        }
                    }
                }

                // Define variables to store elapsed time for each player
                sf::Time White_elapsed, Black_elapsed;
                int player1Seconds = 0, player2Seconds = 0;

                if (turn == -1) {
                    // Measure elapsed time for player 1
                    White_elapsed = whiteTimer.getElapsedTime();
                    player1RemainingTime -= White_elapsed;

                    if (player1RemainingTime <= sf::Time::Zero) {
                        cout << "Player one's time is up" << endl;
                        cout<<"whitewon"<<endl;
                        newwindow.close();
                    }
                    whiteTimer.restart();
                    blackTimer.restart();
                }
                else
                {
                    // Measure elapsed time for player 2
                    Black_elapsed = blackTimer.getElapsedTime();
                    player2RemainingTime -= Black_elapsed;

                    if (player2RemainingTime <= sf::Time::Zero) {
                        cout << "Player two's time is up" << endl;
                        cout << "blackwon" << endl;
                        newwindow.close();
                    }
                    blackTimer.restart();
                    whiteTimer.restart();
                }

                // Update timer display
                player1Seconds = static_cast<int>(player1RemainingTime.asSeconds());
                player1TimerText.setString("Player 1: " + std::to_string(player1Seconds) + " seconds");

                player2Seconds = static_cast<int>(player2RemainingTime.asSeconds());
                player2TimerText.setString("Player 2: " + std::to_string(player2Seconds) + " seconds");
                if (whitewon)  newwindow.draw(Wwon);
                if (gamedraw) newwindow.draw(stale);
                if (blackwon) newwindow.draw(Bwon);
                // Draw timer texts
                newwindow.draw(player1TimerText);
                newwindow.draw(player2TimerText);
                newwindow.display();
            }
        }
    
    return 0;
}
void promotingPawn(int x2, int y2) {
    if (arr[x2][y2] == 2 || arr[x2][y2] == -2) {
        // Check if the pawn reaches the opposite end of the board
        if (x2 == 0 || x2 == 7) {
            cout << "Pawn promotion condition met for pawn at: " << x2 << ", " << y2 << endl;
            // Display options for promotion
            cout << "Choose promotion: (1) Queen, (2) Rook, (3) Bishop, (4) Knight" << endl;
            int choice;
            cin >> choice;

            // Promote the pawn based on player's choice
            switch (choice) {
            case 1:
                arr[x2][y2] = (arr[x2][y2] > 0) ? 6 : -6; // Queen
                break;
            case 2:
                arr[x2][y2] = (arr[x2][y2] > 0) ? 3 : -3; // Rook
                break;
            case 3:
                arr[x2][y2] = (arr[x2][y2] > 0) ? 5 : -5; // Bishop
                break;
            case 4:
                arr[x2][y2] = (arr[x2][y2] > 0) ? 4 : -4; // Knight
                break;
            default:
                cout << "Invalid choice. Promoting to Queen by default." << endl;
                arr[x2][y2] = (arr[x2][y2] > 0) ? 6 : -6; // Default to Queen
                break;
            }
            cout << "Pawn promoted to chosen piece at: " << x2 << ", " << y2 << endl;
        }
    }
}
bool isstalemate(int x2, int y2, int side) {
    int kingx;
    int kingy;
    findking(kingx, kingy, side);
    if (checkmoves(x2, y2, kingx, kingy)) {
        return false;
    }
    for (int i = kingx - 1; i <= kingx + 1; i++) {
        for (int j = kingy - 1; j <= kingy + 1; j++) {
            if (i >= 0 && j >= 0 && i <= 7 && j <= 7 && i != kingx && j != kingy) {
                if (checkmoves(kingx, kingy, i, j) && !iskingundercheck(kingx, kingy, i, j)) {
                    return false;
                }
            }
        }
    }
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (arr[x][y] * side > 0) {
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        if (checkmoves(x, y, i, j) && !iskingundercheck(x, y, i, j)) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}
bool ischeckmate(int x2, int y2, int side) {
    int kingx;
    int kingy;
    findking(kingx, kingy, side);
    if (!checkmoves(x2, y2, kingx, kingy)) {
        return false;
    }
    kingincheck = true;
    for (int i = kingx - 1; i <= kingx + 1; i++) {
        for (int j = kingy - 1; j <= kingy + 1; j++) {
            if (i >= 0 && j >= 0 && i <= 7 && j <= 7) {
                if (checkmoves(kingx, kingy, i, j) && !iskingundercheck(kingx, kingy, i, j)) {
                    return false;
                }
            }
        }
    }
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (arr[x][y] * side > 0) {
                for (int i = 0; i < 8; i++) {
                    for (int j = 0; j < 8; j++) {
                        if (checkmoves(x, y, i, j) && !iskingundercheck(x, y, i, j)) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}
void findking(int& x, int& y, int side) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (arr[i][j] == side * 7) {
                x = i;
                y = j;
            }
        }
    }
}
bool iskingundercheck(int x, int y, int x2, int y2) {
    //x and y rightclick ha(jo move krna ha)
    int side = 0;
    if (arr[x][y] > 0)  side = 1;
    else side = -1;
    int piece = arr[x][y];
    int destination = arr[x2][y2];
    arr[x2][y2] = arr[x][y];
    arr[x][y] = 0;
    int kingx;
    int kingy;
    findking(kingx, kingy, side);
    bool check = false;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (arr[i][j] * side < 0) {
                if (checkmoves(i, j, kingx, kingy)) {
                    check = true;
                    break;
                }
            }
        }
    }
    arr[x][y] = piece;
    arr[x2][y2] = destination;
    return check;
}
bool piecemove(int x, int y, int x2, int y2) {
    if (checkmoves(x, y, x2, y2) && !iskingundercheck(x, y, x2, y2)) {
        if (arr[x2][y2] != 0)piececapture = true;
        arr[x2][y2] = arr[x][y];
        arr[x][y] = 0;
        return true;
    }
    else {
        return false;
    }
}
bool checkmoves(int x, int y, int x2, int y2) {
    Pieces* piece;
    switch (arr[x][y]) {
    case 2:
    case -2:
        piece = new Pawn;
        break;
    case 3:
    case -3:
        piece = new Rook;
        break;
    case 4:
    case -4:
        piece = new Knight;
        break;
    case 5:
    case -5:
        piece = new Bishop;
        break;
    case 6:
    case -6:
        piece = new Queen;
        break;
    case 7:
    case -7:
        piece = new King;
        break;
    default:
        return false; // Invalid move
    }
    if (piece->Moves(x, y, x2, y2))
        return true;
    return false;
}
void initialize() {
    for (int j = 0; j < 8; j++) {
                arr[1][j] = -2;//    -2 for black pawn
                arr[6][j] = 2; //      2 for white pawn
            }
            //  NEGATIVE FOR BLACK, POSITIVE FOR WHITE  
            arr[0][0] = -3; arr[0][7] = -3;        arr[7][0] = 3; arr[7][7] = 3;//    3 for ROOK
            arr[0][1] = -4; arr[0][6] = -4;        arr[7][1] = 4; arr[7][6] = 4;//    4 for KNIGHT
            arr[0][2] = -5; arr[0][5] = -5;        arr[7][2] = 5; arr[7][5] = 5;//    5 for BISHOP
            arr[0][3] = -6;                        arr[7][3] = 6;               //    6 for QUEEN
            arr[0][4] = -7;                        arr[7][4] = 7;               //    7 for KING
}
void display() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cout << arr[i][j] << " ";
        }
        cout << endl;
    }
}