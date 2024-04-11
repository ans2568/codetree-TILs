#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>
using namespace std;
int P, N, M, C, D, rx, ry;
int santa_dx[] = {0, 1, 0, -1};
int santa_dy[] = {-1, 0, 1, 0};
int dolph_dx[] = {1, 1, 1, 0, 0, -1, -1, -1};
int dolph_dy[] = {-1, 0, 1, -1, 1, -1, 0, 1};
vector<int> dolph_dist; // dolph_dist[산타 idx] = 산타와 루돌프 간 거리
int stun_santa[31] = {false, }; // 산타 기절 유무
bool fail_santa[31] = {false, }; // 산타 탈락 유무
unordered_map<int, pair<int, int>> santa; // {산타 번호, {y, x}}
bool visited[51][51]; // 산타가 현재 존재하는 위치
vector<int> santa_score; // 산타 점수
void interaction(int num, int y, int x, int direction, char type);
void cal_dist();
// 거리 계산
int dist(int sy, int sx, int ey, int ex) {
    return ((sy - ey) * (sy - ey) + (sx - ex) * (sx - ex));
}

// 루돌프 움직이기
void dolph_move() {
    int min = 52;
    int santa_num; // 가장 가까운 산타 번호
    int x, y; // 가장 가까운 산타 좌표

    // 가장 가까운 산타 선택
    for (int i=1; i<=P; ++i) {
        // 게임에서 탈락하지 않은 산타 중
        if (!fail_santa[i]) {
            // 가장 가까운 산타 선택
            if (min > dolph_dist[i]) {
                min = dolph_dist[i];
                santa_num = i;
                y = santa[i].first;
                x = santa[i].second;
            }
            // 가장 가까운 산타가 2명 이상이면, r좌표가 크거나 r이 동일한 경우 c가 큰 산타 선택
            else if (min == dolph_dist[i]) {
                if (santa[i].first > y) {
                    santa_num = i;
                    y = santa[i].first;
                    x = santa[i].second;
                }      
                // r이 동일한 경우 c가 큰 산타 선택
                else if (santa[i].first == y) {
                    if (santa[i].second > x) {
                        santa_num = i;
                        y = santa[i].first;
                        x = santa[i].second;
                    }
                }   
            }
        }
    }

    int min_dist = 52;
    int direction = -1;
    // 루돌프가 산타 방향으로 움직이기 8방향 중 최 우선 방향
    for (int i=0; i<8; ++i) {
        int nry = ry + dolph_dy[i];
        int nrx = rx + dolph_dx[i];
        if (nry <= 0 || nry > N || nrx <=0 || nrx > N) continue;
        int d = dist(nry, nrx, y, x); // 거리 계산
        // 가장 우선 순위가 높은 산타를 향한 방향 선택
        if (min_dist > d) {
            min_dist = d;
            direction = i;
        }
    }
    if (direction == -1) cout << direction;
    // 루돌프 위치 업데이트
    ry += dolph_dy[direction];
    rx += dolph_dx[direction];

    // 충돌 확인
    for (int i=1; i<=P; ++i) {
        // 탈락하지 않은 산타 중
        if (!fail_santa[i]) {
            int sy = santa[i].first;
            int sx = santa[i].second;
            int d = dist(ry, rx, sy, sx);
            // 루돌프가 산타를 충돌했을 경우
            if (d == 0) {
                // 산타 C만큼 점수 획득
                santa_score[i] += C;
                // 밀려나기 전 산타 방문 좌표 false로 변경
                visited[santa[i].first][santa[i].second] = false;
                // 산타 C만큼 밀려나기
                santa[i].first += dolph_dy[direction]*C;
                santa[i].second += dolph_dx[direction]*C;
                // 밀려난 곳이 게임판 밖이면 산타 탈락
                if (santa[i].first <= 0 || santa[i].first > N || santa[i].second <= 0 || santa[i].second > N) {
                    fail_santa[i] = true;
                    continue;
                }
                visited[santa[i].first][santa[i].second] = true;

                // 밀려난 곳에 다른 산타가 존재할 경우 상호작용
                interaction(i, santa[i].first, santa[i].second, direction, 'r');

                // 산타 기절
                stun_santa[i] = 2;
            }
        }
    }
    // 루돌프와 산타 사이 거리 업데이트
    cal_dist();
}

void cal_dist() {
    for (int i=1; i<=P; ++i) {
        dolph_dist[i] = dist(ry, rx, santa[i].first, santa[i].second);
    }
}

void interaction(int num, int y, int x, int direction, char type) {
    for (int i=1; i<=P; ++i) {
        if (num == i) continue;
        if (fail_santa[i]) continue;
        // 산타가 존재할 경우
        if (santa[i].first == y && santa[i].second == x) {
            if (type == 'r') {
                santa[i].first += dolph_dy[direction];
                santa[i].second += dolph_dx[direction];
            } else if (type == 's') {
                santa[i].first -= santa_dy[direction];
                santa[i].second -= santa_dx[direction];
            }
            if (santa[i].first <= 0 || santa[i].first > N || santa[i].second <= 0 || santa[i].second > N) {
                fail_santa[i] = true;
                continue;
            }
            visited[santa[i].first][santa[i].second] = true;
            interaction(i, santa[i].first, santa[i].second, direction, type);
        }
    }
}

void santa_move() {
    for (int i=1; i<=P; ++i) {
        // 기절 혹은 탈락한 산타 제거
        if (stun_santa[i] > 0 || fail_santa[i]) continue;
        // 기절하거나 탈락하지 않은 산타 중
        int min_dist = dolph_dist[i];
        int direction = -1;
        for (int j=0; j<4; ++j) {
            int ny = santa[i].first + santa_dy[j];
            int nx = santa[i].second + santa_dx[j];
            // 게임판 밖이거나 움직이는 칸에 산타가 존재하는 경우
            if (ny <= 0 || ny > N || nx <= 0 || nx > N || visited[ny][nx]) continue;
            // 루돌프와 거리가 가깝다면
            int d = dist(ry, rx, ny, nx);
            if (min_dist > d) {
                min_dist = d;
                direction = j;
            }
        }
        if (direction == -1) continue;
        // 산타가 루돌프랑 가까워지는 방향으로 위치 업데이트
        visited[santa[i].first][santa[i].second] = false;
        santa[i].first += santa_dy[direction];
        santa[i].second += santa_dx[direction];
        visited[santa[i].first][santa[i].second] = true;

        // 산타와 루돌프 거리 업데이트
        dolph_dist[i] = dist(ry, rx, santa[i].first, santa[i].second);

        // 산타가 루돌프에게 충돌
        if (dolph_dist[i] == 0) {
            // 산타 D 점수 획득
            santa_score[i] += D;
            visited[santa[i].first][santa[i].second] = false;
            // 산타 D 만큼 반대 방향으로 밀려남
            santa[i].first -= santa_dy[direction]*D;
            santa[i].second -= santa_dx[direction]*D;

            // 밀려난 곳이 게임판 밖이면 산타 탈락
            if (santa[i].first <= 0 || santa[i].first > N || santa[i].second <= 0 || santa[i].second > N) {
                fail_santa[i] = true;
                continue;
            }
            visited[santa[i].first][santa[i].second] = true;

            // 밀려난 곳에 다른 산타가 존재할 경우 상호작용
            interaction(i, santa[i].first, santa[i].second, direction, 's');

            // 기절
            stun_santa[i] = 2;
        }
    }
    // 루돌프와 산타 사이 거리 업데이트
    cal_dist();
}

void input() {
     // 게임판 크기, 게임 턴 수, 산타 수, 루돌프 힘, 산타 힘, 산타 번호
    cin >> N >> M >> P >> C >> D;
    cin >> ry >> rx; // 루돌프 초기 위치
    dolph_dist.resize(P+1);
    santa_score.resize(P+1);
    for (int i=0; i<P; ++i) {
        int num, sy, sx;
        cin >> num >> sy >> sx;
        santa.insert(make_pair(num, make_pair(sy, sx)));
        dolph_dist[num] = dist(ry, rx, sy, sx);
        visited[sy][sx] = true; // 산타 현재 위치 저장
    }
}

int main() {
    // 입력
    input();

    // 게임 시작
    for (int i=0; i<M; ++i) {
        // cout << i+1 << " 턴" << endl;
        // 루돌프 움직임
        // cout << "루돌프 이동 전 위치 : " << ry << ", " << rx << endl;
        dolph_move();
        // cout << "루돌프 이동 후 위치 : " << ry << ", " << rx << endl;
        // for (int j=1; j<=P; ++j) {
        //     cout << j << "번 산타 : " << santa[j].first << ", " << santa[j].second << endl;
        //     cout << j << "번 산타 탈락 여부 : " << fail_santa[j] << endl;
        // }
        // 산타 움직임
        santa_move();
        // for (int j=1; j<=P; ++j) {
        //     cout << j << "번 산타 : " << santa[j].first << ", " << santa[j].second << endl;
        //     cout << j << "번 산타 탈락 여부 : " << fail_santa[j] << endl;
        // }
        // 게임 종료
        bool close = true; // 게임 종료
        for (int j=1; j<=P; ++j) {
            if (!fail_santa[j]) {
                close = false;
                santa_score[j] += 1;
                if (stun_santa[j] > 0)
                    stun_santa[j] -= 1;
            }
        }
        // for (int j=1; j<=P; ++j) {
        //     cout << j << "번 산타 점수 : " << santa_score[j] << endl;
        // }
        if (close)
            break;
    }
    for (int i=1; i<=P; ++i) {
        cout << santa_score[i] << " ";
    }
    cout << endl;


    return 0;
}