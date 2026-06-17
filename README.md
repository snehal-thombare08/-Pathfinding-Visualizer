# 🗺️ Pathfinding Visualizer

An interactive pathfinding algorithm visualizer built with **C++** and **SFML 3.0** — draw walls, place start/end points, and watch 4 algorithms find the path in real-time!

![Pathfinding Visualizer](https://raw.githubusercontent.com/snehal-thombare08/-Pathfinding-Visualizer/main/Screenshot%202026-06-17%20143502.png)

## ✨ Features

- **4 Algorithms** — A*, Dijkstra, BFS, DFS
- **Interactive grid** — draw walls with mouse, erase with RMB
- **Move start/end** points anywhere on the grid
- **Random maze generator** — instant complex mazes
- **Animated exploration** — watch the algorithm search step by step
- **Color coded** — Blue: explored, Yellow: optimal path
- **80x50 grid** — 4000 cells for complex scenarios
- **Fast mode** — instant solve with F key

## 🚀 Download & Run

1. Download **PathfindingVisualizer-v1.0-Windows.zip**
2. Extract all files
3. Run **PathfindingVisualizer.exe**

## 🎮 Controls

| Key / Mouse | Action |
|-------------|--------|
| `LMB drag` | Draw walls |
| `RMB drag` | Erase walls |
| `S + LMB` | Move start point |
| `E + LMB` | Move end point |
| `Space` | Run algorithm |
| `1` | A* Search |
| `2` | Dijkstra |
| `3` | BFS (Breadth-First) |
| `4` | DFS (Depth-First) |
| `M` | Generate random maze |
| `C` | Clear grid |
| `F` | Fast mode |
| `Esc` | Quit |

## 📊 Algorithm Comparison

| Algorithm | Optimal Path? | Speed | Best For |
|-----------|--------------|-------|----------|
| A* | ✅ Yes | Fast | General use |
| Dijkstra | ✅ Yes | Medium | Weighted graphs |
| BFS | ✅ Yes | Medium | Unweighted grids |
| DFS | ❌ No | Fast | Exploration |

## 🛠️ Build from Source

**Requirements:** C++17, SFML 3.0, CMake, MinGW

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake -G "MinGW Makefiles"
mingw32-make
```

## 🧰 Tech Stack

- **Language:** C++17
- **Graphics:** SFML 3.0
- **Build:** CMake + MinGW
