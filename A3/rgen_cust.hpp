#include<vector>
#include<string>
using namespace std;

int random_gen(int min, int max);
char any_letter();
string any_word(int wl);
bool intersects(pair<int, int> point1, pair<int, int> point2, pair<int, int> point3, pair<int, int> point4);
bool line_seg_valid(vector<pair<int, int>> street, pair<int,int> point);


class Graph
{
public:    
    vector<string> street_names;
    vector<vector<pair<int,int>>> street_pts;

    void create_graph();
    vector<pair<int,int>> create_line_segments();
    void remove_commands();
    void add_street_command();
};