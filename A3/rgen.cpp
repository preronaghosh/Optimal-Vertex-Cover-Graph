#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<algorithm>
#include <unistd.h>
#include "rgen_cust.hpp"
using namespace std;

int s_value = 10;
int n_value = 5;
int l_value = 5;
int c_value = 20;


void Graph::remove_commands()
{
    string tmp;
    for(int i=0; i<street_names.size(); i++)
    {
        tmp += "r \"" + street_names[i] + "\"" + "\n";
    }
    cout<<tmp<<endl;
}

void Graph::add_street_command()
{
    string tmp;
    if(street_names.size() > 0)
    {
        for(int i=0; i<street_names.size(); i++)
        {
            tmp = tmp + "a \"" + street_names[i] + "\"";
            for(auto& pt : street_pts[i])
            {
                tmp += " (" + to_string(pt.first) + "," + to_string(pt.second) + ")"; 
            } 
            tmp += "\n";
        }
        tmp += "g";
        cout<<tmp<<endl;
        //debug
        // cerr<<tmp<<endl;
    }
}

void Graph::create_graph()
{
    int num_streets = random_gen(2, s_value);
    street_names.clear();
    for(int i=0; i<num_streets; i++)
    {
        string s = any_word(6);
        // check if street name already exists
        if(find(street_names.begin(), street_names.end(), s) != street_names.end())
        {
            //street name already exists - generate a new name
            s = any_word(6);
        }
        else
        {
            street_names.push_back(s);
            // create a list of line segments for this new street
            street_pts.push_back(create_line_segments());
        }
    }
}

vector<pair<int,int>> Graph::create_line_segments()
{
    int n_seg = random_gen(1, n_value);
    // cout<<"Number of Line segments: "<<n_seg<<endl;
    int coor = c_value;
    int count=0;

    vector<pair<int,int>> st;
    pair<int,int> point;

    //generate line segments within a range
    for(int i=0; i<n_seg; i++)
    {
        point.first = random_gen(-coor, coor);
        point.second = random_gen(-coor, coor);
        //check if point is valid
        if(line_seg_valid(st, point))
        {
            st.push_back(point);
            // cout<<"Point x is: "<<point.first<<" Point y: "<<point.second<<endl; 
            count=0;
        }
        else
        {
            count++;
            if(count > 25)
            {
                cerr<<"Error: failed to generate valid input for "<< count <<" simultaneous attempts"<<endl;
                count=0;
                exit(1);
            }
        }
    }
    // return street with all valid new line segments
    return st;
}

bool line_seg_valid(vector<pair<int, int>> street, pair<int,int> point)
{
    //no duplicate points in the same street
    for(int i=0; i<street.size(); i++)
    {
        if( (street[i].first == point.first) && (street[i].second == point.second) )
            return false;

        // no intersecting or overlapping segments in the same street
        for(int j=i+1; j<street.size(); j++)
        {
            if (i+1 < street.size() && j+1 < street.size())
            {
                if(intersects(street[i], street[i+1], street[j], street[j+1])) 
                    return false;
            }
        }
    }

    // no overlaps across all streets

    //todo


    return true;
}

int random_gen(int min, int max)
{
    ifstream urandom("/dev/urandom");

    if (urandom.fail()) {
        cerr << "Error: cannot open /dev/urandom\n";
        return 1;
    }
    int r = (max - min)+1 ;
    int num=0;     
    while(true) 
    {
        urandom.read((char*)&num,1);
        num = min + num % r;
        // cout<<"RandGen: "<<num<<endl;

        if(num>=min && num<=max)
        {
            urandom.close();
            break;
        }
    }
    return num;
}

string any_word(int wl=10)
{
    string word;
    for(int i=0; i<wl; i++)
    {
        word += any_letter();
    }
    // cout<<"st name: "<<word<<endl;
    return word;
}

char any_letter()
{
    char random;
    int i;
    // only consider lower case letters as street name does not matter
    i = random_gen(97, 122);
    //cout<<"Random number is: "<< i <<endl;
    random = (char)i;
    // cout<<random<<endl;
    return random;
}

bool intersects(pair<int, int> point1, pair<int, int> point2, pair<int, int> point3, pair<int, int> point4) 
{
    int x1 = point1.first;
    int y1 = point1.second;
    int x2 = point2.first;
    int y2 = point2.second;
    int x3 = point3.first;
    int y3 = point3.second;
    int x4 = point4.first;
    int y4 = point4.second;
    float m1;
    float b1;
    float m2;
    float b2;

    int seg1_xmin = min(x1, x2);
    int seg1_xmax = max(x1, x2);
    int seg2_xmin = min(x3, x4);
    int seg2_xmax = max(x3, x4);
    int seg1_ymin = min(y1, y2);
    int seg1_ymax = max(y1, y2);
    int seg2_ymin = min(y3, y4);
    int seg2_ymax = max(y3, y4);
    int x_interval_max = max(seg1_xmin, seg2_xmin);
    int x_interval_min = min(seg1_xmax, seg2_xmax);
    int y_interval_max = max(seg1_ymin, seg2_ymin);
    int y_interval_min = min(seg1_ymax, seg2_ymax);

    // check for vertical overlapping lines
    if ( x1 == x2 && x2 == x3 && x3 == x4 ) 
    {
        if ( (y_interval_min <= y1 && y1 <= y_interval_max) ||
             (y_interval_min <= y2 && y2 <= y_interval_max) ||
             (y_interval_min <= y3 && y3 <= y_interval_max) ||
             (y_interval_min <= y4 && y4 <= y_interval_max) )  
        {
            return true;
        }
    } 
    else if ( x1 != x2 && x3 != x4) 
    {    
        m1 = (y2-y1)/(x2-x1);
        b1 = y1-m1*x1;
        m2 = (y4-y3)/(x4-x3);
        b2 = y3-m2*x3;
        // check if line equations are equal
        if (m1 == m2 && b1 == b2) {
            if ( 
             ( (x_interval_min <= x1 && x1 <= x_interval_max) ||
             (x_interval_min <= x2 && x2 <= x_interval_max) ||
             (x_interval_min <= x3 && x3 <= x_interval_max) ||
             (x_interval_min <= x4 && x4 <= x_interval_max) ) 
             &&
             ( (y_interval_min <= y1 && y1 <= y_interval_max) ||
             (y_interval_min <= y2 && y2 <= y_interval_max) ||
             (y_interval_min <= y3 && y3 <= y_interval_max) ||
             (y_interval_min <= y4 && y4 <= y_interval_max) )
                ) 
            {
                return true;
            }
        }
    }

    float xnum = ((x1*y2-y1*x2)*(x3-x4) - (x1-x2)*(x3*y4-y3*x4));
    float xden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4));

    float ynum = (x1*y2 - y1*x2)*(y3-y4) - (y1-y2)*(x3*y4-y3*x4);
    float yden = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
    if (xden == 0 || yden == 0) {
        return false;
    }

    float xcoor = xnum / xden;
    float ycoor = ynum / yden;
    if ( x_interval_min <= xcoor && xcoor <= x_interval_max &&
        y_interval_min <= ycoor && ycoor <= y_interval_max ) {
            return true;
        }
    return false;
}



int main(int argc, char **argv)
{
    string s, n, l, c;
    int l_time;
    int o;
    opterr = 0;

    while((o = getopt (argc, argv, "s:n:l:c:?")) != -1)
    {
        switch(o)
        {
        case 's':
            s = optarg;
            s_value = atoi(s.c_str());
            if(s_value < 2) {
                cerr<<"Error: Option -s must have value >= 2" << endl;
                return 1;
            }
            break;

        case 'n':
            n = optarg;
            n_value = atoi(n.c_str());
            if(n_value < 1) {
                cerr<<"Error: Option -n must have a value >= 1"<<endl;
                return 1;
            }
        break;

        case 'l':
            l = optarg;
            l_value = atoi(l.c_str());
            if(l_value < 5) {
                cerr<<"Error: Option -l must have a value >= 5"<<endl;
                return 1;
            }
            break;

        case 'c':
            c = optarg;
            c_value = atoi(c.c_str());
            if(c_value < 1) {
                cerr<<"Error: Option -c must have a value >= 1"<<endl;
                return 1;
            }
            break;
        
        case '?':
            cerr<<"Error: Unknown option provided"<<endl;
            return 1;
            
        default:
            return 0;
        }
    }

    while(true)
    {
        Graph random_graph; 

        random_graph.create_graph();
        random_graph.add_street_command();
        
        l_time = random_gen(5, l_value);
        random_graph.remove_commands();
        sleep(l_time);
    }
    return 0;
}