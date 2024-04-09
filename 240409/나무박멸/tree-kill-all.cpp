#include <iostream>
#include <queue>
#include <cstring>

using namespace std;
int N, M, K, C;
int dx[4] = {1, 0, -1, 0};
int dy[4] = {0, 1, 0, -1};
int kill_dx[4] = {1, 1, -1, -1};
int kill_dy[4] = {1, -1, -1, 1};
int map[20][20];
int kill[20][20]; // 제초제
queue<pair<int, int>> trees, breeding, whole;
int kx, ky, max_kill = 0, breed = 0;
bool visited[20][20];

void bfs() {
    memset(visited, false, sizeof(visited));
    // 성장
    while(!trees.empty()) {
        int y = trees.front().first;
        int x = trees.front().second;
        trees.pop();
        for (int i=0; i<4; ++i) {
            int ny = y + dy[i];
            int nx = x + dx[i];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
            if (map[ny][nx] == 0 || map[ny][nx] == -1 || kill[ny][nx] > 0) continue;
            // 인접 칸이 나무 일 때
            // 인접 칸의 수만큼 성장
            map[y][x] += 1;
        }
    }
    queue<pair<pair<int, int>, int>> q; // x, y, 인접 빈칸 개수
    // 번식
    while(!breeding.empty()) {
        int y = breeding.front().first;
        int x = breeding.front().second;
        breeding.pop();
        breed = 0;
        for (int i=0; i<4; ++i) {
            int ny = y + dy[i];
            int nx = x + dx[i];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
            if (map[ny][nx] == -1) continue;
            // 각 칸의 나무 그루 수에서 총 번식이 가능한 칸의 개수만큼 나누어 그루 수만큼 번식
            // 인접 칸이 빈 칸일 때
            if (map[ny][nx] == 0 && kill[ny][nx] == 0) {
                breed += 1;
            }
        }
        visited[y][x] = true;
        q.push(make_pair(make_pair(x, y), breed));
    }
    while(!q.empty()) {
        int x = q.front().first.first;
        int y = q.front().first.second;
        int breed = q.front().second;
        q.pop();
        int breeded_tree;
        if (breed != 0) {
            breeded_tree = map[y][x] / breed;
        } else 
            breeded_tree = 0;
        for (int i=0; i<4; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (nx < 0 || nx >= N || ny < 0 || ny >= N) continue;
            if (!visited[ny][nx] && kill[ny][nx] == 0) {
                if (map[ny][nx] > 0)
                    map[ny][nx] += breeded_tree;
                else if (map[ny][nx] == 0) {
                    map[ny][nx] = breeded_tree;
                    whole.push(make_pair(ny, nx));
                }
            }
        }
    }
    // 제초제
    // 제초제 뿌릴 곳 찾기
    int max_kill_tree = 0;
    while(!whole.empty()) {
        int y = whole.front().first;
        int x = whole.front().second;
        int kill_val = map[y][x];
        whole.pop();
        for (int i=0; i<4; ++i) {
            for (int j=1; j<=K; j++) {
                int nx = x + kill_dx[i]*j;
                int ny = y + kill_dy[i]*j;
                if (nx < 0 || ny < 0 || nx >=N || ny >= N) continue;
                // 대각선으로 K만큼 전파될 때 나무인 칸
                if (map[ny][nx] > 0 && kill[ny][nx] == 0) {
                    kill_val += map[ny][nx];
                }
            }
        }
        if (max_kill_tree == kill_val) {
            // 행이 작은 순서대로
            if (y < ky) {
                kx = x;
                ky = y;
            } else if (y == ky) {
                // 열이 작은 순서대로
                if (x < kx) {
                    kx = x;
                    ky = y;
                }
            }
        }
        else if (max_kill_tree < kill_val) {
            // 제초제 칸 후보
            kx = x;
            ky = y;
            max_kill_tree = kill_val;
        }
    }

    // 제초제 뿌리기
    queue<pair<int, int>> kq;
    kq.push(make_pair(kx, ky));
    kill[ky][kx] = C+1;
    max_kill += map[ky][kx];
    map[ky][kx] = 0;
    while(!kq.empty()) {
        int x = kq.front().first;
        int y = kq.front().second;
        kq.pop();
        for (int i=0; i<4; ++i) {
            for (int j=1; j<=K; ++j) {
                int nx = x + kill_dx[i]*j;
                int ny = y + kill_dy[i]*j;
                if (nx < 0 || ny < 0 || nx >=N || ny >= N) continue;
                if (kill[ny][nx] > 0) {
                    kill[ny][nx] = C+1;
                    break;
                }
                // 제초제 살포
                kill[ny][nx] = C+1;
                // 벽 or 빈칸
                if (map[ny][nx] == -1 || map[ny][nx] == 0)
                    break;
                max_kill += map[ny][nx];
                map[ny][nx] = 0;
            }
        }
    }
    // 1년 증가
    for (int i=0; i<N; ++i) {
        for (int j=0; j<N; ++j) {
            if (kill[i][j] > 0)
                kill[i][j]--;
        }
    }
}

int main() {
    // 여기에 코드를 작성해주세요.
    cin >> N >> M >> K >> C;
    for (int i=0; i<N; ++i) {
        for (int j=0; j<N; ++j) {
            cin >> map[i][j];
        }
    }
    for (int i=0; i<M; ++i) {
        for (int i=0; i<N; ++i) {
            for (int j=0; j<N; ++j) {
                if (map[i][j] != 0 && map[i][j] != -1) {
                    trees.push(make_pair(i, j));
                    breeding.push(make_pair(i, j));
                    whole.push(make_pair(i, j));
                }
            }
        }
        bfs();
    }
    cout << max_kill << endl;
    return 0;
}