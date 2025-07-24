#include<vector>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <string>
#include <sys/wait.h>
using namespace std;


int main (int argc, char **argv) 
{
    //cout<<"Starting a3"<<endl;

    vector<pid_t> kids;
    pid_t child_rgen;    
    pid_t child_a1;
    pid_t child_a2;

    // create pipe for passing data from rgen to A1 process
    int PipeRgenToA1[2];
    int pipe1_RgentoA1_status = pipe(PipeRgenToA1);

    if (pipe1_RgentoA1_status == -1)
	{
	  cerr<<"Error:creating the pipe from Rgen to A1"<<endl;
	  exit(1);
	}

    // create child process to run Rgen
    child_rgen = fork();  

    if(child_rgen == -1)
    {
        cerr<<"Error: Could not fork rgen" << endl;
        return 1;
    } 
    else if(child_rgen == 0)
    {
        // Pipe from Rgen to A1 created; executing RGEN
        // rgen.py generates output to stdout, so copy rgenToA1 pipe's write fd to stdout fd
        // so that rgen's output data gets written into pipe's write fd
        // then pipe's read fd can read from it in A1 process 
        dup2(PipeRgenToA1[1], STDOUT_FILENO); // redirect rgen's stdout to the pipe
        close(PipeRgenToA1[0]);
        close(PipeRgenToA1[1]);
        execvp("./rgen", argv);
        cerr << "Error: Rgen could not be executed" << endl;
        exit(1);
    }
    // store the rgen pid for killing it later on at the end of this parent process
    kids.push_back(child_rgen);

    // Create Pipe A1 to A2
    int PipeA1ToA2[2];
    int pipe2_A1toA2_status = pipe(PipeA1ToA2);

    if(pipe2_A1toA2_status == -1) {
        cerr<<"Error: Failed to create pipe from A1 to A2"<<endl;
        exit(1);
    }

    // Hook A1 to A2
    child_a1 = fork();  
    if(child_a1 == -1)
    {
        cerr<<"Error: Could not fork A1" <<endl;
        return 1;
    }
    else if(child_a1 == 0)
    {     
        /* a1 by default reads from stdin inside the program
        hence we copy read_fd of rgen to A1 pipe into stdin
        so now when A1 accesses data from stdin, it actually reads from rgenToA1 pipe read fd */
        dup2(PipeRgenToA1[0], STDIN_FILENO); 

        /* A1 by default writes to Stdout
        thus, we copy A1toA2 pipe's write_fd into stdout, so that A1's 
        output gets written into the A1toA2 pipe instead, so that A2 can then read the data
        from the pipe */
        dup2(PipeA1ToA2[1], STDOUT_FILENO); 

        close(PipeRgenToA1[0]);
        close(PipeRgenToA1[1]);
        close(PipeA1ToA2[0]);
        close(PipeA1ToA2[1]);
        

        char* py_file[3];
        py_file[0] = (char*)"/usr/bin/python3";
        py_file[1] = "a1ece650.py";
        py_file[2] = nullptr;
        
        execv("/usr/bin/python3", py_file);
        cerr << "Error: Could not execute a1" << endl;
        return 1;
    }
    // store a1's child pid as well
    kids.push_back(child_a1);

    // Connect A2 to A3
    child_a2 = fork();

    if(child_a2 == -1) {
        cerr << "Error: Could not fork A2" << endl;
        return 1;
    } else if(child_a2 == 0) {
        /* a2 by default reads from stdin in its own code
        hence, we copy read_fd of A1toA2 pipe to stdin
        so that when it accesses the stdin for data, it actually reads from the A1toA2 pipe */

        /* Note: A2 writes to stdout by default and we keep it like that */
        dup2(PipeA1ToA2[0], STDIN_FILENO);

        close(PipeA1ToA2[1]);
        close(PipeA1ToA2[0]);
        execvp("./a2ece650", argv);
        cerr << "Error: Could not execute a2" << endl;
        return 1;
    }
    
    // print the input of A2 to stdout in A3 process 
    dup2(PipeA1ToA2[1], STDOUT_FILENO);

    // close all descriptors that are no longer needed
    close(PipeA1ToA2[0]);
    close(PipeA1ToA2[1]);

    // store a2's pid
    kids.push_back(child_a2);

    /* Now that the whole pipeline has been set, continue reading data from the pipeline continuously
    and keep printing to stdout */
    while (!cin.eof())
    {
        // read a line of input until EOL and store in a string
        string inp;
        getline(cin, inp);
        // print out whatever input A3 gets from previous processes in pipeline
        if (inp.size () > 0) cout << inp << endl;
    }

    int status;
    // kill all child pids before closing parent process A3
    for (int& pid : kids) 
    {
        kill(pid, SIGTERM);
        waitpid(ptr, &status, 0);
    }
    return 0;
}
