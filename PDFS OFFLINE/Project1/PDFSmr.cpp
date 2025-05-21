#include <iostream>     // Cho cin, cout, fixed
#include <vector>      // Cho vector
#include <iomanip>     // Cho setprecision
#include <limits>      // Cho numeric_limits
#include <algorithm>   // Cho các thuật toán
#include <utility>     // Cho pair

using namespace std;

// Node structure for tree
struct Node {
    int id;
    bool visited;
    vector<pair<Node*, double>> children;
    Node(int id) : id(id), visited(false) {}
};

// Global variables for PDFS offline
vector<pair<Node*, double>> pathVec; // stores (node, weight) from root to current
vector<int> currentRoute;
vector<vector<int>> routes;
vector<double> routeCosts;
vector<int> fullDFS;
double currentLength;
double routeEnergy;
double sumEnergy;
double B; // energy budget for each route
double leftoverPool = 0.0;
Node* rootPtr;

// DFS function for PDFS offline
void DFS(Node* u) {
    for (auto& edge : u->children) {
        Node* v = edge.first;
        double w = edge.second;
        if (!v->visited) {
            // Check if going to v would exceed budget B
            if (routeEnergy + w + currentLength > B) {
                // Close current route and return to root
                double distToRoot = currentLength;
                routeEnergy += distToRoot;
                // Compute leftover (including any carried over)
                double leftover = B - routeEnergy + leftoverPool;
                // Try to use leftover on an unvisited branch (child of root)
                Node* branchNode = nullptr;
                double minWeight = numeric_limits<double>::infinity();
                // find nearest unvisited child of root
                for (auto& edge2 : rootPtr->children) {
                    Node* ch = edge2.first;
                    double ww = edge2.second;
                    if (!ch->visited && ww <= leftover && ww < minWeight) {
                        minWeight = ww;
                        branchNode = ch;
                    }
                }
                // Add path to currentRoute up to root (excluding last element root itself)
                for (int i = (int)pathVec.size() - 2; i >= 0; i--) {
                    currentRoute.push_back(pathVec[i].first->id);
                }
                // If we can use leftover
                if (branchNode != nullptr) {
                    cout << "Sử dụng " << fixed << setprecision(1) << minWeight
                        << " đơn vị năng lượng dư để duyệt nhánh "
                        << rootPtr->id << " -> " << branchNode->id << endl;
                    // Visit the branch node
                    branchNode->visited = true;
                    fullDFS.push_back(branchNode->id);
                    currentRoute.push_back(branchNode->id);
                    // Increase routeEnergy by travel cost to branch and back
                    routeEnergy += 2 * minWeight;
                    // Reduce leftover by used amount
                    leftover -= minWeight;
                    // Return to root
                    fullDFS.push_back(rootPtr->id);
                    currentRoute.push_back(rootPtr->id);
                    // Update leftoverPool
                    leftoverPool = leftover;
                }
                else {
                    // No branch used, carry leftover to next routes
                    leftoverPool = leftover;
                }
                // Save the completed route (from root to root via path)
                routes.push_back(currentRoute);
                routeCosts.push_back(routeEnergy);
                // Prepare for new route from root
                currentRoute.clear();
                currentRoute.push_back(rootPtr->id);
                routeEnergy = 0.0;
                double newDist = 0.0;
                // Build new route prefix from path (root to current node)
                for (int i = 1; i < (int)pathVec.size(); i++) {
                    Node* curNode = pathVec[i].first;
                    double wt = pathVec[i].second;
                    newDist += wt;
                    routeEnergy += wt;
                    sumEnergy += newDist;
                    currentRoute.push_back(curNode->id);
                }
                currentLength = newDist;
            }
            else {
                // Normal DFS expansion
                v->visited = true;
                fullDFS.push_back(v->id);
                currentLength += w;
                routeEnergy += w; // Thêm vào chi phí tuyến đường khi đi đến v
                sumEnergy += currentLength;
                currentRoute.push_back(v->id);
                pathVec.push_back({ v, w });
                DFS(v);
                // Backtracking
                fullDFS.push_back(u->id);
                pathVec.pop_back();
                currentLength -= w;
                currentRoute.push_back(u->id);
            }
        }
    }
}

void PDFS_offline(Node* root) {
    // Initialize global variables
    pathVec.clear();
    currentRoute.clear();
    routes.clear();
    routeCosts.clear();
    fullDFS.clear();
    currentLength = 0.0;
    routeEnergy = 0.0;
    sumEnergy = 0.0;
    leftoverPool = 0.0;
    rootPtr = root;
    // Starting at root
    pathVec.push_back({ root, 0.0 });
    currentRoute.push_back(root->id);
    fullDFS.push_back(root->id);
    // Mark root as visited
    root->visited = true;
    // Call DFS
    DFS(root);
    // Final route closure if any
    if (currentRoute.size() > 1) {
        double distToRoot = currentLength;
        routeEnergy += distToRoot;
        double leftover = B - routeEnergy + leftoverPool;
        Node* branchNode = nullptr;
        double minWeight = numeric_limits<double>::infinity();
        // find nearest unvisited child of root
        for (auto& edge2 : rootPtr->children) {
            Node* ch = edge2.first;
            double ww = edge2.second;
            if (!ch->visited && ww <= leftover && ww < minWeight) {
                minWeight = ww;
                branchNode = ch;
            }
        }
        // Add return path to root
        for (int i = (int)pathVec.size() - 2; i >= 0; i--) {
            currentRoute.push_back(pathVec[i].first->id);
        }
        if (branchNode != nullptr) {
            cout << "Sử dụng " << fixed << setprecision(1) << minWeight
                << " đơn vị năng lượng dư để duyệt nhánh "
                << rootPtr->id << " -> " << branchNode->id << endl;
            branchNode->visited = true;
            fullDFS.push_back(branchNode->id);
            currentRoute.push_back(branchNode->id);
            routeEnergy += 2 * minWeight;
            fullDFS.push_back(rootPtr->id);
            currentRoute.push_back(rootPtr->id);
            leftoverPool = leftover - minWeight;
        }
        else {
            leftoverPool = leftover;
        }
        routes.push_back(currentRoute);
        routeCosts.push_back(routeEnergy);
    }
    // Output results (full DFS order and routes with costs)
    cout << "Full DFS Order:" << endl;
    for (int id : fullDFS) {
        cout << id << " ";
    }
    cout << endl;
    cout << "Routes and costs:" << endl;
    for (size_t i = 0; i < routes.size(); i++) {
        cout << "Route " << i + 1 << ": ";
        for (int id : routes[i]) {
            cout << id << " ";
        }
        cout << " | Cost: " << fixed << setprecision(1) << routeCosts[i] << endl;
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(NULL);

    int n;
    cin >> n >> B;  // Đọc số điểm n và ngân sách năng lượng B

    // Tạo các nút cho đồ thị
    vector<Node*> nodes(n);
    for (int i = 0; i < n; i++) {
        nodes[i] = new Node(i);
    }

    // Đọc n-1 cạnh của cây
    for (int i = 0; i < n - 1; i++) {
        int u, v;
        double w;
        cin >> u >> v >> w;
        // Thêm cạnh vào cây (không hướng)
        nodes[u]->children.push_back({ nodes[v], w });
        nodes[v]->children.push_back({ nodes[u], w });
    }

    // Thực hiện PDFS từ nút gốc (giả sử nút 0 là gốc)
    PDFS_offline(nodes[0]);

    // Giải phóng bộ nhớ
    for (int i = 0; i < n; i++) {
        delete nodes[i];
    }

    return 0;
}