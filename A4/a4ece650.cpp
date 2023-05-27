#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "custom.hpp"
// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"
using namespace std;


int main()
{
    GraphVertexCover g(0);

    while(!cin.eof())
    {
        string input;
        getline(cin,input);

        if(cin.eof()) break;

        if(input[0] == 'V')
        {
            string sub;
            int N;
            sub = input.substr(2, input.length()-2);
            N = stoi(sub);
            g.setNumVertices(N);
            continue;
        }
        else if(input[0] == 'E')
        {
            g.edge_list.clear();
            pair<int, int> temp_edge;
            if(g.getNumVertices() != 0)
            {
                int s_index, e_index, s, e;
                string sub, start, end, edge_inp = input; 
                vector<int> temp;

                if((signed)edge_inp.find("<") == -1 && (signed)edge_inp.find(">") == -1)
                {
                    //require an empty output as per campuswire queries
                    cout<<endl;
                    //cerr<<"Error: Edge list is empty"<<endl;
                    continue;
                }
                // Parse string and get the start and end nodes
                for(int i=0; (unsigned)i<input.length(); i++)
                {
                    s_index = edge_inp.find("<");
                    e_index = edge_inp.find(">");
                    
                    if(s_index==-1 && e_index==-1) break;

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
                            //add to edge list
                            temp_edge.first = s;
                            temp_edge.second = e;
                            g.edge_list.push_back(temp_edge);
                        }
                        else
                        {
                            g.edge_list.clear();
                            cerr<<"Error: Invalid range of vertex provided, so no vertex cover\n";
                            break;
                        }  
                    }
                    else
                    {
                        g.edge_list.clear();
                        cerr<<"Error: Invalid range of vertex provided, so no vertex cover\n";
                        break;
                    }
                }
                
                if(!g.edge_list.empty()) // Valid edges are available 
                {
                    //find vertex cover
                    vector<int> v_cover = g.get_vertex_cover();
                    sort(v_cover.begin(), v_cover.end());
                    if(!v_cover.empty())
                    {
                        for(int ctr=0; ctr<(int)v_cover.size(); ctr++)
                        {
                            cout<<v_cover[ctr]<<" ";
                        }
                        cout<<endl;
                    }
                    else
                    {   
                        cerr<<"Error: There is no vertex cover for the provided graph"<<endl;
                    }
                    v_cover.clear();
                }
            }
            else
            {
                //require an emoty output as per campuswire queries
                cout<<endl;
                //cerr<<"Error: There is no vertex cover for a graph with zero vertices"<<endl;
            }
            continue;
        }
    }
    return 0;
}

vector<int> GraphVertexCover::get_vertex_cover()
{
    vector<int> vertex_cover;
    vertex_cover.clear();
    int n = vertices;
    bool res;
    
    for(int k=1; k<=vertices; k++) 
    {
        res=false; //initially UNSAT
        //cout<<"Value of k is: "<<k<<endl;
        unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
        if(!res)
        {
            vector<vector<Minisat::Lit>> matrix;
            matrix.resize(vertices, vector<Minisat::Lit> (k));

            // Create n x k literals matrix and initialize
            for(int i=0; i<n; i++)
            {
                for(int j=0; j<k; j++)
                {
                    matrix[i][j] = Minisat::mkLit(solver->newVar());
                }
            }
            //debug
            // cout<<"Number of rows in lit_matrix: "<<n<<endl;
            // cout<<"Number of columns in lit_matrix: "<<k<<endl;

            // First clause - At least one vertex is the ith vertex in the vertex cover
            Minisat::vec<Minisat::Lit> clause1;
            for(int i=0; i<k; i++)
            {
                for(int j=0; j<vertices; j++)
                {
                    clause1.push(matrix[j][i]);
                }
                solver->addClause(clause1);
                clause1.clear();
            }


            // Second clause - No one vertex can appear twice in a vertex cover
            //Minisat::vec<Minisat::Lit> clause2;
            for(int m=0; m<vertices; m++)
            {
                for(int p=0; p<k-1; p++)
                {
                    for(int q=p+1; q<k; q++)
                    {
                        //clause2.push(~matrix[m][p], ~matrix[m][q]);
                        solver->addClause(~matrix[m][p], ~matrix[m][q]);
                    }
                }
            }
            //clause2.clear();

            // Third clause - No more than one vertex appears in the mth position of the vertex cover
            //Minisat::vec<Minisat::Lit> clause3;
            for(int m=0; m<k; m++)
            {
                for(int p=0; p<n-1; p++)
                {
                    for(int q=p+1; q<n; q++)
                    {
                        //clause3.push(~matrix[p][m], ~matrix[q][m]);
                        solver->addClause(~matrix[p][m], ~matrix[q][m]);
                    }
                }
            }
            //clause3.clear();

            // Fourth clause - Every edge is incident to at least one vertex in the vertex cover
            Minisat::vec<Minisat::Lit> clause4;
            for(int i=0; i<(int)edge_list.size(); i++)
            {
                int v1 = edge_list[i].first;
                int v2 = edge_list[i].second;

                for(int j=0; j<k; j++)
                {
                    clause4.push(matrix[v1][j]);
                    clause4.push(matrix[v2][j]);
                }
                solver->addClause(clause4);
                clause4.clear();
            }
            
            res = solver->solve();
            //cout <<"The result is: "<<res<<endl;
            if(res)
            {
                for(int i=0; i<vertices; i++)
                {
                    for(int j=0; j<k; j++)
                    {
                        //printing the SAT assignments
                        //cout<<"Assignment for literal "<<i<<","<<j<<" is: "<<Minisat::toInt(solver->modelValue(matrix[i][j]))<<endl;
                        if(solver->modelValue(matrix[i][j]) == Minisat::l_True)
                        {
                            vertex_cover.push_back(i);
                        }
                    }
                }
                return vertex_cover;
            }
        }
        //reset solver for next loop
        solver.reset(new Minisat::Solver());
    }
    cerr<<"Error: F is unsat for all sizes of k"<<endl;
    return vertex_cover; //will be empty if code reaches this point
}