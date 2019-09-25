//windows剪贴板
//跨进程不跨网络通信

#include <Windows.h>
#include <stdio.h>

#define MSGHEADA "[CLIPPED_BOARD][FROMA]"
#define MSGHEADB "[CLIPPED_BOARD][FROMB]"

DWORD WINAPI Send(LPVOID lpParameter )
{
	char buff[0xFFF];
	DWORD dwd;
	ZeroMemory(buff,sizeof(buff));
	while(true)
	{
		gets_s(buff,sizeof(buff));
		HGLOBAL hClip = 0;
		if (OpenClipboard(0))
		{
			EmptyClipboard();
			hClip=GlobalAlloc(GMEM_MOVEABLE,0xFFF);
			char *pbuff=(char*)GlobalLock(hClip);
			strcpy(pbuff,MSGHEADB);
			strcat(pbuff,buff);
			GlobalUnlock(hClip);
			SetClipboardData(CF_TEXT,hClip);
			CloseClipboard();
			GlobalFree(hClip);
			hClip = 0;
		}
	}
	return 0;
}

LRESULT CALLBACK Recv(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int val = uMsg;
	char tmp[512] = {0};
	if(val ==  WM_CLIPBOARDUPDATE )	
	{

		HGLOBAL hClip = 0;
		if (OpenClipboard(0))
		{
			char* buff;
			hClip=GetClipboardData(CF_TEXT);
			if(hClip) buff=(char*)GlobalLock(hClip);
			else 
			{
				CloseClipboard();
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
			if(buff!=0 && memcmp(buff,MSGHEADA,sizeof(MSGHEADA)-1) ==0)
			{
				printf("FromA: %s\n",buff+sizeof(MSGHEADA)-1);
			}
			else
			{
				GlobalUnlock(hClip);
				CloseClipboard();
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
			GlobalUnlock(hClip);
			CloseClipboard();
		}
		else 
		{
			printf("open error\n");
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}	
	else
	{
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

int main(int argc, char* argv[])
{
	WNDCLASS wndcls; 
	wndcls.cbClsExtra = 0 ;
	wndcls.cbWndExtra = 0 ;
	wndcls.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH ) ;
	wndcls.hCursor = LoadCursor( NULL , IDC_CROSS ) ;
	wndcls.hIcon = LoadIcon( NULL , IDI_APPLICATION ) ;
	wndcls.hInstance = NULL;
	wndcls.lpfnWndProc = Recv;
	wndcls.lpszClassName = "ClippedBoardA_Wnd";
	wndcls.lpszMenuName = NULL;
	wndcls.style = CS_HREDRAW | CS_VREDRAW ;

	RegisterClass( &wndcls ) ; 
	HWND hwnd;
	hwnd = CreateWindow("ClippedBoardA_Wnd","ClippedBoardA_Wnd",WS_OVERLAPPEDWINDOW,
		200,100,640,480,NULL,NULL,NULL,NULL) ;

	AddClipboardFormatListener(hwnd);

	HANDLE hSendThread;
	DWORD hSendThreadID;

	hSendThread = CreateThread(NULL,0,Send,NULL,NULL,&hSendThreadID);

	MSG msg;
	while( GetMessage( &msg , hwnd , 0 , 0 ) )
	{
		TranslateMessage( &msg ) ;
		DispatchMessage( &msg ) ;
	}
	return 0;
}

