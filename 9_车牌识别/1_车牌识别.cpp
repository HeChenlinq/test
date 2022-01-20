#include <stdio.h>
#include <iostream>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <stdlib.h>
#include "base64.h"
#include "base64.h"
#include <string.h>
#include <fstream>
#include "cpp.h"


//access_token返回解包
static size_t callback(void *ptr, size_t size, size_t nmemb, void *stream); 
//request返回解包
static size_t callback1(void *ptr, size_t size, size_t nmemb, void *stream);
//获取access_token
int get_access_token(std::string &access_token, const std::string &AK, const std::string &SK);
					
//获取车牌号
int licensePlate(std::string &json_result, const std::string &access_token,std::string &base_data); 
//图片转base64编码
std::string GetBase64(std::string _filePath);
//json解码获取number,color
int parseJSON(const char* jsonstr,std::string &number,std::string &color);








/*识别后返回的结果保存在licensePlate_result*/
static std::string licensePlate_result;

/*access_token_url*/
const std::string access_token_url ="https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials";

/*request_url*/
const static std::string request_url ="https://aip.baidubce.com/rest/2.0/ocr/v1/license_plate";


/*ak,sk根据自己账号获取*/
const std::string access_AK = "mQMLDV2hbhyaLCVwPaCGFIGd";

const std::string access_token_SK = "T88wFiLp9j6UQS6cYLdDeoGmyZihHNaw";


int main(int argc,char * argv[])
{
    int ret = 1;
    if(argc != 2)
    {
	printf("usage:<%s><photo_path>\r\n",argv[0]);
	return -1;
    }
    std::string  json_result;
    std::string stToken = "24.a04a429e5e17463a511f45d55cc7f5b4.2592000.1634431842.282335-24612626";
    std::string base_data;
    std::string number;
    std::string color;
#if 1		/*获取stToken，每月刷新*/
    ret = get_access_token(stToken,access_AK,access_token_SK);
    if(ret == 0)
    {
	printf("success\r\n");
	std::cout << "获取Token：" << stToken << std::endl;
    }else{
	printf("fail\r\n");
    }
    printf("------------------------------------------------------\r\n");
#endif

    //图片转码
    base_data = GetBase64(argv[1]);
    //printf("======================================================> base :%s\r\n",base_data.c_str());
    //车牌识别
    ret =  licensePlate(json_result, stToken,base_data);
    if(ret)
    {
	printf("licensePlate fail\r\n");
	printf("ret = %d\n",ret);
	return -1;
    }
    if(strstr(licensePlate_result.c_str(),"error_code") != NULL)
    {
	printf("licensePlate fail\r\n");
	printf("=========>%s\n",licensePlate_result.c_str());
	return -1;
    }
    printf("=========>%s\n",licensePlate_result.c_str());

    if( !parseJSON(licensePlate_result.c_str(),number,color))
    {
	printf("number => %s\ncolor =>%s\n",number.c_str(),color.c_str());
    }
	
	
    return 0;
}
/**
 * curl发送http请求调用的回调函数，回调函数中对返回的json格式的body进行了解析，解析结果储存在result中
 * @param 参数定义见libcurl库文档
 * @return 返回值定义见libcurl库文档
 */
static size_t callback(void *ptr, size_t size, size_t nmemb, void *stream) {
    // 获取到的body存放在ptr中，先将其转换为string格式
    std::string s((char *) ptr, size * nmemb);
    // 开始获取json中的access token项目
    Json::Reader reader;
    Json::Value root;
    // 使用boost库解析json
    reader.parse(s,root);
    std::string* access_token_result = static_cast<std::string*>(stream);
    *access_token_result = root["access_token"].asString();
    return size * nmemb;
}

/**
 * 用以获取access_token的函数，使用时需要先在百度云控制台申请相应功能的应用，获得对应的API Key和Secret Key
 * @param access_token 获取得到的access token，调用函数时需传入该参数
 * @param AK 应用的API key
 * @param SK 应用的Secret key
 * @return 返回0代表获取access token成功，其他返回值代表获取失败
 */
int get_access_token(std::string &access_token, const std::string &AK, const std::string &SK) 
{
    CURL *curl;
    CURLcode result_code;
    int error_code = 0;
    curl = curl_easy_init();
    if (curl) {
	std::string url = access_token_url + "&client_id=" + AK + "&client_secret=" + SK;
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	std::string access_token_result;
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &access_token_result);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
	result_code = curl_easy_perform(curl);
	if (result_code != CURLE_OK) {
	    fprintf(stderr,"result_code = %d\n",result_code);
	    fprintf(stderr, "result_code = %d--------curl_easy_perform() failed: %s\n",result_code,
		    curl_easy_strerror(result_code));
	    return 1;
	}
	access_token = access_token_result;
	curl_easy_cleanup(curl);
	error_code = 0;
    } else {
	fprintf(stderr, "curl_easy_init() failed.");
	error_code = 1;
    }
    return error_code;
}


/**
 * 车牌识别
 * @return 调用成功返回0，发生错误返回其他错误码
 */
int licensePlate(std::string &json_result, const std::string &access_token,std::string &base_data) 
{
    std::string url = request_url + "?access_token=" + access_token;
    printf("%s\n",url.c_str());
    CURL *curl = NULL;
    CURLcode result_code;
    int is_success;
    curl = curl_easy_init();
    if (curl) {
	printf("---------------------\r\n");
	curl_easy_setopt(curl, CURLOPT_SSLVERSION,3);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_URL, url.data());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_httppost *post = NULL;
	curl_httppost *last = NULL;
	curl_formadd(&post, &last, CURLFORM_COPYNAME, "image", CURLFORM_COPYCONTENTS, base_data.c_str(), CURLFORM_END);

	curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback1);
	result_code = curl_easy_perform(curl);
	if (result_code != CURLE_OK) {
	    fprintf(stderr,"result_code = %d\n",result_code);
	    fprintf(stderr, "---curl_easy_perform() failed: %s\n",
		    curl_easy_strerror(result_code));
	    is_success = 1;
	    return is_success;
	}
	json_result = licensePlate_result;
	curl_easy_cleanup(curl);
	is_success = 0;
    } else {
	fprintf(stderr, "curl_easy_init() failed.");
	is_success = 1;
    }
	printf( "\n--\n%s\n--\n",json_result.c_str());
    return is_success;
}

/**
 * curl发送http请求调用的回调函数，回调函数中对返回的json格式的body进行了解析，解析结果储存在全局的静态变量当中
 * @param 参数定义见libcurl文档
 * @return 返回值定义见libcurl文档
 */
static size_t callback1(void *ptr, size_t size, size_t nmemb, void *stream)
{
//	std::string licensePlate_result;
    // 获取到的body存放在ptr中，先将其转换为string格式
    licensePlate_result = std::string((char *) ptr, size * nmemb);
	//printf( "\n-1-\n%s\n--\n",licensePlate_result.c_str());
    return size * nmemb;
}


std::string GetBase64(std::string _filePath)
{
    std::fstream f, f2;
    f.open(_filePath.c_str(),std::ios::in|std::ios::binary);//"c:\\ocr.jpg"
    f.seekg(0, std::ios_base::end);
    std::streampos sp = f.tellg();
    int size = sp;
    std::cout << size << std::endl;
    char* buffer = (char*)malloc(sizeof(char)*size);
    f.seekg(0, std::ios_base::beg);//把文件指针移到到文件头位置
    f.read(buffer, size);
    std::cout << "file size:" << size << std::endl;
    std::string imgBase64 = aip::base64_encode(buffer, size);
    //std::cout << "img base64 encode size:" << imgBase64 << std::endl;
    f.close();
    free(buffer);
    return imgBase64;
}


int parseJSON(const char* jsonstr,std::string &number,std::string &color)
{
    Json::Reader reader;
    Json::Value root;	//建立一个json对象
    if (!reader.parse(jsonstr, root, false))
    {
	printf("bad json format!\n");
	return 1;
    }

    number =  root["words_result"]["number"].asString();
    color = root["words_result"]["color"].asString();
    return 0;
}

void complete(char * _filePath,char *json_result, char *access_token)
{
	std::string base_data;
    std::string number;
    std::string color;
	//std::string licensePlate_result;
	int ret;
	
	base_data = GetBase64((std::string &)_filePath);
    //printf("======================================================> base :%s\r\n",base_data.c_str());
    /*车牌识别*/
    ret =  licensePlate((std::string &)json_result, (std::string &)access_token,(std::string &)base_data);
    if(ret)
    {
	printf("licensePlate fail\r\n");
	printf("ret = %d\n",ret);
	return ;
    }
    if(strstr(licensePlate_result.c_str(),"error_code") != NULL)
    {
	printf("licensePlate fail\r\n");
	printf("=========>%s\n",licensePlate_result.c_str());
	return ;
    }
    printf("=========>%s\n",licensePlate_result.c_str());

    if( !parseJSON(licensePlate_result.c_str(),number,color))
    {
	printf("number => %s\ncolor =>%s\n",number.c_str(),color.c_str());
    }
	
	
}
