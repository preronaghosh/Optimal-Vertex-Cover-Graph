#include "refinedApproxVc1.hpp"
#include<algorithm>

void remove_vertex_refined(std::vector<std::pair<int, int>> &edges_copy, int val)
{
    edges_copy.erase(std::remove_if(edges_copy.begin(), edges_copy.end(),
                                    [val](const std::pair<int, int> &p)
                                    {
                                        return (p.first == val || p.second == val) ? true : false;
                                    }),
                     edges_copy.end());
}

std::vector<int> refinedApproxVc1(int &vertex, std::vector<std::pair<int, int>> &edges)
{
    std::vector<int> degree(vertex, 0);
    std::vector<int> result;
    std::vector<std::pair<int, int>> edges_copy;
    std::vector<std::pair<int, int>> edges_copy_2;
    std::copy(edges.begin(), edges.end(), std::back_inserter(edges_copy)); // copy the argument to the edges variable in Graph class
    std::copy(edges.begin(), edges.end(), std::back_inserter(edges_copy_2)); //for refinement

    // loop untill all the edges are traversed and removed and store the values of the edges

    while (edges_copy.size())
    {
        // check the degree of each edges
        for (auto i : edges_copy)
        {
            degree[i.first]++;
            degree[i.second]++;
        }
        auto max_iter = std::max_element(degree.begin(), degree.end());
        int max_idx = std::distance(degree.begin(), max_iter);
        if(std::find(result.begin(), result.end(), max_idx) == result.end())
        {
            result.push_back(max_idx);
        }
        remove_vertex_refined(edges_copy, max_idx);
    }

    //refinement
    int adj_v;
    for(long unsigned int ptr=0; ptr<result.size(); ptr++)
    {
        int i = result[ptr];
        bool flag = false; 
        //take one vertex from vertex cover
        //check if it has any adjacent vertex which is not in VC
        long unsigned int pos=0;
        while(pos<edges_copy_2.size())
        {
            if(edges_copy_2[pos].first == i)
            {
                adj_v = edges_copy_2[pos].second;
                std::vector<int>::iterator itr = std::find(result.begin(), result.end(), adj_v);
                if(itr == result.end())
                {
                    flag = true; 
                    break;
                }
            }
            else if(edges_copy_2[pos].second == i)
            {
                adj_v = edges_copy_2[pos].first;
                std::vector<int>::iterator itr = std::find(result.begin(), result.end(), adj_v);
                if(itr == result.end())
                {
                    flag = true;
                    break;
                }
            }
            pos++;
        }
        if(!flag)
        {
            //there was no adj vertex for i which was not in the VC - vertex i can be removed
            remove(result.begin(), result.end(), i);
            result.resize(result.size()-1);
        }
    }

    sort(result.begin(), result.end());
    return result;
    // std::cout << "REFINED-APPROX-VC-1: ";
    // for (auto i : result)
    // {
    //     std::cout << i << " ";
    // }
    // std::cout << std::endl;
}

std::vector<int> refinedApproxVc2(int &vertex, std::vector<std::pair<int, int>> &edges)
{
    std::vector<int> degree(vertex, 0);
    std::vector<int> result;
    std::vector<std::pair<int, int>> edges_copy;
    std::vector<std::pair<int, int>> edges_copy_2; //for refinement
    std::copy(edges.begin(), edges.end(), std::back_inserter(edges_copy)); // copy the argument to the edges variable in Graph class
    std::copy(edges.begin(), edges.end(), std::back_inserter(edges_copy_2)); //for refinement

    // loop untill all the edges are traversed
    while (edges_copy.size())
    {
        // check the degree of each edges
        int val1= edges_copy[0].first;
        int val2= edges_copy[0].second;
        if(std::find(result.begin(), result.end(), val1) == result.end())
        {
            result.push_back(val1);
        }
        if(std::find(result.begin(), result.end(), val2) == result.end())
        {
            result.push_back(val2);
        }
        remove_vertex(edges_copy, val1);
        remove_vertex(edges_copy, val2);
    }

    //refinement
    int adj_v;
    for(long unsigned int ptr=0; ptr<result.size(); ptr++)
    {
        int i = result[ptr];
        bool flag = false; 
        //take one vertex from vertex cover
        //check if it has any adjacent vertex which is not in VC
        long unsigned int pos=0;
        while(pos<edges_copy_2.size())
        {
            if(edges_copy_2[pos].first == i)
            {
                adj_v = edges_copy_2[pos].second;
                std::vector<int>::iterator itr = std::find(result.begin(), result.end(), adj_v);
                if(itr == result.end())
                {
                    flag = true; 
                    break;
                }
            }
            else if(edges_copy_2[pos].second == i)
            {
                adj_v = edges_copy_2[pos].first;
                std::vector<int>::iterator itr = std::find(result.begin(), result.end(), adj_v);
                if(itr == result.end())
                {
                    flag = true;
                    break;
                }
            }
            pos++;
        }
        if(!flag)
        {
            //there was no adj vertex for i which was not in the VC - vertex i can be removed
            remove(result.begin(), result.end(), i);
            result.resize(result.size()-1);
        }
    }

    sort(result.begin(), result.end());
    return result;
    // std::cout << "REFINED-APPROX-VC-2: ";
    // for (auto i : result)
    // {
    //     std::cout << i << " ";
    // }
    // std::cout << std::endl;
}

// V 5
// E {<0,4>,<4,1>,<0,3>,<3,4>,<3,2>,<1,3>}

