#include<stdio.h>

#include <dlfcn.h>

//extern void complete(char * _filePath,char *json_result, char *access_token);
//std::string licensePlate_result;

int main(void){
	char  json_result[300];
	char  stToken[300] = "24.a04a429e5e17463a511f45d55cc7f5b4.2592000.1634431842.282335-24612626";
	void *libCPPTest = dlopen("/mnt/hgfs/share_18.04/9_车牌识别/libCppLibTest.so", RTLD_NOW);
	int (*cpp_func)(char *,char*,char*) = (void (*)(char *,char*,char*))dlsym(libCPPTest, "_Z8cpp_funci");
	cpp_func("chepai.png",json_result, stToken);
	
	
	return 0;
	
}