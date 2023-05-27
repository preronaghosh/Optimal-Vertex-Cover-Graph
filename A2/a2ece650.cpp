// Compile with c++ a2ece650.cpp -std=c++11 -o a2ece650
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <queue>
#include <string>
#include<typeinfo>
using namespace std;


class Graph
{
    int num_vert;
    vector<vector<int>> adj_list;

public:
    Graph(int V);
    void setNumVertices(int V);
    int getNumVertices();
    void add_edges(int vertices, int start, int end);
    void clearList() {adj_list.clear();}
    void getShortestPath(int start_node, int end_node);
    bool bfs(int src, int dest, int parent[]);
    vector<vector<int>> getAdjList();
};

vector<vector<int>> Graph::getAdjList()
{
    return adj_list;
}
bool Graph::bfs(int src, int dest, int parent[])
{
    queue<int> q; // queue to hold the current node
    q.push(src);

    int visited[num_vert]; // array to hold the visited nodes 0-not visited 1- visited

    // Initialization
    for(int i=0; i<num_vert; i++)
    {
        parent[i] = -1;
        visited[i] = 0;
    }

    visited[src] = 1;

    while(!q.empty())
    {
        int n = q.front(); 
        q.pop(); // remove the node considered in the current cycle
        for(int i=0; (unsigned)i<adj_list[n].size(); i++) // access every connected node of src node
        {
            if(visited[adj_list[n][i]] == 0) // connected node has not been visited
            {
                q.push(adj_list[n][i]);
                visited[adj_list[n][i]]=1;
                parent[adj_list[n][i]]=n;

                // stop bfs if dest node is found 
                if(adj_list[n][i] == dest)
                {
                    return true;
                }
            }
        }

    }
    return false;
}

void Graph::getShortestPath(int start_node, int end_node)
{
    // Check if the vertex values are in range or not
    if(start_node >= 0 && start_node< getNumVertices())
    {
        if(end_node>=0 && end_node< getNumVertices())
        {
            if(start_node == end_node)
            {
                cerr<<start_node<<"-"<<adj_list[start_node][0]<<"-"<<end_node<<endl;                
            }
            else
            {
                // call BFS Algorithm
                int parent[num_vert];

                if(!bfs(start_node, end_node, parent))
                {
                    cerr<<"Error: There is no connected path between "<<start_node<<" and "<<end_node<<"\n";
                }
                else
                {
                    // Reconstruct the path
                    vector<int> path;
                    path.push_back(end_node);
                    int last = end_node;
                    while(parent[last]!=-1)
                    {
                        path.push_back(parent[last]);
                        last = parent[last];
                    }

                    vector<int> :: iterator iter = path.end();
                    for(int i=path.size()-1; i>=0; i--)
                        {
                            cout<<path[i];
                            if(i>0){
                                cout<<"-";
                            }
                        }
                    cout<<endl;
                }
            }
        }
        else
        {
            cerr<<"Error: The provided nodes are out of range.\n";
        }
    }
    else
    {
        cerr<<"Error: The provided nodes are out of range.\n";
    }
}

Graph::Graph(int V)
{
    num_vert = V;
}

int Graph::getNumVertices()
{
    return num_vert; 
}

void Graph::setNumVertices(int V)
{
    num_vert = V;
}

void Graph::add_edges(int vertices, int start, int end)
{   
    adj_list.resize(vertices);
    adj_list[start].push_back(end);
    adj_list[end].push_back(start);

}

int main()
{
    Graph g(0);
    bool no_paths=false;
    while(!cin.eof())
    {
        string input;
        getline(cin,input);

        if(cin.eof())
        {
            break;
        }
        
        if(input[0] == 'V')
        {
            string sub;
            int N;
            sub = input.substr(2, input.length()-2);
            N = stoi(sub);
            g.setNumVertices(N);
            g.clearList();
            continue;
        }
        else if(input[0] == 'E')
        {
            if(g.getNumVertices() != 0)
            {
                int s_index, e_index;
                string sub; 
                vector<int> temp;
                string edge_inp = input;
                string start, end;
                int s, e;
                
                if((signed)edge_inp.find("<") == -1 && (signed)edge_inp.find(">") == -1)
                {
                    no_paths = true;
                }
                // Parse string and get the start and end nodes
                for(int i=0; (unsigned)i<input.length(); i++)
                {
                    s_index = edge_inp.find("<");
                    e_index = edge_inp.find(">");
                    
                    if(s_index==-1 && e_index==-1)
                    {
                        break;
                    }
                    sub = edge_inp.substr(s_index+1, e_index-s_index-1);
                    start = sub.substr(0,sub.find(","));
                    end = sub.substr(sub.find(",")+1, sub.length());
                    s = stoi(start);
                    e = stoi(end);
                    edge_inp.erase(s_index, e_index-s_index+1);
                    // Check validity of vertex values found
                    if(s < g.getNumVertices() && s >= 0)
                    {
                        if(e < g.getNumVertices() && e>=0)
                        {
                            g.add_edges(g.getNumVertices(), s, e);
                        }
                        else
                        {
                            cerr<<"Error: Invalid range of vertex provided\n";
                        }  
                    }
                    else
                    {
                        cerr<<"Error: Invalid range of vertex provided\n";
                    }
                }
            }
            else
            {
                cerr<<"Error: Incorrect number of vertices given as input\n"<<endl;
            }
            continue;
        }
        else if(input[0] == 's')
        {
            int count=0, start_node, end_node;
            input += " ";

            if(no_paths)
            {
                cerr<<"Error: Path does not exist\n";
                no_paths=false;
                continue;
            }

            // Extract the start and end vertices out of the input string
            string sub = "";
            for(int i=2; (unsigned)i<input.length(); i++)
            {
                if(input[i] != ' ')
                {
                    sub += input[i];
                }
                else
                {
                    count++;
                    if(count==1) 
                    {
                        start_node = stoi(sub); 
                    }
                    else if(count==2)
                    {
                        end_node = stoi(sub);
                    }
                    sub = "";
                }
            }
            g.getShortestPath(start_node, end_node);
            continue;
        }
        else
        {
            cerr << "Error: Invalid input. \n";
            continue;

        }
    }
    return 0;
}
