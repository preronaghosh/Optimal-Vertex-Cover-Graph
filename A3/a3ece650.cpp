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

    // create pipes
    int PipeRgenToA1[2];
    int pipe1_RgentoA1_status = pipe(PipeRgenToA1);

    if (pipe1_RgentoA1_status == -1)
	{
	  cerr<<"Error:creating the pipe from Rgen to A1"<<endl;
	  exit(1);
	}

    // Run Rgen 
    //create child process
    child_rgen = fork();  

    if(child_rgen == -1)
    {
        cerr<<"Error: Could not fork rgen" << endl;
        return 1;
    } 
    else if(child_rgen == 0)
    {
        // cout<<"Pipe from Rgen to A1 created; executing RGEN"<<endl;
        //redirect output of rgen to A1
        dup2(PipeRgenToA1[1], STDOUT_FILENO);
        close(PipeRgenToA1[0]);
        close(PipeRgenToA1[1]);
        execvp("./rgen", argv);
        cerr<<"Error: Rgen could not be executed" << endl;
        exit(1);
    }
    //Pipe Rgento A1 contains an output
    kids.push_back(child_rgen);
    // cout<<"Back to driver code after successful execution of Rgen"<<endl;

    //Create Pipe A1 to A2
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
        // cout<<"Child A1 created"<<endl;
        
        dup2(PipeRgenToA1[0], STDIN_FILENO);

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
        cerr<<"Error: Could not execute a1"<<endl;
        return 1;
    }
    kids.push_back(child_a1);

    // Hook A2 to A3
    child_a2 = fork();
    if(child_a2 == -1)
    {
        cerr<<"Error: Could not fork A2" << endl;
        return 1;
    }
    else if(child_a2 == 0)
    {
        // cout<<"Executing child a2"<<endl;
        dup2(PipeA1ToA2[0], STDIN_FILENO);
        close(PipeA1ToA2[1]);
        close(PipeA1ToA2[0]);
        execvp("./a2ece650", argv);
        cerr<<"Error: Could not execute a2"<<endl;
        return 1;
    }
    
    dup2(PipeA1ToA2[1], STDOUT_FILENO);
    close(PipeA1ToA2[0]);
    close(PipeA1ToA2[1]);

    kids.push_back(child_a2);

    while (!cin.eof())
    {
        // read a line of input until EOL and store in a string
        string inp;
        getline(cin, inp);
        if (inp.size () > 0)
            cout<<inp<<endl;
    }        

    int status;
    for (int ptr=0; ptr < kids.size(); ptr++) 
    {
        kill(ptr, SIGTERM);
        waitpid(ptr, &status, 0);
    }
    return 0;
}
