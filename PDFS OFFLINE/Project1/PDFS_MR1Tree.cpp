#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <sstream>
#include <tuple>
#include <algorithm>
#include <functional>

using namespace std;
int n;
double B;
vector<tuple<int, int, double>> edgesList;

struct Node {
	int id;
	vector<pair<Node*, double>> children;
	Node* parent = nullptr;
	bool visited = false; // Mark if the node has been visited during DFS.
	Node(int _id) : id(_id) {}
};

struct PathResult {
	vector<Node*> path;
};

class Tree {
private:
	Node* root;
	double B;
	unordered_map<int, Node*> nodes;
	unordered_map<int, double> distanceFromRoot;

	// Gets the node with given id or creates a new one if it does not exist.
	Node* getOrCreate(int id) {
		if (!nodes.count(id)) {
			nodes[id] = new Node(id);
		}
		return nodes[id];
	}

	// Recursively assign the distance from the root node to each node.
	void assignDistanceFromRoot(Node* node, double currentLength, unordered_set<int>& visitedSet) {
		if (visitedSet.count(node->id))
			return;
		visitedSet.insert(node->id);
		distanceFromRoot[node->id] = currentLength;
		for (auto& childPair : node->children) {
			Node* child = childPair.first;
			double w = childPair.second;
			assignDistanceFromRoot(child, currentLength + w, visitedSet);
		}
	}

	// Return the weight of the edge connecting two nodes.
	double getEdgeWeight(Node* a, Node* b) {
		for (auto& edge : a->children) {
			if (edge.first == b)
				return edge.second;
		}
		for (auto& edge : b->children) {
			if (edge.first == a)
				return edge.second;
		}
		return 0.0;
	}

public:
	// Constructs the tree from an edge list
	Tree(const vector<tuple<int, int, double>>& edgeList, int n, double _B) : B(_B) {
		root = getOrCreate(0);
		for (const auto& edge : edgeList) {
			int from = get<0>(edge);
			int to = get<1>(edge);
			double w = get<2>(edge);
			Node* parent = getOrCreate(from);
			Node* child = getOrCreate(to);
			parent->children.emplace_back(child, w);
			child->parent = parent;
		}
		unordered_set<int> visitedSet;
		assignDistanceFromRoot(root, 0.0, visitedSet);
	}

	PathResult path_to_root(Node* target) {
		PathResult result;
		while (target) {
			result.path.push_back(target);
			target = target->parent;
		}
		return result;
	}


	/*void PDFS_offline() {
		vector<pair<Node*, double>> path;
		vector<int> currentRoute;
		vector<vector<int>> routes;
		vector<double> routeCosts;
		vector<double> surplusEnergy;

		double currentLength = 0.0;
		double routeEnergy = 0.0;
		double sumEnergy = 0.0;
		int routeCount = 1;

		vector<int> fullDFS;
		unordered_set<Node*> exploredNodes;  // Track explored nodes

		path.push_back({ root, 0.0 });
		currentRoute.push_back(root->id);
		fullDFS.push_back(root->id);

		function<bool(Node*, double)> canExploreNode = [&](Node* node, double remainingEnergy) {
			double costToNode = 0.0;
			// Calculate cost to reach this node from current position
			for (auto& childPair : node->children) {
				if (!childPair.first->visited) {
					double nodeCost = childPair.second * 2; // Round trip cost
					if (nodeCost <= remainingEnergy) {
						return true;
					}
				}
			}
			return false;
			};

		function<void(Node*, double)> exploreWithSurplus = [&](Node* currentNode, double surplus) {
			for (auto& childPair : currentNode->children) {
				Node* child = childPair.first;
				double weight = childPair.second;

				if (!child->visited && (weight * 2) <= surplus) {
					// We can explore this node with our surplus
					child->visited = true;
					currentRoute.push_back(child->id);
					fullDFS.push_back(child->id);
					routeEnergy += (weight * 2);

					// Recursively explore from this new node with remaining surplus
					exploreWithSurplus(child, surplus - (weight * 2));

					currentRoute.push_back(currentNode->id);
					fullDFS.push_back(currentNode->id);
				}
			}
			};

		function<void(Node*)> DFS = [&](Node* u) {
			// Iterate over children of node u.
			for (auto& childPair : u->children) {
				Node* v = childPair.first;
				double w = childPair.second;

				if (!v->visited) {
					if (routeEnergy + w + (currentLength + w) > B) {
						// Before starting new route, try to use surplus
						double surplus = B - (routeEnergy + currentLength);
						if (surplus > 0) {
							exploreWithSurplus(u, surplus);
						}

						double distToRoot = currentLength;
						routeEnergy += distToRoot;

						surplusEnergy.push_back(B - routeEnergy);

						for (int i = (int)path.size() - 2; i >= 0; i--) {
							currentRoute.push_back(path[i].first->id);
						}
						routes.push_back(currentRoute);
						routeCosts.push_back(routeEnergy);

						// Start a new route from the root.
						routeCount++;
						currentRoute.clear();
						currentRoute.push_back(root->id);
						routeEnergy = 0;
						double newDist = 0;

						// Reconstruct the route from the root to the current node u.
						for (int i = 1; i < (int)path.size(); i++) {
							Node* parentNode = path[i - 1].first;
							Node* curNode = path[i].first;
							double wt = path[i].second;
							newDist += wt;
							routeEnergy += wt;
							sumEnergy += newDist;
							currentRoute.push_back(curNode->id);
						}
						currentLength = newDist;
					}

					v->visited = true;
					// Record DFS event: visit v.
					fullDFS.push_back(v->id);
					currentLength += w;
					routeEnergy += w;
					sumEnergy += currentLength;
					currentRoute.push_back(v->id);
					path.push_back({ v, w });

					DFS(v);

					// When returning (backtracking) add an event for DFS order.
					fullDFS.push_back(u->id);

					// Backtracking: update path and route metrics.
					path.pop_back();
					currentLength -= w;
					routeEnergy += w;
					currentRoute.push_back(u->id);
				}
			}
			};

		// Start DFS from the root.
		root->visited = true;
		DFS(root);

		// Finish the final route if it was not closed.
		if (currentRoute.size() > 1) {
			double distToRoot = currentLength;
			routeEnergy += distToRoot;

			// Try to use remaining surplus before finishing
			double finalSurplus = B - routeEnergy;
			if (finalSurplus > 0) {
				exploreWithSurplus(root, finalSurplus);
			}

			surplusEnergy.push_back(B - routeEnergy);

			for (int i = (int)path.size() - 2; i >= 0; i--) {
				currentRoute.push_back(path[i].first->id);
			}
			routes.push_back(currentRoute);
			routeCosts.push_back(routeEnergy);
		}

		// Print the full DFS traversal order
		for (int id : fullDFS) {
			cout << id << " ";
		}
		cout << endl;

		for (int i = 0; i < routes.size(); i++) {
			cout << "Route " << (i + 1) << " (cost = " << routeCosts[i]
				<< ", surplus = " << surplusEnergy[i] << "): ";
			for (int j = 0; j < routes[i].size(); j++) {
				cout << routes[i][j];
				if (j < routes[i].size() - 1)
					cout << " ";
			}
			cout << endl;
		}
	}*/
	void PDFS_offline() {
		vector<pair<Node*, double>> path;
		vector<int> currentRoute;
		vector<vector<int>> routes;
		vector<double> routeCosts;
		vector<double> surplusEnergy;

		double currentLength = 0.0;
		double routeEnergy = 0.0;
		double sumEnergy = 0.0;
		int routeCount = 1;
		double carriedSurplus = 0.0; // Năng lượng dồn từ tuyến trước

		vector<int> fullDFS;
		unordered_set<Node*> exploredNodes;

		path.push_back({ root, 0.0 });
		currentRoute.push_back(root->id);
		fullDFS.push_back(root->id);

		function<void(Node*, double)> exploreWithSurplus = [&](Node* currentNode, double surplus) {
			for (auto& childPair : currentNode->children) {
				Node* child = childPair.first;
				double weight = childPair.second;

				if (!child->visited && (weight * 2) <= surplus) {
					child->visited = true;
					currentRoute.push_back(child->id);
					fullDFS.push_back(child->id);
					routeEnergy += (weight * 2);
					exploreWithSurplus(child, surplus - (weight * 2));

					currentRoute.push_back(currentNode->id);
					fullDFS.push_back(currentNode->id);
				}
			}
			};

		function<void(Node*)> DFS = [&](Node* u) {
			for (auto& childPair : u->children) {
				Node* v = childPair.first;
				double w = childPair.second;

				if (!v->visited) {

					if (routeEnergy + w + (currentLength + w) > B + carriedSurplus) {

						double surplus = B + carriedSurplus - (routeEnergy + currentLength);
						if (surplus > 0) {
							exploreWithSurplus(u, surplus);
						}


						double distToRoot = currentLength;
						routeEnergy += distToRoot;

						double actualSurplus = B + carriedSurplus - routeEnergy;
						surplusEnergy.push_back(actualSurplus);
						carriedSurplus = actualSurplus;

						for (int i = (int)path.size() - 2; i >= 0; i--) {
							currentRoute.push_back(path[i].first->id);
						}
						routes.push_back(currentRoute);
						routeCosts.push_back(routeEnergy);


						routeCount++;
						currentRoute.clear();
						currentRoute.push_back(root->id);
						routeEnergy = 0.0;
						double newDist = 0.0;


						for (int i = 1; i < (int)path.size(); i++) {
							Node* parentNode = path[i - 1].first;
							Node* curNode = path[i].first;
							double wt = path[i].second;
							newDist += wt;
							routeEnergy += wt;
							sumEnergy += newDist;
							currentRoute.push_back(curNode->id);
						}
						currentLength = newDist;
					}

					v->visited = true;
					fullDFS.push_back(v->id);
					currentLength += w;
					routeEnergy += w;
					sumEnergy += currentLength;
					currentRoute.push_back(v->id);
					path.push_back({ v, w });

					DFS(v);

					fullDFS.push_back(u->id);
					path.pop_back();
					currentLength -= w;
					routeEnergy += w;
					currentRoute.push_back(u->id);
				}
			}
			};

		root->visited = true;
		DFS(root);


		if (currentRoute.size() > 1) {
			double distToRoot = currentLength;
			routeEnergy += distToRoot;

			double finalSurplus = B + carriedSurplus - routeEnergy;
			if (finalSurplus > 0) {
				exploreWithSurplus(root, finalSurplus);
			}

			surplusEnergy.push_back(finalSurplus);
			carriedSurplus = finalSurplus;

			for (int i = (int)path.size() - 2; i >= 0; i--) {
				currentRoute.push_back(path[i].first->id);
			}
			routes.push_back(currentRoute);
			routeCosts.push_back(routeEnergy);
		}
		// Print the full DFS traversal order
		for (int id : fullDFS) {
			cout << id << " ";
		}
		cout << endl;

		for (int i = 0; i < routes.size(); i++) {
			cout << "Route " << (i + 1) << " (cost = " << routeCosts[i]
				<< ", surplus = " << surplusEnergy[i] << "): ";
			for (int j = 0; j < routes[i].size(); j++) {
				cout << routes[i][j];
				if (j < routes[i].size() - 1)
					cout << " ";
			}
			cout << endl;
		}
	}

	~Tree() {
		for (auto& pair : nodes) {
			delete pair.second;
		}
		nodes.clear();
	}
};


vector<tuple<int, int, double>> readEdgeList() {
	for (int i = 0; i < n - 1; i++) {
		int from, to;
		double w;
		cin >> from >> to >> w;
		edgesList.emplace_back(from, to, w);
	}
	return edgesList;
}

int main() {
	try {
		cin >> n >> B;
		readEdgeList();

		Tree tree(edgesList, n, B);
		tree.PDFS_offline();
	}
	catch (const exception& e) {
		cerr << "Error: " << e.what() << endl;
	}
	return 0;
}
