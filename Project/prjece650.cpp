#include "inputParser.cpp"
#include "vertexCover.cpp"

void *inputThread(void *arg)
{
    VertexCover g;

    while (!std::cin.eof())
    {

        std::string line;
        std::getline(std::cin, line);
        std::istringstream input(line);

        if (line.size() == 0)
            continue;

        char cmd;
        int start;
        int dest;
        int vertex;
        std::string err_msg;
        std::vector<std::pair<int, int>> edges_arg;

        if (parse_input(line, cmd, err_msg, vertex, start, dest, edges_arg))
        {
            switch (cmd)
            {
            case 'V':
                g.reset();
                g.addvertex(vertex);
                break;
            case 'E':
                g.addEdges(edges_arg);
                g.printVertexCover();
                break;
            default:
                std::cout << cmd << "arg";
            }
        }
        else
            std::cerr << "Error: " << err_msg << "\n";
    }
    //std::cout << std::endl;
    return NULL;
}

int main(int argc, char **argv)
{
    pthread_t ip_thread;
    try
    {
        pthread_create(&ip_thread, NULL, &inputThread, NULL);

        pthread_join(ip_thread, NULL);
    }
    catch (...)
    {
        std::cout << "Error in main thread";
    }
    return 0;
}
