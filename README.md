# Range k-th Ranked Student Score with Point Updates

This repository contains a C implementation for maintaining **student scores** with **point updates** and answering **range k-th ranked queries** efficiently.  
The project is based on a **case study of student leaderboards**, where scores change over time and we need to find the rank of students within a range of roll numbers.

---

## Problem Statement

Consider an array of student scores indexed by roll numbers `1..N`. Scores are updated over time. You must support two operations:

- **Update (U idx val):** Set the score of the student at roll number `idx` to `val`.  
- **Query (Q L R k):** Among students with roll numbers in the inclusive range `[L, R]`, return the **k-th smallest score**.

This models ranking queries in **academic systems, leaderboards, and analytics platforms**.

---

## Input Format
n q
A1 A2 ... An
(op_1)
(op_2)
...
(op_q)


- First line: two integers `n` (number of students) and `q` (number of operations).  
- Second line: `n` integers `A1 .. An` — initial scores for roll numbers `1..n`.  
- Next `q` lines: each operation is either:
  - `U idx val` → update operation (1 ≤ idx ≤ n)  
  - `Q L R k` → query operation (1 ≤ L ≤ R ≤ n, 1 ≤ k ≤ R-L+1)

---

## Output Format
- For each `Q` operation, print the **k-th smallest score** in `[L, R]`.  
- If `k` is invalid (e.g., `k > R-L+1`), print `Not found`.

---

## Example

### Input

8 5
30 10 50 40 20 60 70 15
Q 1 5 2
U 3 12
Q 1 5 2
Q 4 8 3
Q 1 8 5


### Step-by-Step Explanation
1. Query (1 5 2): Subarray `[30,10,50,40,20]` → sorted `[10,20,30,40,50]` → 2nd smallest = **20**  
2. Update (3 12): Change roll no. 3 score from 50 → 12 → Array `[30,10,12,40,20,60,70,15]`  
3. Query (1 5 2): Subarray `[30,10,12,40,20]` → sorted `[10,12,20,30,40]` → 2nd smallest = **12**  
4. Query (4 8 3): Subarray `[40,20,60,70,15]` → sorted `[15,20,40,60,70]` → 3rd smallest = **40**  
5. Query (1 8 5): Entire array sorted `[10,12,15,20,30,40,60,70]` → 5th smallest = **30**

### Output

20
12
40
30

---

## Constraints
- 1 ≤ n, q ≤ 200,000  
- 1 ≤ Ai, val ≤ 1e9  
- Efficient solution required: ~O(√N log N) per operation (sqrt decomposition + coordinate compression).

---

## How to Compile and Run

### Compile
```bash
gcc -std=c11 kth_smallest_queries.c -O2 -o main
```
### Run 
```bash
./kth_smallest_queries
```
You will be prompted to enter:

n q

n scores (space/newline separated)

q operations (U/Q format as described)



Case Study Relevance

This project directly demonstrates the student ranking case study:

Scores change dynamically with updates.

Queries help find relative ranks (like k-th smallest score) in any roll number range.

Real-world relevance in leaderboards, merit list generation, and performance analytics.


