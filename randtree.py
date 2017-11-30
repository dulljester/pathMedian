import random

def f( m, n ):
    return random.randint(m,n)

parent,rank = {},{}
C = 1000
cc = 0

def find( x ):
    if parent[x] == x:
        return x
    parent[x] = find(parent[x])
    return parent[x]

def link(x,y):
    if rank[x] > rank[y]:
        parent[y] = x
    else:
        parent[x] = y
        if rank[x] == rank[y]:
            rank[y] = rank[y]+1

if __name__ == '__main__':
    ts = 7
    for t in range(ts):
        parent,rank = {},{}
        n = f(1000,1000000)
        # n = f(5,7)
        C = f(1,1000)
        print n,C
        for i in range(n):
            print f(1,C),
            parent[i] = i
            rank[i] = 0
        print ""
        size = 1
        cc = n
        k = 0
        while cc > 1:
            i = f(0,n-1)
            j = f(0,n-1)
            if find(i) != find(j):
                print i,j
                cc = cc-1
                link(find(i),find(j))
                k = k+1
        if k != n-1:
            x = 1/0
        if n <= 100:
            qr = n*(n+1)/2
            print qr
            for i in range(n):
                for j in range(i,n):
                    print i,j
        else:
            qr = f(10000,100000)
            print qr
            for k in range(qr):
                while True:
                    i,j = f(0,n-1),f(0,n-1)
                    if i != j:
                        break
                print i,j

