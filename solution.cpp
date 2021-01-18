#include <bits/stdc++.h>

#define fi first
#define se second
#define ll long long

using namespace std;

mt19937 rnd(13'06'2019);

const int N = 4e5 + 1;
const int LN = 2e6 + 1;
const int mod = 1e9 + 7;
const long long div1 = 100'001;

struct dek {
    int l, r, sz, key;
    long long hesh;
};

int top = 0;
dek m[LN];
long long t[N];
int lft[N], righ[N], a[N];
bool open[N], bad[N];
bool use[LN];

int get_next() {
    while (use[top])
        ++top;
    use[top] = 1;
    return top;
}

void recalc(int u) {
    m[u].sz = m[m[u].l].sz + m[m[u].r].sz + 1;
    m[u].hesh = (m[m[u].l].hesh * t[m[m[u].r].sz + 1] + t[m[m[u].r].sz] * m[u].key + m[m[u].r].hesh) % mod;
}

void copy_vert(int from, int to) {
    m[to] = m[from];
}

int merg(int a, int b) {
    if (a == 0)
        return b;
    if (b == 0)
        return a;
    int u = get_next();
    if (rnd() % (m[a].sz + m[b].sz) < m[a].sz) {
        copy_vert(a, u);
        m[u].r = merg(m[a].r, b);
    } else {
        copy_vert(b, u);
        m[u].l = merg(a, m[b].l);
    }
    recalc(u);
    return u;
}


void split(int root, int &a, int &b, int x) {
    if (root == 0) {
        a = 0;
        b = 0;
        return;
    }
    int u = get_next();
    copy_vert(root, u);
    if (m[m[root].l].sz >= x) {
        split(m[root].l, a, m[u].l, x);
        b = u;
    } else {
        split(m[root].r, m[u].r, b, x - m[m[root].l].sz - 1);
        a = u;
    }
    recalc(u);
}

int newv(int key) {
    int u = get_next();
    m[u].key = key;
    m[u].l = m[u].r = 0;
    m[u].sz = 1;
    m[u].hesh = key;
    return u;
}

void recalc_do(int v) {
    if (bad[2*v + 1] || bad[2*v + 2])
        bad[v] = 1;
    else {
        if (m[righ[2*v + 1]].sz >= m[lft[2*v + 2]].sz) {
            int a, b;
            split(righ[2*v + 1], a, b, m[righ[2*v + 1]].sz - m[lft[2*v + 2]].sz);
            if (m[b].hesh != m[lft[2*v + 2]].hesh)
                bad[v] = 1;
            else {
                lft[v] = lft[2*v + 1];
                righ[v] = merg(a, righ[2*v + 2]);
                bad[v] = 0;
            }
        } else {
            int a, b;
            split(lft[2*v + 2], a, b, m[lft[2*v + 2]].sz - m[righ[2*v + 1]].sz);
            if (m[b].hesh != m[righ[2*v + 1]].hesh)
                bad[v] = 1;
            else {
                lft[v] = merg(a, lft[2*v + 1]);
                righ[v] = righ[2*v + 2];
                bad[v] = 0;
            }
        }
    }
}

void build_do(int v, int vl, int vr) {
    if (vr - vl == 1) {
        righ[v] = 0;
        lft[v] = 0;
        if (open[vl] == 1)
            righ[v] = newv(a[vl]);
        else
            lft[v] = newv(a[vl]);
        bad[v] = 0;
    } else {
        build_do(2*v + 1, vl, (vl + vr) / 2);
        build_do(2*v + 2, (vl + vr) / 2, vr);
        recalc_do(v);
    }
}

void als(int v, int vl, int vr, int l, int x, bool op) {
    if (vr - vl == 1) {
        a[vl] = x;
        open[vl] = op;
        build_do(v, vl, vr);
    } else {
        if (l < (vl + vr) / 2)
            als(2*v + 1, vl, (vl + vr) / 2, l, x, op);
        else
            als(2*v + 2, (vl + vr) / 2, vr, l, x, op);
        recalc_do(v);
    }
}

bool zpr(int v, int vl, int vr, int l, int r, int &lefq, int &rigq) {
    if (l <= vl && vr <= r) {
        lefq = lft[v];
        rigq = righ[v];
        return bad[v];
    } else if (l >= vr || vl >= r) {
        lefq = 0;
        rigq = 0;
        return 0;
    } else {
        bool b1, b2;
        int l1, r1, l2, r2;
        b1 = zpr(2*v + 1, vl, (vl + vr) / 2, l, r, l1, r1);
        b2 = zpr(2*v + 2, (vl + vr) / 2, vr, l, r, l2, r2);
        if (b1 || b2)
            return true;
        if (m[r1].sz >= m[l2].sz) {
            int a, b;
            split(r1, a, b, m[r1].sz - m[l2].sz);
            if (m[b].hesh != m[l2].hesh)
                return true;
            rigq = merg(a, r2);
            lefq = l1;
        } else {
            int a, b;
            split(l2, a, b, m[l2].sz - m[r1].sz);
            if (m[b].hesh != m[r1].hesh)
                return true;
            rigq = r2;
            lefq = merg(a, l1);
        }
        return false;
    }
}

void dfs(int u) {
    if (use[u])
        return;
    use[u] = 1;
    dfs(m[u].l);
    dfs(m[u].r);
}

int main() {
    int i, j, k, n, p, x, q, type;
    //freopen("input.txt", "r", stdin);
    ios_base::sync_with_stdio(false); cin.tie(0); cout.tie(0);
    
    use[0] = 1;
    cin >> n >> k;
    t[0] = 1;
    for (i = 1; i <= n; ++i)
        t[i] = (t[i - 1] * div1) % mod;
    for (i = 1; i <= n; ++i) {
        cin >> a[i];
        if (a[i] > 0)
            open[i] = 1;
        else {
            a[i] = -a[i];
            open[i] = 0;
        }
    }
    build_do(0, 1, n + 1);
    cin >> q;
    for (i = 0; i < q; ++i) {
        cin >> type;
        if (type == 1) {
            cin >> p >> x;
            bool op;
            if (x > 0)
                op = 1;
            else {
                x = -x;
                op = 0;
            }
            als(0, 1, n + 1, p, x, op);
        } else {
            int l, r, lft, right;
            cin >> l >> r;
            bool bad = zpr(0, 1, n + 1, l, r + 1, lft, right);
            if (bad || lft != 0 || right != 0)
                cout << "No\n";
            else
                cout << "Yes\n";
        }
        if (top > LN - 1e4) {
            memset(use, 0, sizeof(use));
            for (j = 0; j < N; ++j) {
                dfs(lft[j]);
                dfs(righ[j]);
            }
            top = 0;
        }
    }
}
