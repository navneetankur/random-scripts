#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include "sys/stat.h"
#include <sys/wait.h>
#include <unistd.h>
#include <fstream>
using namespace std;

const bool DEBUG = false;
const string binLocation = "/home/navn/bin/cppbins/";

int main(int argc, char *argv[]) {
	for(int i=0;i<argc;i++)
		if(DEBUG)
			std::cout <<"input: " << argv[i]<<"\n";
	struct stat scriptStatus, binStatus;
	string scriptLocation = argv[1];
	int slashIndex = scriptLocation.rfind('/');
	string scriptName = scriptLocation;
	if(slashIndex != string::npos) {
		scriptName = scriptLocation.substr(slashIndex+1);
	}
	string binName = binLocation + scriptName;
	if(stat(scriptLocation.c_str(), &scriptStatus)==-1) {
		perror("stat");
		exit(1);
	}
	int statRet = (stat(binName.c_str(), &binStatus));
	if(statRet || scriptStatus.st_mtim.tv_sec > binStatus.st_mtim.tv_sec) {
		string line;
		ifstream readStream(scriptLocation.c_str());
		string tempScriptDir = std::getenv("XDG_RUNTIME_DIR");
		if(tempScriptDir.empty()) tempScriptDir = "/tmp/";
		else if(tempScriptDir[tempScriptDir.size() - 1] != '/') tempScriptDir += '/';
		string tempScript = tempScriptDir+"tmpscript.cpp";
		ofstream writeStream(tempScript);


		getline (readStream, line); //throw away the first line.
		// Use a while loop together with the getline() function to read the file line by line
		while (getline (readStream, line)) {
			writeStream << line << '\n';
		}

		// Close the file
		readStream.close(); 
		writeStream.close();

		int pid = fork();
		if(pid == -1) {
			perror("fork");
			exit(1);
		}
		else if(pid == 0) {
			//child process
			execlp("g++", "g++","-O3" , "-o", binName.c_str(), tempScript.c_str(), NULL);
			exit(1);
		}
		else {
			//parent process
			//wait for child to finish.
			wait(NULL);
		}
	}
	if(DEBUG)
		cout << "binname" << binName <<'\n';
	execvp(binName.c_str(), argv+1);


	/* stat(filename.c_str(),&status); */
	/* cout << status.st_mtim.tv_nsec; */
}
