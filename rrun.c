#define CACHE_DIR "/home/navn/bin/opt/rrun_cache/"
#define RUST_STDLIB_PATH_ARG "link-args=-Wl,-rpath,/home/navn/nonssd/rust/rustup/toolchains/nightly-x86_64-unknown-linux-gnu/lib"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#define MD5_LENGTH 32

#include <openssl/evp.h>
#define DIGEST_LENGTH 16

typedef struct Digest {
	char value[DIGEST_LENGTH * 2 + 1];
} Digest;

Digest calculate_md5(const char * text, int len) {
    unsigned char bytes[DIGEST_LENGTH];
    EVP_MD_CTX* context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, EVP_md5(), NULL);
    EVP_DigestUpdate(context, text, len);
    unsigned int digestLength = DIGEST_LENGTH;
    EVP_DigestFinal_ex(context, bytes, &digestLength);
    EVP_MD_CTX_free(context);

    /* char digest[DIGEST_LENGTH * 2 + 1]; */
	Digest digest;
    for (int i = 0; i < DIGEST_LENGTH; i++) {
        sprintf(&digest.value[i * 2], "%02x", bytes[i]);
    }
	return digest;
}
//free the return value.
char * read_file(const char *filename, int * length) {
	char * buffer = 0;
	FILE * file = fopen (filename, "rb");
	int len;

	if(file) {
		fseek (file, 0, SEEK_END);
		len = ftell (file);
		fseek (file, 0, SEEK_SET);
		buffer = malloc (len + 1);
		if (buffer)
		{
			fread (buffer, 1, len, file);
		}
		fclose (file);
		buffer[len] = '\0';
		*length = len;
	}
	else {
		*length = -1;
	}

	return buffer;
}
int main(int argc, char **argv) {
	int length;
	char * script_path = argv[1];
	char * script_text = read_file(script_path, &length);
	char * script_name = strrchr(script_path, '/');
	if(script_name == NULL) {
		script_name = script_path;
	}
	else {
		script_name += 1;
	}
	Digest digest = calculate_md5(script_text, length);
	free(script_text);
	script_text = NULL;

	const char * calculated_md5 = digest.value;
	char * bin_file_path = malloc(sizeof(char) * (strlen(CACHE_DIR) + strlen(script_name) + MD5_LENGTH + 2)); //1 for _, 1 for \0

	if(bin_file_path){
		strcpy(bin_file_path, CACHE_DIR);
		strcat(bin_file_path,script_name);
		strcat(bin_file_path,"_");
		strcat(bin_file_path, calculated_md5);
	}
	else {
		perror("malloc failed.");
		return 1;
	}
	if(access( bin_file_path, F_OK) != 0 ) {
		//file doesn't exist. Create it.
		char * kargs[9];
		kargs[0] = "rustc";
		kargs[1] = script_path;
		kargs[2] = "-C";
		kargs[3] = "prefer-dynamic";
		kargs[4] = "-C";
		kargs[5] = RUST_STDLIB_PATH_ARG;
		kargs[6] = "-o";
		kargs[7] = bin_file_path;
		kargs[8] = NULL;

		int child_pid = fork();
		if (child_pid == 0) {
			//inside child
			execvp(kargs[0], kargs);

			//shouldn't be reached.
			perror(kargs[0]);
			return 1;
		}
		//in parent
		wait(NULL);
	}

	//try running the program
	char **jargs = malloc(sizeof(char *) * argc);
	int i=0;
	for(int j=1; j<argc; j++) { //argv[0] is rrun so ignore it.
		jargs[i++] = argv[j];
	}
	jargs[i] = NULL;
	execvp(bin_file_path, jargs);

	//shouldn't be reached.
	perror(bin_file_path);

	free(bin_file_path);
	bin_file_path = NULL;
	free(jargs);
	jargs = NULL;
}
