#include "visualizer.h"
#include "graph.h"
#include "knapsack.h"
#include <raylib.h>
#include <string.h>
#include <stdio.h>

/* ── layout ─────────────────────────────────────────── */
#define WIN_W   1200
#define WIN_H    700
#define DIV      700      /* graph canvas / info panel split */
#define NODE_R    22

/* ── theme ───────────────────────────────────────────── */
#define C_BG      (Color){12,  12,  22,  255}
#define C_PANEL   (Color){22,  24,  38,  255}
#define C_EDGE    (Color){70,  72,  95,  255}
#define C_NODE    (Color){55,  75,  130, 255}
#define C_VISIT   (Color){30,  185, 80,  255}
#define C_CURR    (Color){255, 200, 0,   255}
#define C_PATH    (Color){0,   190, 255, 255}
#define C_MST_E   (Color){0,   230, 100, 255}
#define C_REJECT  (Color){215, 55,  55,  255}
#define C_BF      (Color){220, 70,  70,  255}
#define C_BB      (Color){70,  220, 110, 255}
#define C_DIM     (Color){130, 132, 150, 255}
#define C_ORANGE  (Color){255, 165, 0,   255}
#define C_CYAN    (Color){0,   190, 255, 255}

/* ── city screen positions (geographic layout) ────────── */
static const Vector2 POS[6] = {
    {335, 520},   /* 0 Bangalore */
    {515, 498},   /* 1 Chennai   */
    {388, 345},   /* 2 Hyderabad */
    {168, 352},   /* 3 Mumbai    */
    {204, 432},   /* 4 Pune      */
    {362, 118},   /* 5 Delhi     */
};

/* ── shared graph ────────────────────────────────────── */
static Graph g;

/* ── view state machine ───────────────────────────────── */
typedef enum { V_MENU, V_DIJKSTRA, V_MST, V_TSP, V_KNAPSACK } View;
static View view = V_MENU;

/* ══════════════════════════════════════════════════════
   DIJKSTRA  step capture
══════════════════════════════════════════════════════ */
typedef struct { int dist[6], vis[6], par[6], cur; } DS;
static DS  dsteps[10];
static int dcount, dcur;

static void precompute_dijkstra(void) {
    int n = g.num_cities;
    int dist[6], vis[6], par[6];
    for (int i = 0; i < n; i++) { dist[i]=INF; vis[i]=0; par[i]=-1; }
    dist[0] = 0; dcount = 0;

    for (int iter = 0; iter < n-1; iter++) {
        int u = -1;
        for (int v = 0; v < n; v++)
            if (!vis[v] && (u==-1 || dist[v]<dist[u])) u = v;
        if (u==-1 || dist[u]==INF) break;
        vis[u] = 1;

        DS *s = &dsteps[dcount++];
        memcpy(s->dist, dist, n*sizeof(int));
        memcpy(s->vis,  vis,  n*sizeof(int));
        memcpy(s->par,  par,  n*sizeof(int));
        s->cur = u;

        for (int v = 0; v < n; v++)
            if (!vis[v] && g.dist[u][v]!=INF &&
                dist[u]+g.dist[u][v] < dist[v]) {
                dist[v] = dist[u]+g.dist[u][v]; par[v] = u;
            }
    }
    DS *s = &dsteps[dcount++];
    memcpy(s->dist, dist, n*sizeof(int));
    memcpy(s->vis,  vis,  n*sizeof(int));
    memcpy(s->par,  par,  n*sizeof(int));
    s->cur = -1; dcur = 0;
}

/* ══════════════════════════════════════════════════════
   KRUSKAL  step capture
══════════════════════════════════════════════════════ */
typedef struct { int u, v, w, ok; } KE;
static KE  ksteps[30];
static int kcount, kcur, kmst_cost;

static int kpar[6], krnk[6];
static int kfind(int x){ return kpar[x]==x?x:(kpar[x]=kfind(kpar[x])); }
static void kunion(int a,int b){
    a=kfind(a);b=kfind(b); if(a==b) return;
    if(krnk[a]<krnk[b]){int t=a;a=b;b=t;}
    kpar[b]=a; if(krnk[a]==krnk[b]) krnk[a]++;
}

static void precompute_kruskal(void) {
    int n = g.num_cities;
    typedef struct{int u,v,w;} RE;
    RE edges[30]; int em = 0;
    for (int u=0;u<n;u++)
        for (int v=u+1;v<n;v++)
            if (g.dist[u][v]!=INF)
                edges[em++]=(RE){u,v,g.dist[u][v]};

    for (int i=1;i<em;i++) {        /* insertion sort */
        RE key=edges[i]; int j=i-1;
        while(j>=0&&edges[j].w>key.w){edges[j+1]=edges[j];j--;}
        edges[j+1]=key;
    }
    for (int i=0;i<n;i++){kpar[i]=i;krnk[i]=0;}
    kcount=0; kmst_cost=0;

    for (int i=0;i<em;i++) {
        int ok = (kfind(edges[i].u)!=kfind(edges[i].v));
        ksteps[kcount++]=(KE){edges[i].u,edges[i].v,edges[i].w,ok};
        if (ok){ kunion(edges[i].u,edges[i].v); kmst_cost+=edges[i].w; }
    }
    kcur = 0;
}

/* ══════════════════════════════════════════════════════
   TSP  brute-force + B&B  (self-contained)
══════════════════════════════════════════════════════ */
static int tpath[6], tvis[6], tbest, tbest_p[6], tnodes;

static void bf_run(int dep, int cur, int cost, int n) {
    tnodes++;
    if (dep==n) {
        int tot=cost+g.dist[cur][tpath[0]];
        if(tot<tbest){tbest=tot;memcpy(tbest_p,tpath,n*sizeof(int));}
        return;
    }
    for (int v=0;v<n;v++)
        if (!tvis[v]&&g.dist[cur][v]!=INF) {
            tvis[v]=1; tpath[dep]=v;
            bf_run(dep+1,v,cost+g.dist[cur][v],n);
            tvis[v]=0;
        }
}

static int bpath[6], bvis[6], bbest, bbest_p[6], bnodes;

static int lb_bb(int p[],int dep,int vis[],int n) {
    int b=0;
    for(int i=0;i<dep-1;i++) b+=g.dist[p[i]][p[i+1]];
    for(int v=0;v<n;v++){
        if(vis[v]) continue;
        int m=INF;
        for(int u=0;u<n;u++)
            if(u!=v&&g.dist[v][u]!=INF&&g.dist[v][u]<m) m=g.dist[v][u];
        if(m!=INF) b+=m;
    }
    return b;
}

static void bb_run(int dep, int cur, int cost, int n) {
    bnodes++;
    if (dep==n) {
        int tot=cost+g.dist[cur][bpath[0]];
        if(tot<bbest){bbest=tot;memcpy(bbest_p,bpath,n*sizeof(int));}
        return;
    }
    for (int v=0;v<n;v++){
        if(bvis[v]||g.dist[cur][v]==INF) continue;
        bvis[v]=1; bpath[dep]=v;
        int nc=cost+g.dist[cur][v];
        if(nc+lb_bb(bpath,dep+1,bvis,n)<bbest)
            bb_run(dep+1,v,nc,n);
        bvis[v]=0;
    }
}

static int bf_tour[6], bf_cost, bf_nn;
static int bb_tour[6], bb_cost, bb_nn;
static int tsp_show; /* 0=BF  1=B&B */

static void precompute_tsp(void) {
    int n=g.num_cities;
    tbest=INF; tnodes=0;
    memset(tvis,0,sizeof(tvis)); tvis[0]=1; tpath[0]=0;
    bf_run(1,0,0,n);
    bf_cost=tbest; memcpy(bf_tour,tbest_p,n*sizeof(int)); bf_nn=tnodes;

    bbest=INF; bnodes=0;
    memset(bvis,0,sizeof(bvis)); bvis[0]=1; bpath[0]=0;
    bb_run(1,0,0,n);
    bb_cost=bbest; memcpy(bb_tour,bbest_p,n*sizeof(int)); bb_nn=bnodes;
    tsp_show=0;
}

/* ══════════════════════════════════════════════════════
   KNAPSACK  DP + Greedy  (self-contained)
══════════════════════════════════════════════════════ */
#define KCAP 40
static CargoItem  items[MAX_ITEMS];
static int        nitems;
static int        dp_sel[MAX_ITEMS], dp_tv, dp_tw;
static double     gr_frac_orig[MAX_ITEMS]; /* indexed to items[] order */
static double     gr_tv; static int gr_tw;
static int        kmode; /* 0=DP  1=Greedy */

static void precompute_knapsack(void) {
    knapsack_load_sample(items, &nitems);
    int n=nitems, W=KCAP;

    /* 0/1 DP */
    static int dp[MAX_ITEMS+1][MAX_CAPACITY+1];
    for(int i=0;i<=n;i++)
        for(int w=0;w<=W;w++) {
            if(!i||!w) dp[i][w]=0;
            else if(items[i-1].weight<=w) {
                int tk=items[i-1].value+dp[i-1][w-items[i-1].weight];
                int sk=dp[i-1][w];
                dp[i][w]=tk>sk?tk:sk;
            } else dp[i][w]=dp[i-1][w];
        }
    memset(dp_sel,0,sizeof(dp_sel));
    { int w=W;
      for(int i=n;i>0;i--)
          if(dp[i][w]!=dp[i-1][w]){dp_sel[i-1]=1;w-=items[i-1].weight;}
    }
    dp_tv=dp[n][W]; dp_tw=0;
    for(int i=0;i<n;i++) if(dp_sel[i]) dp_tw+=items[i].weight;

    /* Greedy fractional — sort by ratio */
    CargoItem gr_sorted[MAX_ITEMS];
    double    gr_frac[MAX_ITEMS];
    memcpy(gr_sorted,items,n*sizeof(CargoItem));
    for(int i=1;i<n;i++){
        CargoItem key=gr_sorted[i];
        double kr=(double)key.value/key.weight; int j=i-1;
        while(j>=0&&(double)gr_sorted[j].value/gr_sorted[j].weight<kr)
            {gr_sorted[j+1]=gr_sorted[j];j--;}
        gr_sorted[j+1]=key;
    }
    memset(gr_frac,0,sizeof(gr_frac));
    int rem=W; gr_tv=0; gr_tw=0;
    for(int i=0;i<n&&rem>0;i++){
        if(gr_sorted[i].weight<=rem){
            gr_frac[i]=1.0; rem-=gr_sorted[i].weight;
            gr_tv+=gr_sorted[i].value; gr_tw+=gr_sorted[i].weight;
        } else {
            gr_frac[i]=(double)rem/gr_sorted[i].weight;
            gr_tv+=gr_frac[i]*gr_sorted[i].value;
            gr_tw+=rem; rem=0;
        }
    }
    /* map greedy fractions back to original items[] order */
    memset(gr_frac_orig,0,sizeof(gr_frac_orig));
    for(int j=0;j<n;j++)
        if(gr_frac[j]>0)
            for(int i=0;i<n;i++)
                if(strcmp(items[i].name,gr_sorted[j].name)==0)
                    {gr_frac_orig[i]=gr_frac[j];break;}
    kmode=0;
}

/* ══════════════════════════════════════════════════════
   DRAWING  HELPERS
══════════════════════════════════════════════════════ */
static void draw_edge_styled(int u, int v, Color col, float thick) {
    DrawLineEx(POS[u], POS[v], thick, col);
    Vector2 mid={(POS[u].x+POS[v].x)/2,(POS[u].y+POS[v].y)/2};
    DrawText(TextFormat("%d",g.dist[u][v]),
             (int)mid.x-10,(int)mid.y-8, 13, C_DIM);
}

static void draw_all_edges(void) {
    int n=g.num_cities;
    for(int u=0;u<n;u++)
        for(int v=u+1;v<n;v++)
            if(g.dist[u][v]!=INF)
                draw_edge_styled(u,v,C_EDGE,1.5f);
}

static void draw_node(int i, Color col, const char *lbl) {
    DrawCircleV(POS[i], NODE_R, col);
    DrawCircleLines((int)POS[i].x,(int)POS[i].y,NODE_R,WHITE);
    if(lbl) DrawText(lbl,
                     (int)POS[i].x-(int)MeasureText(lbl,15)/2,
                     (int)POS[i].y-9, 15, BLACK);
    DrawText(g.cities[i].name,
             (int)POS[i].x-(int)MeasureText(g.cities[i].name,12)/2,
             (int)POS[i].y+NODE_R+4, 12, WHITE);
}

static void draw_info_panel(void) {
    DrawRectangle(DIV+8,8,WIN_W-DIV-16,WIN_H-16,C_PANEL);
}

static void draw_ctrl_bar(const char *txt) {
    DrawRectangle(0,WIN_H-34,DIV,34,(Color){28,30,48,255});
    DrawText(txt,10,WIN_H-24,13,C_DIM);
}

/* ══════════════════════════════════════════════════════
   DIJKSTRA  VIEW
══════════════════════════════════════════════════════ */
static void draw_dijkstra(void) {
    DS *s = &dsteps[dcur];
    int n = g.num_cities;

    draw_all_edges();
    for(int v=0;v<n;v++)           /* highlight path edges */
        if(s->par[v]!=-1)
            draw_edge_styled(s->par[v],v,C_PATH,3.5f);

    for(int i=0;i<n;i++) {
        Color col = (i==s->cur) ? C_CURR :
                    s->vis[i]   ? C_VISIT : C_NODE;
        char lbl[8];
        if(s->dist[i]==INF) snprintf(lbl,sizeof(lbl),"?");
        else                snprintf(lbl,sizeof(lbl),"%d",s->dist[i]);
        draw_node(i,col,lbl);
    }

    draw_info_panel();
    int px=DIV+22, py=22;
    DrawText("DIJKSTRA", px,py,24,C_CYAN); py+=32;
    DrawText("Single-Source Shortest Path", px,py,13,C_DIM); py+=18;
    DrawText("Complexity: O(V^2)", px,py,15,WHITE); py+=28;
    DrawText(TextFormat("Source : %s",g.cities[0].name),px,py,15,C_CURR); py+=20;
    DrawText(TextFormat("Step   : %d / %d",dcur+1,dcount),px,py,15,WHITE); py+=24;
    DrawLine(px,py,WIN_W-22,py,C_EDGE); py+=10;

    DrawText("City",        px,    py,14,(Color){180,180,200,255});
    DrawText("Dist(km)",    px+170,py,14,(Color){180,180,200,255}); py+=20;

    for(int i=0;i<n;i++){
        if(i==0) continue;
        Color tc = s->dist[i]==INF ? C_DIM :
                   s->vis[i]        ? C_VISIT : WHITE;
        DrawText(g.cities[i].name,
                 s->dist[i]==INF ? px : px, py, 15, tc);
        DrawText(s->dist[i]==INF ? "INF" : TextFormat("%d",s->dist[i]),
                 px+170, py, 15, tc);
        py+=20;
    }

    /* legend */
    py=WIN_H-108;
    DrawText("Legend:", px,py,13,C_DIM); py+=16;
    DrawCircle(px+8,py+7,8,C_CURR);  DrawText("Current node",px+22,py,13,WHITE); py+=20;
    DrawCircle(px+8,py+7,8,C_VISIT); DrawText("Visited",     px+22,py,13,WHITE); py+=20;
    DrawCircle(px+8,py+7,8,C_NODE);  DrawText("Unvisited",   px+22,py,13,WHITE);

    draw_ctrl_bar("SPACE: next step    R: reset    ESC: menu");
}

/* ══════════════════════════════════════════════════════
   MST (KRUSKAL)  VIEW
══════════════════════════════════════════════════════ */
static void draw_mst(void) {
    int n=g.num_cities;
    draw_all_edges();

    for(int i=0;i<kcur&&i<kcount;i++)
        if(ksteps[i].ok)
            draw_edge_styled(ksteps[i].u,ksteps[i].v,C_MST_E,4.5f);

    if(kcur<kcount) {
        KE *k=&ksteps[kcur];
        draw_edge_styled(k->u,k->v,k->ok?C_MST_E:C_REJECT,5.5f);
    }
    for(int i=0;i<n;i++) draw_node(i,C_NODE,NULL);

    draw_info_panel();
    int px=DIV+22, py=22;
    DrawText("KRUSKAL'S MST",px,py,22,C_MST_E); py+=30;
    DrawText("Minimum Spanning Tree",px,py,13,C_DIM); py+=18;
    DrawText("Complexity: O(E log E)",px,py,15,WHITE); py+=24;
    DrawText(TextFormat("Step: %d / %d",kcur+1,kcount),px,py,15,WHITE); py+=22;
    DrawLine(px,py,WIN_W-22,py,C_EDGE); py+=10;

    DrawText("Edge",  px,    py,13,(Color){180,180,200,255});
    DrawText("Dist",  px+175,py,13,(Color){180,180,200,255});
    DrawText("Result",px+235,py,13,(Color){180,180,200,255}); py+=18;

    int running=0;
    for(int i=0;i<=kcur&&i<kcount;i++) {
        KE *k=&ksteps[i]; int is_cur=(i==kcur);
        Color tc=k->ok?C_MST_E:C_REJECT;
        if(is_cur) DrawRectangle(px-3,py-2,WIN_W-DIV-30,18,(Color){40,42,65,255});
        DrawText(TextFormat("%s-%s",g.cities[k->u].name,g.cities[k->v].name),
                 px,    py, 13, is_cur?WHITE:tc);
        DrawText(TextFormat("%d",k->w), px+175,py,13,is_cur?WHITE:tc);
        DrawText(k->ok?"ADD":"SKIP",    px+235,py,13,tc);
        py+=18;
        if(k->ok) running+=k->w;
    }
    DrawLine(px,py+2,WIN_W-22,py+2,C_EDGE);
    DrawText(TextFormat("MST cost so far: %d km",running),px,py+8,15,C_MST_E);
    if(kcur>=kcount-1)
        DrawText(TextFormat("FINAL MST = %d km",kmst_cost),px,py+28,18,C_CURR);

    int ly=WIN_H-52;
    DrawRectangle(px,ly,12,12,C_MST_E);
    DrawText("Accepted",px+16,ly,13,WHITE);
    DrawRectangle(px+110,ly,12,12,C_REJECT);
    DrawText("Rejected (cycle)",px+126,ly,13,WHITE);

    draw_ctrl_bar("SPACE: next edge    R: reset    ESC: menu");
}

/* ══════════════════════════════════════════════════════
   TSP  VIEW
══════════════════════════════════════════════════════ */
static void draw_tsp(void) {
    int   n    = g.num_cities;
    int  *tour = tsp_show==0 ? bf_tour : bb_tour;
    int   cost = tsp_show==0 ? bf_cost : bb_cost;
    int   nn   = tsp_show==0 ? bf_nn   : bb_nn;
    Color tcol = tsp_show==0 ? C_BF    : C_BB;

    draw_all_edges();
    for(int i=0;i<n;i++)
        draw_edge_styled(tour[i],tour[(i+1)%n],tcol,4.5f);
    for(int i=0;i<n;i++) draw_node(i,C_NODE,NULL);

    draw_info_panel();
    int px=DIV+22, py=22;
    DrawText("TRAVELLING SALESMAN",px,py,19,C_ORANGE); py+=28;
    DrawText("NP-Hard — no poly-time solution",px,py,13,C_DIM); py+=22;
    DrawLine(px,py,WIN_W-22,py,C_EDGE); py+=12;

    Color bfc = tsp_show==0?WHITE:C_DIM;
    Color bbc = tsp_show==1?WHITE:C_DIM;

    DrawText("BRUTE FORCE",px,py,16,tsp_show==0?C_BF:C_DIM); py+=20;
    DrawText(TextFormat("  Cost   : %d km",bf_cost),  px,py,14,bfc); py+=18;
    DrawText(TextFormat("  Nodes  : %d explored",bf_nn),px,py,14,bfc); py+=18;
    DrawText(TextFormat("  O(n!)  : %d! perms",n),     px,py,14,bfc); py+=26;

    DrawText("BRANCH & BOUND",px,py,16,tsp_show==1?C_BB:C_DIM); py+=20;
    DrawText(TextFormat("  Cost   : %d km",bb_cost),  px,py,14,bbc); py+=18;
    DrawText(TextFormat("  Nodes  : %d explored",bb_nn),px,py,14,bbc); py+=18;
    int pct = bf_nn>0 ? (100*(bf_nn-bb_nn)/bf_nn) : 0;
    DrawText(TextFormat("  Pruned : %d%% of BF work",pct),px,py,14,bbc); py+=26;

    DrawLine(px,py,WIN_W-22,py,C_EDGE); py+=10;
    int same = (bf_cost==bb_cost);
    DrawText(same?"Same optimal tour: YES":"Same optimal tour: NO",
             px,py,15,same?C_VISIT:C_REJECT); py+=24;
    DrawText(TextFormat("Showing: %s  | Cost: %d km",
             tsp_show==0?"Brute Force":"Branch&Bound",cost),
             px,py,14,tcol); py+=20;
    DrawText(TextFormat("Nodes visited: %d",nn),px,py,14,tcol);

    /* tour path text */
    int ty=WIN_H-80;
    DrawText("Tour:",px,ty,14,C_DIM); ty+=16;
    int tx=px;
    for(int i=0;i<n;i++){
        const char *nm=g.cities[tour[i]].name;
        DrawText(nm,tx,ty,12,tcol);
        tx+=MeasureText(nm,12)+4;
        if(i<n-1){DrawText("->",tx,ty,12,C_DIM);tx+=18;}
    }

    draw_ctrl_bar("1: Brute Force    2: Branch & Bound    ESC: menu");
}

/* ══════════════════════════════════════════════════════
   KNAPSACK  VIEW
══════════════════════════════════════════════════════ */
static void draw_knapsack(void) {
    int n=nitems;
    int bx=20, bar_max_h=220, bar_base=600;
    int bar_w=(DIV-bx*2)/n - 4;

    /* title */
    DrawText(kmode==0 ? "0/1 KNAPSACK  (DP) - OPTIMAL"
                      : "FRACTIONAL KNAPSACK  (Greedy)",
             bx,18,20,kmode==0?C_CYAN:C_ORANGE);
    DrawText(kmode==0 ? "Items must be taken whole or left"
                      : "Items can be split into fractions",
             bx,44,13,C_DIM);

    /* capacity bar */
    int used = kmode==0 ? dp_tw : gr_tw;
    DrawRectangle(bx,66,DIV-bx*2,20,(Color){35,35,55,255});
    float frac=(float)used/KCAP; if(frac>1.0f)frac=1.0f;
    DrawRectangle(bx,66,(int)((DIV-bx*2)*frac),20,kmode==0?C_CYAN:C_ORANGE);
    DrawRectangleLines(bx,66,DIV-bx*2,20,WHITE);
    DrawText(TextFormat("Capacity: %d / %d units",used,KCAP),
             bx+4,70,13,BLACK);

    DrawText("Bar height = value  |  Bar width = weight",bx,96,12,C_DIM);
    DrawLine(bx,bar_base+2,DIV-bx,bar_base+2,C_DIM);

    int max_v=0;
    for(int i=0;i<n;i++) if(items[i].value>max_v) max_v=items[i].value;

    for(int i=0;i<n;i++){
        int bxi=bx+i*(bar_w+4);
        int bhi=(int)((float)items[i].value/max_v*bar_max_h);
        Color bc;
        if(kmode==0){
            bc=dp_sel[i]?C_CYAN:(Color){55,55,75,255};
        } else {
            double fr=gr_frac_orig[i];
            if(fr>=1.0)       bc=C_ORANGE;
            else if(fr>0.0)   bc=(Color){255,100,50,255};
            else              bc=(Color){55,55,75,255};
        }
        DrawRectangle(bxi,bar_base-bhi,bar_w,bhi,bc);
        DrawRectangleLines(bxi,bar_base-bhi,bar_w,bhi,WHITE);
        DrawText(TextFormat("v%d",items[i].value),bxi+2,bar_base-bhi-14,10,WHITE);
        DrawText(TextFormat("w%d",items[i].weight),bxi+2,bar_base+5,10,C_DIM);
    }

    /* info panel */
    draw_info_panel();
    int px=DIV+22, py=22;
    DrawText("KNAPSACK",px,py,24,C_ORANGE); py+=32;
    DrawText(TextFormat("Capacity: %d units",KCAP),px,py,15,WHITE); py+=22;
    DrawLine(px,py,WIN_W-22,py,C_EDGE); py+=10;

    DrawText("Item",   px,    py,13,(Color){180,180,200,255});
    DrawText("W",      px+178,py,13,(Color){180,180,200,255});
    DrawText("V",      px+208,py,13,(Color){180,180,200,255}); py+=18;

    for(int i=0;i<n;i++){
        Color tc;
        const char *tag="";
        if(kmode==0){
            tc=dp_sel[i]?C_CYAN:C_DIM; if(dp_sel[i]) tag="[+]";
        } else {
            double fr=gr_frac_orig[i];
            if(fr>=1.0)     {tc=C_ORANGE; tag="[full]";}
            else if(fr>0.0) {tc=(Color){255,120,60,255}; tag="[part]";}
            else            {tc=C_DIM;}
        }
        DrawText(items[i].name,            px,    py,13,tc);
        DrawText(TextFormat("%d",items[i].weight),px+178,py,13,tc);
        DrawText(TextFormat("%d",items[i].value), px+208,py,13,tc);
        DrawText(tag,                      px+235,py,12,tc);
        py+=18;
    }

    DrawLine(px,py+2,WIN_W-22,py+2,C_EDGE); py+=14;
    if(kmode==0){
        DrawText(TextFormat("Weight used : %d / %d",dp_tw,KCAP),px,py,15,WHITE); py+=20;
        DrawText(TextFormat("Total value : Rs.%dk",dp_tv),px,py,18,C_CYAN); py+=22;
        DrawText("INTEGER OPTIMAL result",px,py,13,C_VISIT);
    } else {
        DrawText(TextFormat("Weight used : %d / %d",gr_tw,KCAP),px,py,15,WHITE); py+=20;
        DrawText(TextFormat("Total value : Rs.%.1fk",gr_tv),px,py,18,C_ORANGE); py+=22;
        DrawText("Uses fractions - not feasible for real cargo",px,py,12,C_REJECT);
    }

    draw_ctrl_bar("1: DP Optimal    2: Greedy Fractional    ESC: menu");
}

/* ══════════════════════════════════════════════════════
   MENU  VIEW
══════════════════════════════════════════════════════ */
static void draw_menu(void) {
    DrawText("SMART LOGISTICS VISUALIZER",
             WIN_W/2-220,80,32,WHITE);
    DrawText("Design & Analysis of Algorithms  |  Raylib GUI",
             WIN_W/2-220,118,17,C_DIM);
    DrawLine(100,148,WIN_W-100,148,(Color){55,58,80,255});

    const char *labels[4]={
        "1   Dijkstra  -  Shortest Path (step by step)",
        "2   Kruskal   -  MST edge by edge",
        "3   TSP       -  Brute Force vs Branch & Bound",
        "4   Knapsack  -  DP Optimal vs Greedy Fractional",
    };
    Color cols[4]={ C_CYAN, C_MST_E, C_ORANGE, C_ORANGE };

    for(int i=0;i<4;i++){
        Rectangle r={(float)(WIN_W/2-290),(float)(210+i*95),580,64};
        DrawRectangleRounded(r,0.15f,8,C_PANEL);
        DrawRectangleRoundedLinesEx(r,0.15f,8,1.5f,cols[i]);
        DrawText(labels[i],WIN_W/2-262,228+i*95,19,cols[i]);
    }
    DrawText("Press number key to enter a module   |   ESC to quit",
             WIN_W/2-240,WIN_H-44,14,C_DIM);
}

/* ══════════════════════════════════════════════════════
   PUBLIC  ENTRY  POINT
══════════════════════════════════════════════════════ */
void run_visualizer(void) {
    graph_load_sample(&g);
    precompute_dijkstra();
    precompute_kruskal();
    precompute_tsp();
    precompute_knapsack();

    InitWindow(WIN_W, WIN_H,
               "Smart Logistics Visualizer  |  DAA Mini Project");
    SetTargetFPS(60);
    view = V_MENU;

    while (!WindowShouldClose()) {
        /* ── input ── */
        if (IsKeyPressed(KEY_ESCAPE)) {
            if (view != V_MENU) view = V_MENU;
            else break;
        }
        switch (view) {
            case V_MENU:
                if(IsKeyPressed(KEY_ONE))  {view=V_DIJKSTRA;dcur=0;}
                if(IsKeyPressed(KEY_TWO))  {view=V_MST;kcur=0;}
                if(IsKeyPressed(KEY_THREE)){view=V_TSP;tsp_show=0;}
                if(IsKeyPressed(KEY_FOUR)) {view=V_KNAPSACK;kmode=0;}
                break;
            case V_DIJKSTRA:
                if(IsKeyPressed(KEY_SPACE)&&dcur<dcount-1) dcur++;
                if(IsKeyPressed(KEY_R)) dcur=0;
                break;
            case V_MST:
                if(IsKeyPressed(KEY_SPACE)&&kcur<kcount-1) kcur++;
                if(IsKeyPressed(KEY_R)) kcur=0;
                break;
            case V_TSP:
                if(IsKeyPressed(KEY_ONE))  tsp_show=0;
                if(IsKeyPressed(KEY_TWO))  tsp_show=1;
                break;
            case V_KNAPSACK:
                if(IsKeyPressed(KEY_ONE))  kmode=0;
                if(IsKeyPressed(KEY_TWO))  kmode=1;
                break;
        }

        /* ── draw ── */
        BeginDrawing();
        ClearBackground(C_BG);
        switch (view) {
            case V_MENU:     draw_menu();     break;
            case V_DIJKSTRA: draw_dijkstra(); break;
            case V_MST:      draw_mst();      break;
            case V_TSP:      draw_tsp();      break;
            case V_KNAPSACK: draw_knapsack(); break;
        }
        EndDrawing();
    }
    CloseWindow();
}
