#pragma once

#undef UNICODE

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <ctype.h>
#include <string>
#include <regex>
#include <fcntl.h>
#include <io.h>      //저수준 파일 입출력
#include <stdio.h>
#include <shellapi.h> //for hyperlink
/* GDI PLUS HEADER*/
#include <objidl.h>
#include <gdiplus.h>
//#include <gdiplusgraphics.h>

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")
#pragma comment(lib, "ws2_32.lib") 
/* GDI PLUS HEADER END*/

#include <CommCtrl.h>// for progress , status bar

#include "resource.h" //마우스 오른쪽 버튼 팝업 메뉴를 위한 리소스 헤더 추가
#include "vector" 


//#define PORT_NUM 8979 //CGI PORT NUMBER 경미님
//#define PORT_NUM 8677 //CGI PORT NUMBER 효근님
//#define PORT_NUM 8090 //CGI PORT NUMBER 재민님
//#define PORT_NUM 8777 //CGI PORT NUMBER 효범님
//#define PORT_NUM 8887 // 유민님
#define MAXLEN 1024
#define TEMP_SIZE 1000000
//#define RBUFSIZE 1000000
#define IDC_MAIN_BUTTON 101
#define IDC_VISITPAGE_BUTTON 103 //다운로드 버튼
#define BACK_BUTTON 104 //이전페이지 버튼
#define FRONT_BUTTON 105 //다음페이지 버튼
#define REFRESH_BUTTON 106 //새로고침 버튼
#define FAVORITE_BUTTON 107 //즐겨찾기 버튼
#define IDC_MAIN_EDIT 102 //에딧텍스트
#define ID_LISTBOX 100 //디렉토리 검색


#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#pragma warning(disable:4996)

HWND Main_hWnd;

WNDCLASS WndClass;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SubEditProc(HWND, UINT, WPARAM, LPARAM); //for subclassing
LPCTSTR lpszClass = "이상재의 웹브라우저";


WNDPROC OldEditProc; //for subclassing


static HWND EdittextBox;
static HWND button;
static HWND textField;
static HWND g_OldEditProc;
static HWND g_OldButtonProc;

HWND hProg; //Progress bar
HWND hState; //상태바(하단)
HWND hList; //디렉토리 탐색
HWND directory_button; //디렉토리 버튼
HWND back_button; //뒤로가기 버튼
HWND front_button; //앞으로가기 버튼
HWND refresh_button; //앞으로가기 버튼
HWND favorite_button; //즐겨찾기 버튼

HINSTANCE g_hInst; /* for progress bar*/
char textbox_buffer[256] = { 0 };
char temp_addr2[20] = { 0 };
int temp_port2 = 0;

char *dns_query_addr = NULL;

long bufsize = 0; //동적 할당을 위해 사이즈를 누적해 나감
char rbuf[MAXLEN] = { 0 };

int clientsocket(char *, int);
using namespace std;

int cases = 0;

std::vector<string> vector_page;
std::vector<string> vector_all_page;//모든 방문했던 페이지
string totalresult;
char * wow = NULL;


HMENU hMenu = CreateMenu(), hPopup = CreatePopupMenu(); //메뉴를 전역에서 생성해놓고 추가함
int visit_pagenumber = 50001; //즐겨찾기 버튼 넘버

int __cdecl dns(char *hostname)
{
	//-----------------------------------------
	// Declare and initialize variables
	WSADATA wsaData;
	ZeroMemory(&wsaData, sizeof(wsaData));
	int iResult = 0;
	INT iRetval = 0;

	DWORD dwRetval;
	ZeroMemory(&dwRetval, sizeof(dwRetval));

	int i = 1;

	struct addrinfo *result;
	ZeroMemory(&result, sizeof(result));
	struct addrinfo *ptr;
	ZeroMemory(&ptr, sizeof(ptr));
	struct addrinfo hints;
	ZeroMemory(&hints, sizeof(hints));
	struct sockaddr_in  *sockaddr_ipv4;
	ZeroMemory(&sockaddr_ipv4, sizeof(sockaddr_ipv4));
	
	//struct sockaddr_in6 *sockaddr_ipv6;
	LPSOCKADDR sockaddr_ip;
	ZeroMemory(&sockaddr_ip, sizeof(sockaddr_ip));

	char ipstringbuffer[46] = { 0 };
	DWORD ipbufferlength = 46;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	//--------------------------------
	// Setup the hints address info structure
	// which is passed to the getaddrinfo() function
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//printf("Calling getaddrinfo with following parameters:\n");
	//printf("\tnodename = %s\n", argv[1]);
	//printf("\tservname (or port) = %s\n\n", argv[2]);

	//--------------------------------
	// Call getaddrinfo(). If the call succeeds,
	// the result variable will hold a linked list
	// of addrinfo structures containing response
	// information
	dwRetval = getaddrinfo(hostname, 0, &hints, &result);
	if (dwRetval != 0) {
		printf("getaddrinfo failed with error: %d\n", dwRetval);
		WSACleanup();
		return 1;
	}

	printf("getaddrinfo returned success\n");

	// Retrieve each address and print out the hex bytes
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		printf("getaddrinfo response %d\n", i++);
		printf("\tFlags: 0x%x\n", ptr->ai_flags);
		printf("\tFamily: ");
		switch (ptr->ai_family) {
		case AF_UNSPEC:
			printf("Unspecified\n");
			break;
		case AF_INET:
			printf("AF_INET (IPv4)\n");
			sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
			printf("\tIPv4 address %s\n",
				inet_ntoa(sockaddr_ipv4->sin_addr));
			dns_query_addr = inet_ntoa(sockaddr_ipv4->sin_addr);
			break;
		case AF_INET6:
			printf("AF_INET6 (IPv6)\n");

			// We use WSAAddressToString since it is supported on Windows XP and later
			sockaddr_ip = (LPSOCKADDR)ptr->ai_addr;
			// The buffer length is changed by each call to WSAAddresstoString
			// So we need to set it for each iteration through the loop for safety
			ipbufferlength = 46;
			iRetval = WSAAddressToString(sockaddr_ip, (DWORD)ptr->ai_addrlen, NULL,
				ipstringbuffer, &ipbufferlength);
			if (iRetval)
				printf("WSAAddressToString failed with %u\n", WSAGetLastError());
			else
				printf("\tIPv6 address %s\n", ipstringbuffer);
			break;
		case AF_NETBIOS:
			printf("AF_NETBIOS (NetBIOS)\n");
			break;
		default:
			printf("Other %ld\n", ptr->ai_family);
			break;
		}
		printf("\tSocket type: ");
		switch (ptr->ai_socktype) {
		case 0:
			printf("Unspecified\n");
			break;
		case SOCK_STREAM:
			printf("SOCK_STREAM (stream)\n");
			break;
		case SOCK_DGRAM:
			printf("SOCK_DGRAM (datagram) \n");
			break;
		case SOCK_RAW:
			printf("SOCK_RAW (raw) \n");
			break;
		case SOCK_RDM:
			printf("SOCK_RDM (reliable message datagram)\n");
			break;
		case SOCK_SEQPACKET:
			printf("SOCK_SEQPACKET (pseudo-stream packet)\n");
			break;
		default:
			printf("Other %ld\n", ptr->ai_socktype);
			break;
		}
		printf("\tProtocol: ");
		switch (ptr->ai_protocol) {
		case 0:
			printf("Unspecified\n");
			break;
		case IPPROTO_TCP:
			printf("IPPROTO_TCP (TCP)\n");
			break;
		case IPPROTO_UDP:
			printf("IPPROTO_UDP (UDP) \n");
			break;
		default:
			printf("Other %ld\n", ptr->ai_protocol);
			break;
		}
		printf("\tLength of this sockaddr: %d\n", ptr->ai_addrlen);
		printf("\tCanonical name: %s\n", ptr->ai_canonname);
	}

	freeaddrinfo(result);
	WSACleanup();

	return 0;
}

void progressbar(int Pos)
{
	if (Pos > 100) Pos = 100;
	SendMessage(hProg, PBM_SETPOS, Pos, 0);
}
