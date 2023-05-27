#include "vertexCover.hpp"
#include "solvers/cnf/cnfSatVc.cpp"
#include "solvers/approxVc/approxVc1.cpp"
#include "solvers/refinedApprox/refinedApproxVc1.cpp"

#define TIMEOUT 120;

#define THREAD_ANALYSIS false

struct args
{
    int vertex;
    std::vector<std::pair<int, int>> edges;
};

long long cnf_sat_thread_time, cnf3_sat_thread_time, approx_vc1_thread_time, approx_vc2_thread_time, refined_approx_vc1_thread_time, refined_approx_vc2_thread_time;
std::vector<int> cnf_vc_result, cnf3_vc_result, approx_vc1_result, approx_vc2_result, refined_approx_vc1_result, refined_approx_vc2_result;
pthread_t cnf_vc, cnf3_vc, approx_vc1, approx_vc2, refined_approx_vc1, refined_approx_vc2;
// bool cnf_vc_status, cnf_vc3_status;
bool cnf_vc_timeout = false;
bool cnf_vc3_timeout = false;

void *cnfVcHandler(void *arg)
{
    auto start = std::chrono::high_resolution_clock::now();
    args *Arg = static_cast<args *>(arg);

    cnf_vc_result = cnfSatVc(Arg->vertex, Arg->edges);

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    cnf_sat_thread_time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

    return NULL;
}
void *cnf3SatVcHandler(void *arg)
{
    auto start = std::chrono::high_resolution_clock::now();
    args *Arg = static_cast<args *>(arg);

    cnf3_vc_result = cnf3SatVc(Arg->vertex, Arg->edges);

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    cnf3_sat_thread_time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

    return NULL;
}

void *approxVc1Handler(void *arg)
{
    auto start = std::chrono::high_resolution_clock::now();
    args *Arg = static_cast<args *>(arg);

    approx_vc1_result = approxVc1(Arg->vertex, Arg->edges);

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    approx_vc1_thread_time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

    return NULL;
}
void *approxVc2Handler(void *arg)
{
    auto start = std::chrono::high_resolution_clock::now();
    args *Arg = static_cast<args *>(arg);

    approx_vc2_result = approxVc2(Arg->vertex, Arg->edges);

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    approx_vc2_thread_time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

    return NULL;
}
void *refinedApproxVc1Handler(void *arg)
{
    auto start = std::chrono::high_resolution_clock::now();
    args *Arg = static_cast<args *>(arg);

    refined_approx_vc1_result = refinedApproxVc1(Arg->vertex, Arg->edges);

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    refined_approx_vc1_thread_time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

    return NULL;
}

void *refinedApproxVc2Handler(void *arg)
{
    auto start = std::chrono::high_resolution_clock::now();

    args *Arg = static_cast<args *>(arg);

    refined_approx_vc2_result = refinedApproxVc2(Arg->vertex, Arg->edges);

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    refined_approx_vc2_thread_time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

    return NULL;
}

void print_result(std::string title, std::vector<int> result)
{
    std::cout << title;
    std::for_each(result.begin(), result.end(), [](int i)
                  { std::cout << i << ' '; });
    // if(title != "REFINED-APPROX-VC-2: ")
    // {
    std::cout << std::endl;
    // }
}

VertexCover::VertexCover() {}

void VertexCover::addvertex(int x) { vertex = x; }
void VertexCover::addEdges(std::vector<std::pair<int, int>> &src)
{
    edges.clear();                                                // clear any existing edges
    std::copy(src.begin(), src.end(), std::back_inserter(edges)); // copy the argument to the edges variable in Graph class
}
void VertexCover::printVertexCover()
{

    args ArgsVC_1 = {vertex, edges};
    struct timespec ts;

    pthread_create(&cnf_vc, NULL, &cnfVcHandler, &ArgsVC_1);
    pthread_create(&cnf3_vc, NULL, &cnf3SatVcHandler, &ArgsVC_1);
    pthread_create(&approx_vc1, NULL, &approxVc1Handler, &ArgsVC_1);
    pthread_create(&approx_vc2, NULL, &approxVc2Handler, &ArgsVC_1);
    pthread_create(&refined_approx_vc1, NULL, &refinedApproxVc1Handler, &ArgsVC_1);
    pthread_create(&refined_approx_vc2, NULL, &refinedApproxVc2Handler, &ArgsVC_1);

    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += TIMEOUT;

    int result = pthread_timedjoin_np(cnf_vc, NULL, &ts);

    cnf_vc_timeout = result == ETIMEDOUT ? true : false;

    // Wait for the CNF3-SAT-VC algorithm to finish, with timeout
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += TIMEOUT;

    result = pthread_timedjoin_np(cnf3_vc, NULL, &ts);
    cnf_vc3_timeout = result == ETIMEDOUT ? true : false;

    pthread_join(approx_vc1, NULL);
    pthread_join(approx_vc2, NULL);
    pthread_join(refined_approx_vc1, NULL);
    pthread_join(refined_approx_vc2, NULL);

    if (cnf_vc_timeout)
        std::cout << "CNF-SAT-VC: timeout"<<std::endl;
    else
        print_result("CNF-SAT-VC: ", cnf_vc_result);

    if (cnf_vc3_timeout)
        std::cout << "CNF-3-SAT-VC: timeout"<<std::endl;
    else
        print_result("CNF-3-SAT-VC: ", cnf3_vc_result);

    print_result("APPROX-VC-1: ", approx_vc1_result);
    print_result("APPROX-VC-2: ", approx_vc2_result);
    print_result("REFINED-APPROX-VC-1: ", refined_approx_vc1_result);
    print_result("REFINED-APPROX-VC-2: ", refined_approx_vc2_result);

    THREAD_ANALYSIS &&std::cout << "CNF-SAT-VC CPU time:   " << cnf_sat_thread_time << " µs" << std::endl;
    THREAD_ANALYSIS &&std::cout << "CNF3-SAT-VC CPU time:   " << cnf3_sat_thread_time << " µs" << std::endl;
    THREAD_ANALYSIS &&std::cout << "APPROX-VC-1 CPU time:   " << approx_vc1_thread_time << " µs" << std::endl;
    THREAD_ANALYSIS &&std::cout << "APPROX-VC-2 CPU time:   " << approx_vc2_thread_time << " µs" << std::endl;
    THREAD_ANALYSIS &&std::cout << "REFINED-APPROX-VC-1 CPU time:   " << refined_approx_vc1_thread_time << " µs" << std::endl;
    THREAD_ANALYSIS &&std::cout << "REFINED-APPROX-VC-2 CPU time:   " << refined_approx_vc2_thread_time << " µs" << std::endl;
}

void VertexCover::reset()
{
    vertex = 0;
}

// V 15
// E {<2,6>,<2,8>,<2,5>,<6,5>,<5,8>,<6,10>,<10,8>}
// s 2 10

// V 5
// E {<0,2>,<2,1>,<2,3>,<3,4>,<4,1>}
// s 4 0

// V 5
// E {<0,4>,<4,1>,<0,3>,<3,4>,<3,2>,<1,3>}
