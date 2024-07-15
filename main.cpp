//
// Created by ken on 24-7-13.
//
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>

using namespace cv;
using namespace std;

namespace fs = std::filesystem;

//计算size的众数
int ModeCalc(const vector<int>& sizes)
{
    map<int, int> frequency;
    for (int size : sizes)
    {
        frequency[size]++;
    }

    int mode = sizes[0];
    int maxCount = 0;
    for (const auto& [key, count] : frequency)
    {
        if (count > maxCount)
        {
            maxCount = count;
            mode = key;
        }
    }
    return mode;
}

// 直线连接
bool link0(const vector<vector<int>>& matrix, Point p1, Point p2) {
    if (p1 == p2)
        // 同一个点
        return false;
    if (p1.x != p2.x && p1.y != p2.y) // 既不在同一行也不在同一列
        return false;

    int inc;
    if (p1.x == p2.x) {
        // 在同一列
        inc = p1.y < p2.y ? 1 : -1;
        for (int y = p1.y + inc; y != p2.y; y += inc)
            if (matrix[y][p1.x] != -1) // 该列有非空格点则不能直接连接
                return false;
    } else {
        // 在同一行
        inc = p1.x < p2.x ? 1 : -1;
        for (int x = p1.x + inc; x != p2.x; x += inc)
            if (matrix[p1.y][x] != -1) // 该行有非空格点则不能直接连接
                return false;
    }
    return true;
}

// 一折连接
bool link1(const vector<vector<int>>& matrix, Point p1, Point p2, Point &t1) {
    if (p1.x == p2.x || p1.y == p2.y)
        // 在同一行或同一列
        return false;

    t1 = Point(p1.x, p2.y);
    if (matrix[t1.y][t1.x] == -1 && link0(matrix, p1, t1) && link0(matrix, t1, p2))
        return true;

    t1 = Point(p2.x, p1.y);
    if (matrix[t1.y][t1.x] == -1 && link0(matrix, p1, t1) && link0(matrix, t1, p2))
        return true;

    return false;
}

// 二折连接
bool link2(const vector<vector<int>>& matrix, Point p1, Point p2, Point &t1, Point &t2) {
    Point c1, c2;
    int len = 9999;

    c1.x = p1.x;
    // 沿Y轴检查
    for (c1.y = 0; c1.y < matrix.size(); c1.y++) {
        if (c1.y == p1.y || c1.y == p2.y)
            continue;
        if (matrix[c1.y][c1.x] == -1 && link0(matrix, p1, c1) && link1(matrix, c1, p2, c2) &&
            (abs(c1.y - p1.y) + abs(p1.x - p2.x) + abs(c2.y - p2.y) < len)) {
            t1 = c1;
            t2 = c2;
            len = abs(c1.y - p1.y) + abs(p1.x - p2.x) + abs(c2.y - p2.y);
        }
    }

    c1.y = p1.y;
    // 沿X轴检查
    for (c1.x = 0; c1.x < matrix[0].size(); c1.x++) {
        if (c1.x == p1.x || c1.x == p2.x)
            continue;
        if (matrix[c1.y][c1.x] == -1 && link0(matrix, p1, c1) && link1(matrix, c1, p2, c2) &&
            (abs(c1.x - p1.x) + abs(p1.y - p2.y) + abs(c2.x - p2.x) < len)) {
            t1 = c1;
            t2 = c2;
            len = abs(c1.x - p1.x) + abs(p1.y - p2.y) + abs(c2.x - p2.x);
        }
    }
    return (len < 9999);
}

// 检查是否可以消除的函数
bool canEliminate(const vector<vector<int>>& matrix, Point p1, Point p2) {
    if (matrix[p1.y][p1.x] != matrix[p2.y][p2.x] || matrix[p1.y][p1.x] == -1)
        return false;

    Point t1, t2;
    if (link0(matrix, p1, p2)) {
        return true;
    } else if (link1(matrix, p1, p2, t1)) {
        return true;
    } else if (link2(matrix, p1, p2, t1, t2)) {
        return true;
    }
    return false;
}
//参考知乎文章https://zhuanlan.zhihu.com/p/596689758?

// 用户输入消除元素
void userPlay(vector<vector<int>>& matrix) {
    while (true) {
        cout << "输入x1 y1 x2 y2, -1退出: ";
        int x1, y1, x2, y2;
        cin >> x1;
        if (x1 == -1) break;
        cin >> y1 >> x2 >> y2;

        Point p1(x1, y1);
        Point p2(x2, y2);

        if (canEliminate(matrix, p1, p2)) {
            matrix[y1][x1] = -1;
            matrix[y2][x2] = -1;
            cout << "成功: (" << x1 << ", " << y1 << ") 和 (" << x2 << ", " << y2 << ")" << endl;
        } else {
            cout << "不可以" << endl;
        }

        // 输出矩阵
        for (const auto& r : matrix) {
            for (int a : r) {
                if(a >= 0){
                    cout << "  " << a ;
                }else
                {
                    cout << " " << a;
                }
            }
            cout << endl;
        }
    }
}

// 自动
void autoPlay(vector<vector<int>>& matrix) {
    bool found = true;
    int c = 0;
    while (found) {
        found = false;
        for (int y1 = 0; y1 < matrix.size(); y1++) {
            for (int x1 = 0; x1 < matrix[0].size(); x1++) {
                if(c == 34)
                {
                    break;
                }
                if (matrix[y1][x1] == -1) continue;

                for (int y2 = 0; y2 < matrix.size(); y2++) {
                    for (int x2 = 0; x2 < matrix[0].size(); x2++) {
                        if (matrix[y2][x2] == -1 || (y1 == y2 && x1 == x2)) continue; //跳过自己

                        if (canEliminate(matrix, Point(x1, y1), Point(x2, y2))) {
                            cout << "(" << x1 << "," << y1 << ") (" << x2 << "," << y2 << ")" << endl;
                            matrix[y1][x1] = -1;
                            matrix[y2][x2] = -1;
                            found = true;
                            c++;
                        }
                    }
                }
            }
        }
    }
}


int main() {
    //导入图片和模板
    Mat example = imread("../example.jpg");     //图片在可执行文件的上一级目录
    vector<Mat> templates;
    for(auto& i : fs::directory_iterator("../templates"))
    {
        cout<<"read:"<<i.path().string()<<endl;
        templates.push_back(imread(i.path().string()));
    }

    //生成灰度图，便于阈值处理
    Mat gray;
    cvtColor(example,gray,COLOR_BGR2GRAY);

    //阈值处理，将图标大部分变为白色
    Mat binary;
    threshold(gray,binary,245,250,THRESH_BINARY);

    // 进行开运算以去除小噪声，像手机顶栏和无关紧要的字
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat opened;
    morphologyEx(binary, opened, MORPH_OPEN, kernel);

    //查找轮廓，便于求模板的大小方便模板匹配
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(opened, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 计算每个轮廓的边界框，并存储其尺寸
    vector<int> sizes;
    for (const auto & contour : contours)
    {
        Rect boundingBox = boundingRect(contour);                     //画每个轮廓的最小矩形框
        //rectangle(example, boundingBox, Scalar(0, 255, 0), 2);        //在原图上绘制框
        sizes.push_back(boundingBox.width);                             //反正是正方形，让size等于横边长
        //cout << "Bounding Box " << i << ": " << boundingBox << endl;
    }

    // 计算边界框尺寸的众数
    int modeSize = ModeCalc(sizes);
    cout << "图标size众数: " << modeSize << endl;

    //切割模板匹配的结果图
    Mat result;
    int result_cols = example.cols - templates[0].cols + 1;
    int result_rows = example.rows - templates[0].rows + 1;
    result.create(result_cols, result_rows, CV_32FC1);

    //定义7个颜色分明的scalar来表示不同的框
    vector<Scalar> colors = {
        Scalar(0, 0, 255),      //红色
        Scalar(0, 255, 0),      //绿色
        Scalar(255, 0, 0),      //蓝色
        Scalar(0, 255, 255),    //黄色
        Scalar(255, 0, 255),    //红色
        Scalar(255, 255, 0),    //青色
        Scalar(0 , 0 , 0 )      //黑色
    };

    //建立表情图标坐标系矩阵
    int row = 10;    //害，自己数的，不优雅
    int col = 7;
    vector<vector<int>> matrix(row,vector<int>(col,-1));

    //开始模板匹配
    for(int i = 0; i <= 6; ++i)
    {
        resize(templates[i],templates[i],Size(modeSize,modeSize));

        matchTemplate(example,templates[i],result,TM_SQDIFF_NORMED);

        //模板匹配画框
        // double minVal;
        // double maxVal;
        // Point minLoc;
        // Point maxLoc;
        // minMaxLoc(result,&minVal,&maxVal,&minLoc,&maxLoc);
        //
        // Point p = Point(minLoc.x + modeSize ,minLoc.y + modeSize);
        //
        // rectangle(example,minLoc,p,Scalar(0,0,255),2,LINE_8,0);

        //设置匹配的阈值
        double thresholdValue = 0.02;
        Mat dst;
        threshold(result, dst, thresholdValue, 1.0, THRESH_BINARY_INV);

        //找到匹配位置并标记
        vector<Point> iconLoc;
        findNonZero(dst, iconLoc);

        for (const Point& p : iconLoc)
        {
            rectangle(example, Rect(p.x, p.y, templates[i].cols, templates[i].rows), colors[i], 2);
            cout<<"icon["<<i<<"] : ( "<<p.x<<" , "<<p.y<<" )"<<endl;

            row = p.y / (modeSize + 10 ) - 4;       //这里也不优雅，主要加个裁减就差不多了
            col = p.x / (modeSize + 10 ) - 0;
            matrix[row][col] = i;
        }

        cout<<endl;
    }

    // 输出矩阵
    for (const auto& r : matrix)
    {
        for (int a : r)
        {
            cout << a << " ";
        }
        cout << endl;
    }

    //显示匹配完的图片
    resize(example,example,Size(),0.75,0.75,INTER_LINEAR);
    imshow("example:",example);

    //开始玩
    int whoPlay;
    cout<<"1：自己玩"<<endl<<"0:机器玩"<<endl<<"请输入：";
    cin>>whoPlay;
    if(whoPlay == 1)
    {
        userPlay(matrix);
    }
    else if(whoPlay == 0)
    {
        autoPlay(matrix);
        cout<<"好了，别玩了";
    }
    else
    {
        cout<<"别玩了。";
    }


    waitKey(0);

    return 0;
}