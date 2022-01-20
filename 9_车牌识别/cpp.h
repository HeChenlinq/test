#ifndef _CPP_H_
#define _CPP_H_

#ifdef __cplusplus

/*access_token返回解包*/
static size_t callback(void *ptr, size_t size, size_t nmemb, void *stream); 
/*request返回解包*/
static size_t callback1(void *ptr, size_t size, size_t nmemb, void *stream);
/*获取access_token*/
int get_access_token(std::string &access_token, const std::string &AK, const std::string &SK);
/*获取车牌号*/
int licensePlate(std::string &json_result, const std::string &access_token,std::string &base_data); 
/*图片转base64编码*/
std::string GetBase64(std::string _filePath);
/*json解码获取number,color*/
int parseJSON(const char* jsonstr,std::string &number,std::string &color);


extern "C" {
#endif

void complete(char * _filePath,char *json_result, char *access_token);

#ifdef __cplusplus
}
#endif



#endif 