#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#define CACHE_DIR "/home/navn/bin/opt/krun_cache/"
#define KOTLIN_STDLIB_PATH "/usr/share/kotlin/lib/kotlin-stdlib.jar:"
#define MD5_LENGTH 32
//free the return value
char * get_kotlin_class_name(char *filename) {
	int len = strlen(filename);
	char *class_name = malloc(sizeof(char) * len); //don't need +1 as we will remove last dot
	class_name[0] = toupper(filename[0]);
	for(int i=1; i<len; i++) {
		if(filename[i] == '-')
			class_name[i] = '_';
		else
			class_name[i] = filename[i];
	}
	class_name[len - 1] = 0;
	class_name[len - 2] = 't';
	class_name[len - 3] = 'K';
	return class_name;
}
//free the return value
char * calculate_md5(const char *const data) {
	int len = strlen(data);
	unsigned char c[MD5_DIGEST_LENGTH];
	int i;
	MD5_CTX mdContext;
	char *filemd5 = (char*) malloc(33 *sizeof(char));
	MD5_Init (&mdContext);
	MD5_Update (&mdContext, data, len);
	MD5_Final (c,&mdContext);
	for(i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(&filemd5[i*2], "%02x", (unsigned int)c[i]);
	}
	return filemd5;

}
char * read_file(const char *filename) {
	char * buffer = 0;
	long length;
	FILE * f = fopen (filename, "rb");

	if (f)
	{
		fseek (f, 0, SEEK_END);
		length = ftell (f);
		fseek (f, 0, SEEK_SET);
		buffer = malloc (length + 1);
		if (buffer)
		{
			fread (buffer, 1, length, f);
		}
		fclose (f);
		buffer[length] = '\0';
	}

	return buffer;
}
int main(int argc, char **argv) {
	char * script_path = argv[1];
	char * script_text = read_file(script_path);
	char * script_name = strrchr(script_path, '/');
	if(script_name == NULL) {
		script_name = script_path;
	}
	else {
		script_name += 1;
	}
	char *calculated_md5 = calculate_md5(script_text);
	free(script_text);
	char * jar_file_path = malloc(sizeof(char) * (strlen(CACHE_DIR) + strlen(script_name) + MD5_LENGTH + strlen(".jar") + 2)); //1 for _, 1 for \0
	if(jar_file_path){
		strcpy(jar_file_path, CACHE_DIR);
		strcat(jar_file_path,script_name);
		strcat(jar_file_path,"_");
		strcat(jar_file_path, calculated_md5);
		strcat(jar_file_path, ".jar");
	}
	else {
		perror("malloc failed.");
		return 1;
	}
	if( access( jar_file_path, F_OK ) != 0 ) {
		//file doesn't exist. Create it.
		char * kargs[5];
		kargs[0] = "kotlinc";
		kargs[1] = script_path;
		kargs[2] = "-d";
		kargs[3] = jar_file_path;
		kargs[4] = NULL;

		int child_pid = fork();
		if (child_pid == 0) {
			//inside child
			execvp(kargs[0], kargs);
			perror(kargs[0]);
			return 1;
		}
		wait(NULL);
	}

	//try running the program
	char *kotlin_class_name = get_kotlin_class_name(script_name);
	char * jargs[4 + argc]; // 4 fixed args below
							// 1 null, and rest argc - 1
	jargs[0] = "java";
	jargs[1] = "-cp";
	char *cp = malloc(sizeof(char) * (strlen(KOTLIN_STDLIB_PATH) + strlen(jar_file_path) + 1));
	strcpy(cp, KOTLIN_STDLIB_PATH);
	strcat(cp, jar_file_path);
	jargs[2] = cp;
	jargs[3] = kotlin_class_name;
	const int fixed_arg_index = 3;
	//fill all argc except the first one
	for(int i=1; i<argc; i++) {
		jargs[i + fixed_arg_index] = argv[i];
	}
	jargs[argc + fixed_arg_index] = NULL;
	execvp(jargs[0], jargs);
	free(kotlin_class_name);
	perror(jar_file_path);
}
