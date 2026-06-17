#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <stack>
#include <cmath>
#include <string>
#include <algorithm>
#include <functional>

const int COLS = 80, ROWS = 50;
const int CELL = 15;
const int WIDTH  = COLS * CELL;
const int HEIGHT = ROWS * CELL + 55;

enum NodeType { EMPTY, WALL, START, END, OPEN, CLOSED, PATH };

struct Node {
    NodeType type = EMPTY;
    float g = 1e9f, f = 1e9f;
    int px = -1, py = -1;
};

Node grid[ROWS][COLS];
int startX=2, startY=2, endX=COLS-3, endY=ROWS-3;

void resetGrid(){
    for(int r=0;r<ROWS;r++)
        for(int c=0;c<COLS;c++){
            if(grid[r][c].type!=WALL && grid[r][c].type!=START && grid[r][c].type!=END)
                grid[r][c]={EMPTY,1e9f,1e9f,-1,-1};
            else { grid[r][c].g=1e9f; grid[r][c].f=1e9f; grid[r][c].px=-1; grid[r][c].py=-1; }
        }
    grid[startY][startX].type=START;
    grid[endY][endX].type=END;
}

void clearAll(){
    for(int r=0;r<ROWS;r++)
        for(int c=0;c<COLS;c++)
            grid[r][c]={EMPTY,1e9f,1e9f,-1,-1};
    grid[startY][startX].type=START;
    grid[endY][endX].type=END;
}

// Random maze (recursive division simplified)
void generateMaze(){
    clearAll();
    srand((unsigned)time(0));
    for(int r=0;r<ROWS;r++)
        for(int c=0;c<COLS;c++){
            if(grid[r][c].type==START||grid[r][c].type==END) continue;
            if(rand()%3==0) grid[r][c].type=WALL;
        }
}

enum Algo { ASTAR, DIJKSTRA, BFS, DFS };
std::string algoNames[]={"A* Search","Dijkstra","BFS","DFS"};
Algo curAlgo = ASTAR;

// Step-based solver
struct SolverState {
    bool running=false, done=false, found=false;
    // A*/Dijkstra
    struct PQNode{ int x,y; float f; bool operator>(const PQNode& o)const{return f>o.f;}};
    std::priority_queue<PQNode,std::vector<PQNode>,std::greater<PQNode>> pq;
    // BFS
    std::queue<std::pair<int,int>> bfsQ;
    // DFS
    std::stack<std::pair<int,int>> dfsS;
    std::vector<std::vector<bool>> visited;
};
SolverState solver;

void initSolver(){
    resetGrid();
    solver={};
    solver.visited.assign(ROWS,std::vector<bool>(COLS,false));
    solver.running=true;
    grid[startY][startX].g=0;
    grid[startY][startX].f=0;
    if(curAlgo==ASTAR||curAlgo==DIJKSTRA){
        solver.pq.push({startX,startY,0.f});
    } else if(curAlgo==BFS){
        solver.bfsQ.push({startX,startY});
        solver.visited[startY][startX]=true;
    } else {
        solver.dfsS.push({startX,startY});
    }
}

int dx4[]={0,1,0,-1};
int dy4[]={-1,0,1,0};

float heur(int x,int y){
    return std::abs(x-endX)+std::abs(y-endY);
}

void reconstructPath(){
    int cx=endX,cy=endY;
    while(!(cx==startX&&cy==startY)){
        if(grid[cy][cx].type!=END) grid[cy][cx].type=PATH;
        int px=grid[cy][cx].px, py=grid[cy][cx].py;
        if(px<0||py<0) break;
        cx=px; cy=py;
    }
    solver.found=true;
}

bool solverStep(){
    if(!solver.running) return false;

    if(curAlgo==ASTAR||curAlgo==DIJKSTRA){
        if(solver.pq.empty()){solver.running=false;solver.done=true;return false;}
        auto [x,y,f]=solver.pq.top(); solver.pq.pop();
        if(grid[y][x].type==CLOSED) return true;
        if(grid[y][x].type!=START&&grid[y][x].type!=END) grid[y][x].type=CLOSED;
        if(x==endX&&y==endY){reconstructPath();solver.running=false;solver.done=true;return false;}
        for(int d=0;d<4;d++){
            int nx=x+dx4[d],ny=y+dy4[d];
            if(nx<0||nx>=COLS||ny<0||ny>=ROWS) continue;
            if(grid[ny][nx].type==WALL||grid[ny][nx].type==CLOSED) continue;
            float ng=grid[y][x].g+1.f;
            if(ng<grid[ny][nx].g){
                grid[ny][nx].g=ng;
                float h=(curAlgo==ASTAR)?heur(nx,ny):0.f;
                grid[ny][nx].f=ng+h;
                grid[ny][nx].px=x; grid[ny][nx].py=y;
                if(grid[ny][nx].type!=START&&grid[ny][nx].type!=END) grid[ny][nx].type=OPEN;
                solver.pq.push({nx,ny,grid[ny][nx].f});
            }
        }
    } else if(curAlgo==BFS){
        if(solver.bfsQ.empty()){solver.running=false;solver.done=true;return false;}
        auto [x,y]=solver.bfsQ.front(); solver.bfsQ.pop();
        if(grid[y][x].type!=START&&grid[y][x].type!=END) grid[y][x].type=CLOSED;
        if(x==endX&&y==endY){reconstructPath();solver.running=false;solver.done=true;return false;}
        for(int d=0;d<4;d++){
            int nx=x+dx4[d],ny=y+dy4[d];
            if(nx<0||nx>=COLS||ny<0||ny>=ROWS) continue;
            if(grid[ny][nx].type==WALL||solver.visited[ny][nx]) continue;
            solver.visited[ny][nx]=true;
            grid[ny][nx].px=x; grid[ny][nx].py=y;
            if(grid[ny][nx].type!=END) grid[ny][nx].type=OPEN;
            solver.bfsQ.push({nx,ny});
        }
    } else { // DFS
        if(solver.dfsS.empty()){solver.running=false;solver.done=true;return false;}
        auto [x,y]=solver.dfsS.top(); solver.dfsS.pop();
        if(solver.visited[y][x]) return true;
        solver.visited[y][x]=true;
        if(grid[y][x].type!=START&&grid[y][x].type!=END) grid[y][x].type=CLOSED;
        if(x==endX&&y==endY){reconstructPath();solver.running=false;solver.done=true;return false;}
        for(int d=0;d<4;d++){
            int nx=x+dx4[d],ny=y+dy4[d];
            if(nx<0||nx>=COLS||ny<0||ny>=ROWS) continue;
            if(grid[ny][nx].type==WALL||solver.visited[ny][nx]) continue;
            grid[ny][nx].px=x; grid[ny][nx].py=y;
            if(grid[ny][nx].type!=END) grid[ny][nx].type=OPEN;
            solver.dfsS.push({nx,ny});
        }
    }
    return true;
}

sf::Color nodeColor(NodeType t){
    switch(t){
        case EMPTY:  return sf::Color(22,26,38);
        case WALL:   return sf::Color(50,55,70);
        case START:  return sf::Color(50,220,100);
        case END:    return sf::Color(220,70,70);
        case OPEN:   return sf::Color(50,160,220);
        case CLOSED: return sf::Color(30,80,140);
        case PATH:   return sf::Color(255,210,50);
        default:     return sf::Color::Black;
    }
}

int main(){
    sf::RenderWindow window(sf::VideoMode({(unsigned)WIDTH,(unsigned)HEIGHT}),
        "Pathfinding Visualizer | LMB: Wall | RMB: Erase | S/E: Set Start/End | 1-4: Algo | Space: Run");
    window.setFramerateLimit(60);

    clearAll();

    bool placingStart=false, placingEnd=false;
    bool fast=false;
    int speed=8;

    sf::Font font;
    bool hasFont=font.openFromFile("C:/Windows/Fonts/arial.ttf");
    sf::Text hud(font);
    hud.setCharacterSize(14);
    hud.setFillColor(sf::Color::White);
    hud.setOutlineColor(sf::Color::Black);
    hud.setOutlineThickness(1.5f);
    hud.setPosition({4.f,(float)HEIGHT-42.f});

    sf::Text title(font);
    title.setCharacterSize(16);
    title.setFillColor(sf::Color(255,210,50));
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(1.5f);
    title.setPosition({4.f,4.f});

    sf::RectangleShape cell({(float)CELL-1.f,(float)CELL-1.f});

    while(window.isOpen()){
        auto mpos=sf::Mouse::getPosition(window);
        int mx=mpos.x/CELL, my=mpos.y/CELL;
        bool inGrid=(mx>=0&&mx<COLS&&my>=0&&my<ROWS);

        while(auto ev=window.pollEvent()){
            if(ev->is<sf::Event::Closed>()) window.close();
            if(auto* k=ev->getIf<sf::Event::KeyPressed>()){
                if(k->code==sf::Keyboard::Key::Escape) window.close();
                if(k->code==sf::Keyboard::Key::Space){ initSolver(); }
                if(k->code==sf::Keyboard::Key::C){ clearAll(); solver={}; }
                if(k->code==sf::Keyboard::Key::M){ generateMaze(); solver={}; }
                if(k->code==sf::Keyboard::Key::F) fast=!fast;
                if(k->code==sf::Keyboard::Key::S) placingStart=true;
                if(k->code==sf::Keyboard::Key::E) placingEnd=true;
                if(k->code==sf::Keyboard::Key::Num1){curAlgo=ASTAR;    resetGrid();solver={};}
                if(k->code==sf::Keyboard::Key::Num2){curAlgo=DIJKSTRA; resetGrid();solver={};}
                if(k->code==sf::Keyboard::Key::Num3){curAlgo=BFS;      resetGrid();solver={};}
                if(k->code==sf::Keyboard::Key::Num4){curAlgo=DFS;      resetGrid();solver={};}
                if(k->code==sf::Keyboard::Key::Up)   speed=std::min(60,speed+4);
                if(k->code==sf::Keyboard::Key::Down) speed=std::max(1,speed-4);
            }
            if(auto* k=ev->getIf<sf::Event::KeyReleased>()){
                if(k->code==sf::Keyboard::Key::S) placingStart=false;
                if(k->code==sf::Keyboard::Key::E) placingEnd=false;
            }
            if(auto* mb=ev->getIf<sf::Event::MouseButtonPressed>()){
                if(inGrid && mb->button==sf::Mouse::Button::Left){
                    if(placingStart){ 
                        grid[startY][startX].type=EMPTY;
                        startX=mx;startY=my;
                        grid[startY][startX].type=START;
                    } else if(placingEnd){
                        grid[endY][endX].type=EMPTY;
                        endX=mx;endY=my;
                        grid[endY][endX].type=END;
                    }
                }
            }
        }

        // Draw walls with mouse drag
        if(inGrid && !placingStart && !placingEnd){
            if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)){
                if(grid[my][mx].type==EMPTY) grid[my][mx].type=WALL;
            }
            if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)){
                if(grid[my][mx].type==WALL) grid[my][mx].type=EMPTY;
            }
        }

        int spf=fast?200:speed;
        if(solver.running)
            for(int i=0;i<spf;i++) if(!solverStep()) break;

        window.clear(sf::Color(12,14,22));

        for(int r=0;r<ROWS;r++){
            for(int c=0;c<COLS;c++){
                cell.setPosition({(float)c*CELL,(float)r*CELL});
                cell.setFillColor(nodeColor(grid[r][c].type));
                window.draw(cell);
            }
        }

        // Grid lines (subtle)
        sf::VertexArray lines(sf::PrimitiveType::Lines);
        sf::Color gc(30,35,50);
        for(int c=0;c<=COLS;c++){
            lines.append(sf::Vertex{sf::Vector2f((float)c*CELL,0),gc});
            lines.append(sf::Vertex{sf::Vector2f((float)c*CELL,(float)ROWS*CELL),gc});
        }
        for(int r=0;r<=ROWS;r++){
            lines.append(sf::Vertex{sf::Vector2f(0,(float)r*CELL),gc});
            lines.append(sf::Vertex{sf::Vector2f((float)COLS*CELL,(float)r*CELL),gc});
        }
        window.draw(lines);

        // HUD
        if(hasFont){
            std::string status;
            if(!solver.running&&!solver.done) status="Draw walls, then Space to run";
            else if(solver.running) status="Searching...";
            else if(solver.found) status="Path found! Length: "+std::to_string(
                [&](){int n=0;for(int r=0;r<ROWS;r++)for(int c=0;c<COLS;c++)if(grid[r][c].type==PATH)n++;return n;}()
            );
            else status="No path found!";

            title.setString(algoNames[curAlgo]+"  |  "+status);
            window.draw(title);

            hud.setString("LMB: Wall | RMB: Erase | S+LMB: Move Start | E+LMB: Move End | 1:A* 2:Dijkstra 3:BFS 4:DFS | Space: Run | M: Maze | C: Clear | F: Fast");
            window.draw(hud);
        }

        window.display();
    }
    return 0;
}
