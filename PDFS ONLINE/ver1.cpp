#include <iostream>
#include <vector>
#include <stack>
#include <utility>
#include <algorithm>
#include <cstring>
using namespace std;


struct Step {
    int x, y, e, d;
};

int n, m, E;
int a[100][100], d[101][101] = {0};
int dx[4] = {0, -1, 0, 1};
int dy[4] = {-1, 0, 1, 0};
int sx = -1, sy = -1;
bool vis[100][100] = {0};
int curE = 0, px = -1, py = -1;

pair<int, int> par[100][100];

bool newRoute = true;
vector<Step> path[100];
int cnt = 0;

stack<pair<int, int>> stL, stU, stR, stD;

void printD();
void save(int x, int y);
void upd(int x, int y, int nx, int ny);
void go(int x, int y);
void run();

int main() {

  //  auto start = std::chrono::high_resolution_clock::now(); // Bắt đầu đo thời gian

    freopen("T1.inp", "r", stdin);
   // freopen("T4.out", "w", stdout);
    cin >> n >> m >> E;
    if (E % 2 != 0) E--; 
    
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            cin >> a[i][j];
            if (a[i][j] == 2) sx = i, sy = j;
        }

    path[cnt].push_back({sx, sy, curE, d[sx][sy]});
    stL.push({sx, sy});
    run();
    printD();


    // auto end = std::chrono::high_resolution_clock::now(); // Kết thúc đo thời gian
    // std::chrono::duration<double> diff = end - start;
    // cout << "Time taken: " << diff.count() << " s\n";
    return 0;
}

void printD() {
    cout<<"Number of routes: " << cnt + 1 << endl;

    for (int i = 0; i <= cnt; i++) {
        cout << "Route " << i + 1 << endl;
        for (const auto& s : path[i])
            cout << "x=" << s.x << " y=" << s.y << " d=" << s.d << " e=" << s.e << endl;
    }
    cout<<"======================"<<endl;
    int total_steps = 0;
    for ( int i =0; i<=cnt;i++){
       auto it = path[i][path[i].size()-1];
        total_steps += it.e + it.d;
    }
    cout<<"Total steps: " << total_steps << endl;
}

void save(int x, int y) {
    
    path[cnt].clear();
    pair<int, int> p = {x, y};
    int e = curE;
    path[cnt].push_back({p.first, p.second, e, d[p.first][p.second]});
    while (p.first != sx || p.second != sy) {
        e--;
        p = par[p.first][p.second];
        path[cnt].push_back({p.first, p.second, e, d[p.first][p.second]});
    }
    reverse(path[cnt].begin(), path[cnt].end());
}

void upd(int x, int y, int nx, int ny) {
    if (d[nx][ny] == 0) {
        if (d[x][y] + 1 > E / 2) return;
        d[nx][ny] = d[x][y] + 1;
        par[nx][ny] = {x, y};
    } else if (d[nx][ny] > d[x][y] + 1) {
        d[nx][ny] = d[x][y] + 1;
        par[nx][ny] = {x, y};
    } else if (d[nx][ny] == d[x][y] + 1) {
        par[nx][ny] = {x, y};
    }
}

void go(int x, int y) {
    //path[cnt].push_back({x, y, curE, d[x][y]});

    if (!newRoute) {
        curE = d[x][y];
        newRoute = true;
        save(x, y);
    } else {
        int stepE = abs(d[px][py] - d[par[x][y].first][par[x][y].second]) + 1;
        if (x != sx || y != sy) {
            if (curE + stepE + d[x][y] == E) {
                curE += stepE;
                path[cnt].push_back({x, y, curE, d[x][y]});
                newRoute = false;
                cnt++;
            } else if (curE + stepE + d[x][y] > E) {
                cnt++;
                save(x, y);
                curE = d[x][y];
            } else {
                curE += stepE;
                path[cnt].push_back({x, y, curE, d[x][y]});
            }
        }
    }

    vis[x][y] = 1;

    for (int k = 0; k < 4; k++) {
        int nx = x + dx[k], ny = y + dy[k];
        if (nx >= 0 && nx < n && ny >= 0 && ny < m && a[nx][ny] != 1 && !vis[nx][ny]) {
            if (k == 0) stL.push({nx, ny});
            else if (k == 1) stU.push({nx, ny});
            else if (k == 2) stR.push({nx, ny});
            else if (k == 3) stD.push({nx, ny});
            upd(x, y, nx, ny);
        }
    }

    px = x; py = y;
    run();
}

void run() {
    while (!stL.empty()) {
        auto t = stL.top(); stL.pop();
        if (!vis[t.first][t.second]) go(t.first, t.second);
    }
    while (!stU.empty()) {
        auto t = stU.top(); stU.pop();
        if (!vis[t.first][t.second]) go(t.first, t.second);
    }
    while (!stR.empty()) {
        auto t = stR.top(); stR.pop();
        if (!vis[t.first][t.second]) go(t.first, t.second);
    }
    while (!stD.empty()) {
        auto t = stD.top(); stD.pop();
        if (!vis[t.first][t.second]) go(t.first, t.second);
    }
}
