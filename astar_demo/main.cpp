#include <opencv2/opencv.hpp>
#include <vector>
#include <queue>
#include <cmath>
#include <iostream>
#include <algorithm>
using namespace cv;
using namespace std;

// 改名字：AstarNode，不和cv::Node冲突
struct AstarNode
{
    int x, y;
    double g;
    double h;
    double f;
    AstarNode* parent;

    AstarNode(int x_, int y_) : x(x_), y(y_), g(0), h(0), f(0), parent(nullptr) {}
};

struct CompareNode
{
    bool operator()(AstarNode* a, AstarNode* b)
    {
        return a->f > b->f;
    }
};

double heuristic(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

vector<Point> astar(
    const vector<vector<int>>& grid,
    Point start,
    Point goal,
    Mat& vis_img,
    int cell_size = 20
)
{
    int rows = grid.size();
    int cols = grid[0].size();
    vector<vector<bool>> closed(rows, vector<bool>(cols, false));
    priority_queue<AstarNode*, vector<AstarNode*>, CompareNode> open_list;
    vector<vector<AstarNode*>> node_map(rows, vector<AstarNode*>(cols, nullptr));

    AstarNode* start_node = new AstarNode(start.x, start.y);
    start_node->h = heuristic(start.x, start.y, goal.x, goal.y);
    start_node->f = start_node->g + start_node->h;
    open_list.push(start_node);
    node_map[start.y][start.x] = start_node;

    vector<Point> dirs = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};

    while (!open_list.empty())
    {
        AstarNode* curr = open_list.top();
        open_list.pop();

        if (curr->x == goal.x && curr->y == goal.y)
        {
            vector<Point> path;
            AstarNode* p = curr;
            while(p != nullptr)
            {
                path.emplace_back(p->x, p->y);
                p = p->parent;
            }
            reverse(path.begin(), path.end());
            return path;
        }

        if (closed[curr->y][curr->x]) continue;
        closed[curr->y][curr->x] = true;

        rectangle(vis_img,
            Point(curr->x*cell_size, curr->y*cell_size),
            Point((curr->x+1)*cell_size, (curr->y+1)*cell_size),
            Scalar(180,180,180), -1);
        imshow("A* Visual", vis_img);
        waitKey(30);

        for(auto& d : dirs)
        {
            int nx = curr->x + d.x;
            int ny = curr->y + d.y;
            if(nx<0 || nx>=cols || ny<0 || ny>=rows) continue;
            if(grid[ny][nx]==1) continue;
            if(closed[ny][nx]) continue;

            double cost = (d.x !=0 && d.y !=0) ? 1.414 : 1.0;
            double new_g = curr->g + cost;

            AstarNode* neighbor = node_map[ny][nx];
            if(neighbor == nullptr)
            {
                neighbor = new AstarNode(nx, ny);
                neighbor->g = new_g;
                neighbor->h = heuristic(nx, ny, goal.x, goal.y);
                neighbor->f = neighbor->g + neighbor->h;
                neighbor->parent = curr;
                node_map[ny][nx] = neighbor;
                open_list.push(neighbor);
                rectangle(vis_img,
                    Point(nx*cell_size, ny*cell_size),
                    Point((nx+1)*cell_size, (ny+1)*cell_size),
                    Scalar(220,180,80), -1);
            }
            else if(new_g < neighbor->g)
            {
                neighbor->g = new_g;
                neighbor->f = neighbor->g + neighbor->h;
                neighbor->parent = curr;
            }
        }
    }
    return {};
}

int main()
{
    vector<vector<int>> grid = {
        {0,0,0,0,0,0,0,0,0,0},
        {0,1,1,1,0,1,1,1,1,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,1,1,0,1,1,1,1,0,0},
        {0,0,0,0,0,0,0,1,0,0},
        {0,1,1,1,1,1,0,1,0,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,1,1,1,1,1,1,1,0},
        {0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0},
    };
    int cell = 40;
    int rows = grid.size();
    int cols = grid[0].size();
    Mat img(rows*cell, cols*cell, CV_8UC3, Scalar(255,255,255));

    for(int y=0;y<rows;y++){
        for(int x=0;x<cols;x++){
            if(grid[y][x]==1){
                rectangle(img, Point(x*cell,y*cell), Point((x+1)*cell,(y+1)*cell), Scalar(0,0,0), -1);
            }
            rectangle(img, Point(x*cell,y*cell), Point((x+1)*cell,(y+1)*cell), Scalar(100,100,100),1);
        }
    }

    Point start(0,0);
    Point goal(9,9);
    rectangle(img, Point(start.x*cell,start.y*cell), Point((start.x+1)*cell,(start.y+1)*cell), Scalar(0,200,0),-1);
    rectangle(img, Point(goal.x*cell,goal.y*cell), Point((goal.x+1)*cell,(goal.y+1)*cell), Scalar(0,0,200),-1);

    vector<Point> path = astar(grid, start, goal, img, cell);

    for(int i=1;i<path.size();i++){
        line(img,
            Point(path[i-1].x*cell+cell/2, path[i-1].y*cell+cell/2),
            Point(path[i].x*cell+cell/2, path[i].y*cell+cell/2),
            Scalar(255,0,0),3);
    }

    imwrite("astar_result.png", img);
    imshow("A* Visual", img);
    cout << "寻路完成，图片已保存 astar_result.png" << endl;
    waitKey(0);
    destroyAllWindows();
    return 0;
}
