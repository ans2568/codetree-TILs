#include <iostream>
#include <vector>
#include <map>
#include <cstring>
using namespace std;
struct Knight {
    int y; // r
    int x; // c
    int w; // 너비
    int h; // 높이
    int k; // 체력
    int damaged; // 받은 데미지
};
int L, N, Q;
int game[50][50];
int dy[4] = {-1, 0, 1, 0};
int dx[4] = {0, 1, 0, -1};
bool killed[50];
bool pulled[50];
map<int, Knight> knights;
bool result;
void input();

bool move(int idx, int direction, vector<pair<int, pair<int, int>>>& vec) {
    // cout << idx << "번째 기사 이동 시작" << endl;
    int y = knights[idx].y;
    int x = knights[idx].x;
    int w = knights[idx].w;
    int h = knights[idx].h;
    int nx = x + dx[direction];
    int ny = y + dy[direction];
    // 기사의 좌표를 포함한 모든 방패에서
    for (int r=ny; r<=ny+h-1; ++r) {
        for (int c=nx; c<=nx+w-1; ++c) {
            // cout << idx << "번째 기사 " << r << ", " << c << " 좌표 확인" << endl;
            //벽이거나 맵 밖이라면
            if (game[r][c] == 2 || r <= 0 || r > L || c <= 0 || c > L) {
                return false;
            }
            for (int i=1; i<=N; ++i) {
                if (i == idx) continue;
                if (killed[i]) continue;
                // 이미 존재하는 기사 있다면
                if ((r >= knights[i].y && r <= (knights[i].y + knights[i].h - 1)) && 
                    (c >= knights[i].x && c <= (knights[i].x + knights[i].w - 1))) {
                    // 밀쳤는데 이미 있던 기사가 움직이지 않았다면
                    if (!move(i, direction, vec) || !result) {
                        result = false;
                        return false;
                    }
                    // cout << i << "번째 기사 밀쳐짐" << endl;
                    pulled[i] = true;
                }
            }
        }
    }
    vec.push_back(make_pair(idx, make_pair(ny, nx)));
    // knights[idx].y = ny;
    // knights[idx].x = nx;
    return true;
}

void damage(int idx) {
    for (int i=1; i<=N; ++i) {
        if (i == idx) continue;
        if (killed[i]) continue;
        if (!pulled[i]) continue;
        auto k = knights[i];
        for (int y=k.y; y<=k.y+k.h-1; ++y) {
            for (int x=k.x; x<=k.x+k.w-1; ++x) {
                if (killed[i]) continue;
                if (game[y][x] == 1) {
                    knights[i].k -= 1;
                    if (knights[i].k <= 0) {
                        killed[i] = true;
                        continue;
                    }
                    knights[i].damaged += 1;
                    // cout << i << "번째 기사 받은 데미지 + 1 = " << knights[i].damaged << endl;
                }
            }
        }
    }
}

int main() {
    input();
    int res = 0;
    for (int i=1; i<=N; ++i) {
        if (!killed[i]) {
            res += knights[i].damaged;
        }
    }
    cout << res << endl;

    return 0;
}

void input() {
    cin >> L >> N >> Q;
    for (int i=1; i<=L; ++i) {
        for (int j=1; j<=L; ++j) {
            cin >> game[i][j];
        }
    }
    for (int i=1; i<=N; ++i) {
        int r, c, h, w, k;
        cin >> r >> c >> h >> w >> k;
        Knight kn;
        kn.y = r;
        kn.x = c;
        kn.h = h;
        kn.w = w;
        kn.k = k;
        kn.damaged = 0;
        knights.insert({i, kn});
    }
    for (int i=0; i<Q; ++i) {
        memset(pulled, false, sizeof(pulled));
        result = true;
        // 왕 명령
        int idx, d;
        cin >> idx >> d;
        if (killed[idx]) {
            // cout << idx << "번째 기사 죽음" << endl;
            continue;
        } 
            
        // 기사 이동
        vector<pair<int, pair<int, int>>> vec;
        if (!move(idx, d, vec)) {
            // cout << idx << "번째 기사가 밀쳤으나 벽이나 밖이라 움직이지 않음" << endl;
            continue;
        }
        for (int j=0; j<vec.size(); ++j) {
            // cout << vec[j].first << "번째 기사 좌표 변환" << endl;
            knights[vec[j].first].y = vec[j].second.first;
            knights[vec[j].first].x = vec[j].second.second;
        }
        damage(idx);
    }
}