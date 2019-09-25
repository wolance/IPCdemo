#ifndef Common_h__
#define Common_h__

#include <Windows.h>

wchar_t tmp_tmp_wc[1024] = {0};
char tmp_tmp_c[1024] = {0};

wchar_t* c2w(wchar_t *pwstr,size_t len,const char *str)
{	
	if(!pwstr) pwstr = tmp_tmp_wc;
	ZeroMemory(tmp_tmp_wc,1024);
	if(str)
	{
		size_t nu = strlen(str);
		size_t n =(size_t)MultiByteToWideChar(CP_ACP,0,(const char *)str,(int)nu,NULL,0);
		if(n >= len)
			n=len-1;
		MultiByteToWideChar(CP_ACP,0,(const char *)str,(int)nu,pwstr,(int)n);
		pwstr[n]=0;
	}
	return pwstr;
}

char* w2c(char *str, const wchar_t *pwstr,size_t len)
{
	if(!str) str = tmp_tmp_c;
	ZeroMemory(tmp_tmp_c,1024);
	int nlength = wcslen(pwstr);
	int nbytes = WideCharToMultiByte( 0, // specify the code page used to perform the conversion
		0, // no special flags to handle unmapped characters
		pwstr,     // wide character string to convert
		nlength,   // the number of wide characters in that string
		NULL,      // no output buffer given, we just want to know how long it needs to be
		0,
		NULL,      // no replacement character given
		NULL );    // we don't want to know if a character didn't make it through the translation

	// make sure the buffer is big enough for this, making it larger if necessary

	if(nbytes > (int)len)   
		nbytes=len;

	// 通过以上得到的结果，转换unicode 字符为ascii 字符

	WideCharToMultiByte( 0, // specify the code page used to perform the conversion
		0,         // no special flags to handle unmapped characters
		pwstr,   // wide character string to convert
		nlength,   // the number of wide characters in that string
		str, // put the output ascii characters at the end of the buffer
		nbytes,                           // there is at least this much space there
		NULL,      // no replacement character given
		NULL );
	str[nbytes] = '\0';
	return str ;
}

#endif // Common_h__
