//seed为4个字节的unsigned char 数组指针，通过BCMSecurityAccess计算后得到4个字节的
//计算结果放入key数组中，如果seed不足4个字节，则扩充到4个字节并在后面补0

void BCMSecurityAccess(unsigned char* seed,unsigned char* key);