#include <bits/stdc++.h>
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

void printD(); // In ra thông tin về đường đi
void save(int x, int y); // Lưu đường đi từ điểm (x, y) về điểm xuất phát (sx, sy)
void upd(int x, int y, int nx, int ny); // Cập nhật khoảng cách và Parent cho ô (nx, ny) từ ô (x, y)
void go(int x, int y); // Xử lý ô (x, y), cập nhật đường đi và khoảng cách
void run(); // Chạy thuật toán để tìm đường đi

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);
    auto start = std::chrono::high_resolution_clock::now(); // Bắt đầu đo thời gian
    // Nếu muốn nhận dự liệu đầu vào từ grid_matrix.txt
    //Kết quả xuất ra màn hình
    freopen("grid_matrix.txt", "r", stdin);

    //Nếu muốn nhận dự liệu đầu vào từ các file Ti.inp
    //Kết quả được xuất ra file Ti.out tương ứng
   // freopen("T4.inp", "r", stdin);
   // freopen("T4.out", "w", stdout);
    cin >> n >> m >> E;
    if (E % 2 != 0) E--; 
    if (E< 4*max(n,m)){
        cout<< "E must be at least 4 times the maximum of n and m." << endl;
        return 0;
    }
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            cin >> a[i][j];
            if (a[i][j] == 2) sx = i, sy = j;
        }

    path[cnt].push_back({sx, sy, curE, d[sx][sy]});
    stL.push({sx, sy});
    run();
    printD();


    auto end = std::chrono::high_resolution_clock::now(); // Kết thúc đo thời gian
    std::chrono::duration<double> diff = end - start;
    cout << "Time taken: " << diff.count() << " s\n";
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
}



void run() {
    // Ưu tiên duyệt theo thứ tự Trái, Trên, Phải, Dưới (Tây, Bắc, Đông, Nam)
    while (!stL.empty() || !stU.empty() || !stR.empty() || !stD.empty()) {
        pair<int, int> t;
        if (!stL.empty()) { t = stL.top(); stL.pop(); }
        else if (!stU.empty()) { t = stU.top(); stU.pop(); }
        else if (!stR.empty()) { t = stR.top(); stR.pop(); }
        else { t = stD.top(); stD.pop(); }

        if (!vis[t.first][t.second]) {
            go(t.first, t.second);
        }
    }
}
