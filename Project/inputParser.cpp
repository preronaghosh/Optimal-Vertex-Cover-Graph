#include <iostream>
#include <sstream>
#include <regex>

#include "inputParser.hpp"

std::vector<std::pair<int, int>> edge_parse(std::string s)
{
    std::pair<int, int> ind_edge;
    std::vector<std::pair<int, int>> edges_arr;

    try
    {
        // regex to match numbers alone in a text
        std::regex re("-?[0-9]+");

        // iterate to ge the start and end pair
        for (std::sregex_iterator i = std::sregex_iterator(s.begin(), s.end(), re); i != std::sregex_iterator(); ++i)
        {
            std::smatch start_match = *i;
            ++i; // increment to get the second value of the pair of edges
            std::smatch end_match = *i;

            std::string start = start_match.str();
            std::string dest = end_match.str();

            ind_edge.first = std::stoi(start);
            ind_edge.second = std::stoi(dest);

            // push the pair of edges into the resultant array
            edges_arr.push_back(ind_edge);
        }
    }
    catch (std::regex_error &e)
    {
        edges_arr.clear();
    }
    return edges_arr;
}

bool parse_input(const std::string &line, char &cmd, std::string &err_msg, int &vertex, int &start, int &dest, std::vector<std::pair<int, int>> &edges)
{

    std::istringstream input(line);

    // remove whitespace
    std::ws(input);

    if (input.eof())
    {
        err_msg = "Empty command";
        return false;
    }

    char command_input;
    input >> command_input;

    if (input.fail())
    {
        err_msg = "Failed to read input";
        return false;
    }

    // for user to specify the vertices
    if (command_input == 'V')
    {
        int num;
        input >> num;
        if (input.fail())
        {
            err_msg = "Missing or bad argument";
            return false;
        }
        ws(input);
        if (!input.eof())
        {
            err_msg = "Unexpected argument";
            return false;
        }
        cmd = command_input;
        vertex = num;
        return true;
    }

    // for user to specify the edge
    else if (command_input == 'E')
    {
        std::string edge_string;
        input >> edge_string;
        std::ws(input);
        if (!input.eof())
        {
            err_msg = "Unexpected argument";
            return false;
        }
        std::vector<std::pair<int, int>> parsed_edges;
        parsed_edges = edge_parse(edge_string);
        for (std::pair<int, int> i : parsed_edges)
        {
            if (i.first >= vertex || i.second >= vertex)
            {
                err_msg = "Invalid vertice:" + std::to_string(i.first >= vertex ? i.first : i.second);
                return false;
            }
        }
        cmd = command_input;
        edges = edge_parse(edge_string);
        return true;
    }
    
    else
    {
        err_msg = "Unknown command";
        return false;
    }
}