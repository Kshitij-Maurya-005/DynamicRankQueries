/*Problem Title

Range k-th Ranked Student Score with Point Updates

Problem Statement

Consider an array of student scores indexed by roll numbers 1..N. Scores are updated over time. You must support two operations:

- U idx val — Update: set the score of student at roll number idx to val.

- Q L R k — Query: among students with roll numbers in the inclusive range [L, R], return the k-th smallest  score.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAXN 200005

/* Interactive program for:
   "Range k-th Ranked Student Score with Point Updates"
   - Method: sqrt-decomposition with sorted blocks + coordinate compression
   - This version prints prompts so users know what to enter.
*/

int n, q;
int arr[MAXN];       // compressed indices (1-based)
int origVal[MAXN];   // original raw values
int compVals[MAXN];  // sorted unique values for compression
int compSize = 0;

int blockId[MAXN];
int blockStart[MAXN], blockEnd[MAXN];
int blockSize;
int numBlocks;

int *blkArr[MAXN];
int blkLen[MAXN];

int int_cmp(const void *a, const void *b) {
    int va = *(const int*)a;
    int vb = *(const int*)b;
    return (va < vb) ? -1 : (va > vb) ? 1 : 0;
}

// upper_bound: first index > x in sorted a[0..len-1]
int upper_bound_arr(int *a, int len, int x) {
    int l = 0, r = len;
    while (l < r) {
        int m = (l + r) >> 1;
        if (a[m] <= x) l = m + 1;
        else r = m;
    }
    return l;
}

void blk_insert(int b, int val) {
    int len = blkLen[b];
    int pos = (len>0) ? upper_bound_arr(blkArr[b], len, val) : 0;
    int *na = (int*)realloc(blkArr[b], (len + 1) * sizeof(int));
    if (!na) { fprintf(stderr, "Memory allocation failed\n"); exit(1); }
    for (int i = len; i > pos; --i) na[i] = na[i-1];
    na[pos] = val;
    blkArr[b] = na;
    blkLen[b] = len + 1;
}

void blk_remove(int b, int val) {
    int len = blkLen[b];
    if (len == 0) return;
    int *a = blkArr[b];
    int l = 0, r = len - 1, pos = -1;
    while (l <= r) {
        int m = (l + r) >> 1;
        if (a[m] == val) { pos = m; break; }
        else if (a[m] < val) l = m + 1;
        else r = m - 1;
    }
    if (pos == -1) {
        for (int i = 0; i < len; ++i) if (a[i] == val) { pos = i; break; }
    }
    if (pos == -1) return;
    for (int i = pos; i < len - 1; ++i) a[i] = a[i+1];
    blkLen[b] = len - 1;
    if (blkLen[b] == 0) { free(a); blkArr[b] = NULL; }
    else {
        int *na = (int*)realloc(a, blkLen[b] * sizeof(int));
        if (na) blkArr[b] = na;
        else blkArr[b] = a;
    }
}

int count_leq(int L, int R, int val) {
    int res = 0;
    int bL = blockId[L], bR = blockId[R];
    if (bL == bR) {
        for (int i = L; i <= R; ++i) if (arr[i] <= val) ++res;
        return res;
    }
    for (int i = L; i <= blockEnd[bL]; ++i) if (arr[i] <= val) ++res;
    for (int b = bL + 1; b <= bR - 1; ++b) if (blkLen[b] > 0)
        res += upper_bound_arr(blkArr[b], blkLen[b], val);
    for (int i = blockStart[bR]; i <= R; ++i) if (arr[i] <= val) ++res;
    return res;
}

void rebuild_all_blocks() {
    // free existing
    for (int b = 0; b < numBlocks; ++b) {
        if (blkArr[b]) { free(blkArr[b]); blkArr[b] = NULL; blkLen[b] = 0; }
    }
    // rebuild from current arr[]
    for (int b = 0; b < numBlocks; ++b) {
        int s = blockStart[b], e = blockEnd[b];
        int len = e - s + 1;
        int *tmp = (int*)malloc(len * sizeof(int));
        if (!tmp && len>0) { fprintf(stderr, "Memory fail\n"); exit(1); }
        int idx2 = 0;
        for (int i = s; i <= e; ++i) tmp[idx2++] = arr[i];
        qsort(tmp, len, sizeof(int), int_cmp);
        blkArr[b] = tmp;
        blkLen[b] = len;
    }
}

void do_update(int idx, int newValRaw) {
    // find compressed pos
    int l = 0, r = compSize - 1, pos = -1;
    while (l <= r) {
        int m = (l + r) >> 1;
        if (compVals[m] == newValRaw) { pos = m; break; }
        else if (compVals[m] < newValRaw) l = m + 1;
        else r = m - 1;
    }
    if (pos == -1) {
        // new value encountered: add and rebuild compression & blocks
        compVals[compSize++] = newValRaw;
        qsort(compVals, compSize, sizeof(int), int_cmp);
        int uniq = 0;
        for (int i = 0; i < compSize; ++i)
            if (i == 0 || compVals[i] != compVals[i-1]) compVals[uniq++] = compVals[i];
        compSize = uniq;
        // remap arr[] from origVal[]
        for (int i = 1; i <= n; ++i) {
            int lv = 0, rv = compSize - 1, p = -1;
            while (lv <= rv) {
                int mv = (lv + rv) >> 1;
                if (compVals[mv] == origVal[i]) { p = mv; break; }
                else if (compVals[mv] < origVal[i]) lv = mv + 1;
                else rv = mv - 1;
            }
            arr[i] = (p == -1) ? 0 : p + 1;
        }
        rebuild_all_blocks();
    }
    int b = blockId[idx];
    if (arr[idx] != 0) blk_remove(b, arr[idx]);
    origVal[idx] = newValRaw;
    // find new compressed pos
    int lv = 0, rv = compSize - 1, ppos = -1;
    while (lv <= rv) {
        int mv = (lv + rv) >> 1;
        if (compVals[mv] == newValRaw) { ppos = mv; break; }
        else if (compVals[mv] < newValRaw) lv = mv + 1;
        else rv = mv - 1;
    }
    if (ppos == -1) return;
    arr[idx] = ppos + 1;
    blk_insert(b, arr[idx]);
}

int do_query(int L, int R, int k) {
    int low = 1, high = compSize, ans = -1;
    while (low <= high) {
        int mid = (low + high) >> 1;
        int cnt = count_leq(L, R, mid);
        if (cnt >= k) { ans = mid; high = mid - 1; }
        else low = mid + 1;
    }
    if (ans == -1) return -1;
    return compVals[ans - 1];
}

int main() {
    printf("=== Range k-th Ranked Student Score (interactive) ===\n");
    printf("You will first enter: n q\n");
    printf("Then enter n integers (initial scores for roll 1..n) on one or more lines.\n");
    printf("Then enter q operations, each on its own line:\n");
    printf("  U idx val    -- update student at roll idx to score val\n");
    printf("  Q L R k      -- query k-th smallest score in roll range [L,R]\n");
    printf("\nExample input (paste after entering n q and array):\n");
    printf("Q 1 5 2\nU 3 12\nQ 1 5 2\nQ 4 8 3\nQ 1 8 5\n\n");

    printf("Enter n and q: ");
    fflush(stdout);
    if (scanf("%d %d", &n, &q) != 2) {
        printf("Invalid input. Exiting.\n"); return 0;
    }
    if (n <= 0 || n >= MAXN) { printf("n out of range. Exiting.\n"); return 0; }

    printf("Enter %d initial scores (space separated or newline separated):\n", n);
    fflush(stdout);
    for (int i = 1; i <= n; ++i) {
        int x;
        if (scanf("%d", &x) != 1) { printf("Insufficient initial values. Exiting.\n"); return 0; }
        origVal[i] = x;
        compVals[i-1] = x; // gather
    }

    // initial compression
    compSize = n;
    qsort(compVals, compSize, sizeof(int), int_cmp);
    int uniq = 0;
    for (int i = 0; i < compSize; ++i)
        if (i == 0 || compVals[i] != compVals[i-1]) compVals[uniq++] = compVals[i];
    compSize = uniq;

    // fill arr with compressed indices
    for (int i = 1; i <= n; ++i) {
        int l = 0, r = compSize - 1, pos = -1;
        while (l <= r) {
            int m = (l + r) >> 1;
            if (compVals[m] == origVal[i]) { pos = m; break; }
            else if (compVals[m] < origVal[i]) l = m + 1;
            else r = m - 1;
        }
        arr[i] = pos + 1;
    }

    blockSize = (int)sqrt(n); if (blockSize <= 0) blockSize = 1;
    numBlocks = (n + blockSize - 1) / blockSize;
    for (int b = 0; b < numBlocks; ++b) {
        blockStart[b] = b * blockSize + 1;
        blockEnd[b] = (b + 1) * blockSize;
        if (blockEnd[b] > n) blockEnd[b] = n;
        for (int i = blockStart[b]; i <= blockEnd[b]; ++i) blockId[i] = b;
    }

    // build blocks
    for (int b = 0; b < numBlocks; ++b) {
        int s = blockStart[b], e = blockEnd[b];
        int len = e - s + 1;
        blkLen[b] = len;
        blkArr[b] = (int*)malloc(len * sizeof(int));
        if (!blkArr[b] && len>0) { printf("Memory fail\n"); return 1; }
        int idx2 = 0;
        for (int i = s; i <= e; ++i) blkArr[b][idx2++] = arr[i];
        qsort(blkArr[b], len, sizeof(int), int_cmp);
    }

    printf("\nNow enter %d operations (one per line):\n", q);
    fflush(stdout);

    // process q operations
    for (int op = 0; op < q; ++op) {
        char type;
        if (scanf(" %c", &type) != 1) break;
        if (type == 'U') {
            int idx, val;
            if (scanf("%d %d", &idx, &val) != 2) { printf("Bad update input, skipping\n"); continue; }
            if (idx < 1 || idx > n) { printf("Update idx out of range, skipping\n"); continue; }
            do_update(idx, val);
            printf("Updated roll %d to %d\n", idx, val);
            fflush(stdout);
        } else if (type == 'Q') {
            int L, R, k;
            if (scanf("%d %d %d", &L, &R, &k) != 3) { printf("Bad query input, skipping\n"); continue; }
            if (L < 1) L = 1;
            if (R > n) R = n;
            if (L > R || k <= 0 || k > (R - L + 1)) {
                printf("Not found\n");
                fflush(stdout);
                continue;
            }
            int ans = do_query(L, R, k);
            if (ans == -1) printf("Not found\n");
            else printf("Answer: %d\n", ans);
            fflush(stdout);
        } else {
            // consume rest of line and ignore
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("Unknown operation '%c' ignored\n", type);
            fflush(stdout);
        }
    }

    printf("\nAll operations processed. Exiting.\n");
    // cleanup
    for (int b = 0; b < numBlocks; ++b) if (blkArr[b]) free(blkArr[b]);
    return 0;
}
