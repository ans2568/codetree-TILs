#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
using namespace std;
#define MAX_VAL 987654321
int dy[] = {-1, 1, 0, 0}; // 상 하 좌 우
int dx[] = {0, 0, -1, 1};

void input();
int dist(int sy, int sx, int ey, int ex);
void all_dist();
void turn(int miny, int minx, int maxy, int maxx);
void move();

// 미로 칸
struct Kan {
    int hp; // 내구도
    bool can_move = true;
    bool is_exit = false;
    int distance; // 출구까지의 거리

    Kan& operator= (const Kan& other) {
        // Guard self assignment
        if (this == &other)
            return *this;
        hp = other.hp;
        can_move = other.can_move;
        is_exit = other.is_exit;
        distance = other.distance;
        return *this;
    }
};

Kan maze[11][11]; // 미로
int N, M, K, exit_y, exit_x;
vector<pair<pair<int, int>, int>> person;
bool escape[10];
int min_y = MAX_VAL, min_x = MAX_VAL, max_y = 0, max_x = 0;

void move() {
    // 모든 참가자
    for (int idx=0; idx<M; ++idx) {
        int y = person[idx].first.first;
        int x = person[idx].first.second;
        int move_cost = person[idx].second;
        int current_dist = dist(y, x, exit_y, exit_x);
        for (int i=0; i<4; ++i) {
            int ny = y + dy[i];
            int nx = x + dx[i];
            if (ny <= 0 || ny > N || nx <= 0 || nx > N) continue; // 밖으로 나갔을 경우
            if (maze[ny][nx].hp > 0) continue; // 벽일 경우
            int next_dist = dist(ny, nx, exit_y, exit_x);
            // 출구에 도착할 경우
            if (next_dist == 0) {
                escape[idx] = true;
                person[idx].first.first = ny;
                person[idx].first.second = nx;
                person[idx].second = move_cost + 1;
                break;
            }
            if (next_dist < current_dist) { // 현재 거리보다 이동할 거리가 짧을 경우
                person[idx].first.first = ny;
                person[idx].first.second = nx;
                person[idx].second = move_cost + 1;
            }
        }
    }
}

void select_square() {
    for (int i=0; i<M; ++i) {
        if (escape[i]) continue;
        min_y = min(min_y, person[i].first.first);
        min_x = min(min_x, person[i].first.second);
        max_y = min(max_y, person[i].first.first);
        max_x = min(max_x, person[i].first.second);
    }
    min_y = min(min_y, exit_y);
    min_x = min(min_x, exit_x);
    max_y = min(max_y, exit_y);
    max_x = min(max_x, exit_x);
}

bool success() {
    for (int i=0; i<M; ++i) {
        if (!escape[i]) {
            return false;
        }
    }
    return true;
}

int main() {
    // 입력
    input();

    for (int i=0; i<K; ++i) {
        // 출구까지 거리 계산
        all_dist();
        // 참가자 이동
        move();
        // 가장 작은 정사각형 선택
        select_square();
        // 미로 회전
        turn(min_y, min_x, max_y, max_x);
        // 게임 종료 확인
        if (success())
            break; 
    }
    int res = 0;
    for (int i=0; i<M; ++i) {
        res += person[i].second;
    }
    cout << res << endl;
    cout << exit_y << " " << exit_x << endl;

    return 0;
}

void input() {
    cin >> N >> M >> K;
    for (int i=1; i<=N; i++) {
        for (int j=1; j<=N; j++) {
            int power;
            Kan k;
            cin >> power;
            if (power > 0) {
                k.hp = power;
                k.can_move = false;
            } else if (power == 0) {
                k.hp = 0;
            }
            maze[i][j] = k;
        }
    }
    person.resize(M);
    for (int i=0; i<M; ++i) {
        cin >> person[i].first.first >> person[i].first.second;
        person[i].second = 0;
    }
    Kan exit;
    cin >> exit_y >> exit_x;
    exit.hp = 0;
    exit.is_exit = true;
    exit.distance = 0;
    maze[exit_y][exit_x] = exit;
}

// 거리 계산
int dist(int sy, int sx, int ey, int ex) {
    return (abs(sy - ey) + abs(sx - ex)); 
}

// 출구까지 모든 거리 계산
void all_dist() {
    for (int i=1; i<=N; ++i) {
        for (int j=1; j<=N; ++j) {
            maze[i][j].distance = dist(i, j, exit_y, exit_x);
        }
    }
}

// 회전
void turn(int miny, int minx, int maxy, int maxx) {
    int n = maxy-miny+1;
    Kan temp[n][n];
    for (int i=miny; i<=maxy; ++i) {
        for (int j=minx; j<=maxx; ++j) {
            if (maze[i][j].hp > 0)
                maze[i][j].hp -= 1;
            temp[i-miny][j-minx] = maze[i][j];
        }
    }
    for (auto& p : person) {
        int y = p.first.first;
        int x = p.first.second;
        if (y <= maxy && y >= miny && x <= maxx && x >= minx) {
            p.first.first = n-x-1;
            p.first.second = y;
        }
    }
    for (int i=0; i<n; ++i) {
        for (int j=0; j<n; ++j) {
            maze[miny+i][minx+j] = temp[n-j-1][i];
        }
    }
}