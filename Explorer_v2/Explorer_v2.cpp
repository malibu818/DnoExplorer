// Explorer_v2.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Explorer_v2.h"


#define MAX_LOADSTRING	100
#define MAX_PATH		512

// Global variables:
HINSTANCE hInst;								// current instanse/example
TCHAR szTitle[MAX_LOADSTRING];					// The text of the title bar
TCHAR szWindowClass[MAX_LOADSTRING];			// Class name of the main window
bool reloadFileList = 1;
HWND hWndListBox1, hWndListBox2;
HWND hWndEdit1, hWndEdit2;
HWND hWndProgressBar;
TCHAR path1[MAX_PATH], path2[MAX_PATH];
TCHAR selectedFile1[MAX_PATH], selectedFile2[MAX_PATH];
int lastListBox = 0;
int id_button = ID_BUTTON_START;

// Send function declarations, included in the code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Dialog_Progress_Bar(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Dialog_Progress_Bar_Move(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Dialog_Progress_Bar_Delete(HWND, UINT, WPARAM, LPARAM);
DWORD CALLBACK CopyProgressRoutine(   //Progress bar for dialog
	_In_      LARGE_INTEGER TotalFileSize,
	_In_      LARGE_INTEGER TotalBytesTransferred,
	_In_      LARGE_INTEGER StreamSize,
	_In_      LARGE_INTEGER StreamBytesTransferred,
	_In_      DWORD dwStreamNumber,
	_In_      DWORD dwCallbackReason,
	_In_      HANDLE hSourceFile,
	_In_      HANDLE hDestinationFile,
	_In_opt_  LPVOID lpData
	);
HWND				CreateListBox(int x, int y, int width, int heigth, HWND hWnd, HMENU id);    //Creating List Area
void				LoadFileList(HWND hWndlistBox, TCHAR *path);    //Loading Files in List Box
int CALLBACK		SortUpDir(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
void				AddIconToListBox(HWND hWndListBox, int size, TCHAR c_dir[MAX_PATH]);    //Load Icons from system

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,      //main function
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: insert code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialization of global lines
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EXPLORER_V2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EXPLORER_V2));

	// The main message cycle :
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: To register window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXPLORER_V2));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_EXPLORER_V2);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance processing and creates the main window.
//
//   COMMENTS:
//
//        In this function instanse descriptor is stored in a global varaible,
//        also creates and displays the main program window
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Save instance/example descriptor in a global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1000, 600, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: To process the message in the main window.
//  WM_COMMAND	- processing the application menu
//  WM_PAINT	- Zakrasit main window
//  WM_DESTROY	 -  enter a message of exit and return.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)   //Main window procedure
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	LPNMHDR lpnmHdr = (LPNMHDR)lParam;
	LPNMLISTVIEW pnmLV = (LPNMLISTVIEW)lParam;
	TCHAR *selectedFile = 0;
	TCHAR selectedFileSize[MAX_PATH];
	TCHAR fullPathToFile[MAX_PATH];
	int k = 0;
	HWND hWndListBox = 0;
	HWND hWndEdit = 0;
	TCHAR *path = 0;
	TCHAR *disk, *disk_start;
	int x, y;

	switch (message)
	{
	case WM_CREATE:		
		InitCommonControls();
			
		disk_start = disk = new TCHAR[256];   //Loading disks
		memset(disk_start, 0, sizeof(disk_start));

		GetLogicalDrives();
		GetLogicalDriveStrings(256, (LPTSTR)disk);

		x = 10;
		y = 10;
		k = _tcslen(disk) + 1;
		while (*disk != '\0')   //Creating disk buttons
		{
			disk[1] = 0;
			CreateWindow(_T("BUTTON"), disk, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				x, y, 30, 20, hWnd, (HMENU)id_button++, NULL, NULL);

			CreateWindow(_T("BUTTON"), disk, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				x + 490, y, 30, 20, hWnd, (HMENU)id_button++, NULL, NULL);

			x += 40;
			disk += k;
		}
		delete[]disk_start;

		x = 10;
		y += 30;

		// Create edits
		hWndEdit1 = CreateWindow(_T("EDIT"), NULL, WS_BORDER | WS_VISIBLE | WS_CHILD | ES_LEFT | ES_READONLY,
			x, y, 480, 20, hWnd, (HMENU)ID_EDIT_1, NULL, NULL);

		hWndEdit2 = CreateWindow(_T("EDIT"), NULL, WS_BORDER | WS_VISIBLE | WS_CHILD | ES_LEFT | ES_READONLY,
			x + 490, y, 480, 20, hWnd, (HMENU)ID_EDIT_2, NULL, NULL);
		
		y += 30;

		hWndListBox1 = CreateListBox(
			x, y,
			480, 420,
			hWnd,
			(HMENU)ID_LISTBOX_1);

		hWndListBox2 = CreateListBox(
			x + 490, y,
			480, 420,
			hWnd,
			(HMENU)ID_LISTBOX_2);	
		y += 430;

		path1[0] = 0;
		path2[0] = 0;

		_tcscat_s(path1, _T("c:\\"));
		_tcscat_s(path2, _T("c:\\"));

		LoadFileList(hWndListBox1, path1);
		LoadFileList(hWndListBox2, path2);

		SetWindowText(hWndEdit1, path1);
		SetWindowText(hWndEdit2, path2);

		//Buttons for Actions

		CreateWindow(_T("BUTTON"), _T("Копирование"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			x, y, 100, 30, hWnd, (HMENU)ID_BUTTON_COPY, NULL, NULL);

		CreateWindow(_T("BUTTON"), _T("Перемещение"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			x + 120, y, 100, 30, hWnd, (HMENU)ID_BUTTON_MOVE, NULL, NULL);

		CreateWindow(_T("BUTTON"), _T("Удаление"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			x + 240, y, 100, 30, hWnd, (HMENU)ID_BUTTON_DELETE, NULL, NULL);

		break;
	case WM_NOTIFY:   //get message
		switch (lpnmHdr->code)
		{
		case NM_CLICK:
			switch (lpnmHdr->idFrom)
			{
			case ID_LISTBOX_1:
				hWndListBox = hWndListBox1;
				selectedFile = selectedFile1;
				lastListBox = 1; 
				//hWndEdit = hWndEdit1;
				//path = path1;
				break;
			case ID_LISTBOX_2:
				hWndListBox = hWndListBox2;
				selectedFile = selectedFile2;
				lastListBox = 2;
				//hWndEdit = hWndEdit2;
				//path = path2;
				break;
			default:
				break;
			}
			if (hWndListBox)
			{
				ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, 0, selectedFile, MAX_PATH);   //into selected file insert path of the file
			}
			break;
//		case NM_RETURN:
		case NM_DBLCLK:
			switch (lpnmHdr->idFrom)
			{
			case ID_LISTBOX_1:
				hWndListBox = hWndListBox1;
				hWndEdit = hWndEdit1;
				path = path1;
				break;
			case ID_LISTBOX_2:
				hWndListBox = hWndListBox2;
				hWndEdit = hWndEdit2;
				path = path2;
				break;
			default:
				break;
			}
			if (hWndListBox)
			{
				selectedFile = new TCHAR[MAX_PATH];
				ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, 0, selectedFile, MAX_PATH);

				if (_tcscmp(selectedFile, _T("..")) == 0)	// Go up one directory
				{
					path[_tcslen(path) - 1] = 0;
					for (int i = _tcslen(path) - 1; i > 0; i--)
					{
						TCHAR s;
						s = path[i];
						if (s == '\\')
						{
							k = i;
							break;
						}
					}
					path[k + 1] = 0;
				}
				else if (_tcscmp(selectedFile, _T(".")) == 0)	// go to the root of the disk
				{
					path[3] = 0;
				}
				else
				{
					ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, 1, selectedFileSize, MAX_PATH);
					if (_tcscmp(selectedFileSize, _T("<Папка>")) == 0)
					{
						_tcscat_s(path, MAX_PATH, _T("\\"));
						path[_tcslen(path) - 1] = 0;
						_tcscat_s(path, MAX_PATH, selectedFile);
						_tcscat_s(path, MAX_PATH, _T("\\"));
					}
					else
					{
						fullPathToFile[0] = 0;
						_tcscpy_s(fullPathToFile, path);
						_tcscat_s(fullPathToFile, selectedFile);
						ShellExecute(0, _T("open"), fullPathToFile, NULL, NULL, SW_SHOWNORMAL);
						reloadFileList = 0;
					}
				}
				if (reloadFileList)
				{
					lastListBox = 0;
					SetWindowText(hWndEdit, path);
					LoadFileList(hWndListBox, path);
				}
				delete[]selectedFile;
			}
			break;
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		//Disassemble the choice in the menu :
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_BUTTON_COPY:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_PROGRESS_BAR), hWnd, Dialog_Progress_Bar);
			break;
		case ID_BUTTON_MOVE:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_PROGRESS_BAR), hWnd, Dialog_Progress_Bar_Move);
			break;
		case ID_BUTTON_DELETE:
			fullPathToFile[0] = 0;
			switch (lastListBox) {
			case 1:
				_tcscpy_s(fullPathToFile, path1);
				_tcscat_s(fullPathToFile, selectedFile1);
				break;
			case 2:
				_tcscpy_s(fullPathToFile, path2);
				_tcscat_s(fullPathToFile, selectedFile2);
				break;
			}
			DeleteFile(fullPathToFile);
			reloadFileList = 1;
			lastListBox = 0;
			SetWindowText(hWndEdit1, path1);
			LoadFileList(hWndListBox1, path1);
			lastListBox = 0;
			SetWindowText(hWndEdit2, path2);
			LoadFileList(hWndListBox2, path2);
			break;
		default:
			if (wmId >= ID_BUTTON_START && wmId < id_button)
			{
				if (wmId % 2 == 0)
				{
					hWndListBox = hWndListBox1;
					hWndEdit = hWndEdit1;
					path = path1;
				}
				else
				{
					hWndListBox = hWndListBox2;
					hWndEdit = hWndEdit2;
					path = path2;
				}
				GetWindowText(GetDlgItem(hWnd, wmId),path,MAX_PATH);
				_tcscat_s(path, MAX_PATH, _T(":\\"));
				SetWindowText(hWndEdit, path);
				LoadFileList(hWndListBox, path);
				lastListBox = 0;
			}
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  Add any drawing code...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for the window "About".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//Dialog for copying
INT_PTR CALLBACK Dialog_Progress_Bar(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)    
{
	TCHAR lpExistingFileName[MAX_PATH];
	TCHAR lpNewFileName[MAX_PATH];
	switch (message)
	{
	case WM_INITDIALOG:
		switch (lastListBox)
		{
		case 0:
			EndDialog(hDlg, LOWORD(IDCANCEL));
		case 1:
			//get paths
			_tcscpy_s(lpExistingFileName, path1);
			_tcscat_s(lpExistingFileName, selectedFile1);
			_tcscpy_s(lpNewFileName, path2);
			_tcscat_s(lpNewFileName, selectedFile1);
			CopyFileEx(lpExistingFileName, lpNewFileName, CopyProgressRoutine, GetDlgItem(hDlg, IDC_PROGRESS1), 0, COPY_FILE_FAIL_IF_EXISTS);
			reloadFileList = 1;
			//EndDialog(hDlg, LOWORD(wParam));
			break;
		case 2:
			_tcscpy_s(lpExistingFileName, path2);
			_tcscat_s(lpExistingFileName, selectedFile2);
			_tcscpy_s(lpNewFileName, path1);
			_tcscat_s(lpNewFileName, selectedFile2);
			CopyFileEx(lpExistingFileName, lpNewFileName, CopyProgressRoutine, GetDlgItem(hDlg, IDC_PROGRESS1), 0, COPY_FILE_FAIL_IF_EXISTS);
			reloadFileList = 1;
			//EndDialog(hDlg, LOWORD(wParam));
			break;
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			//close the window
			reloadFileList = 1;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//Dialog for moving
INT_PTR CALLBACK Dialog_Progress_Bar_Move(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR lpExistingFileName[MAX_PATH];
	TCHAR lpNewFileName[MAX_PATH];
	switch (message)
	{
	case WM_INITDIALOG:
		switch (lastListBox)
		{
		case 0:
			EndDialog(hDlg, LOWORD(IDCANCEL));
		case 1:
			_tcscpy_s(lpExistingFileName, path1);
			_tcscat_s(lpExistingFileName, selectedFile1);
			_tcscpy_s(lpNewFileName, path2);
			_tcscat_s(lpNewFileName, selectedFile1);
			MoveFileWithProgress(lpExistingFileName, lpNewFileName, CopyProgressRoutine, GetDlgItem(hDlg, IDC_PROGRESS1), MOVEFILE_COPY_ALLOWED);
			reloadFileList = 1;
			//EndDialog(hDlg, LOWORD(wParam));
			break;
		case 2:
			_tcscpy_s(lpExistingFileName, path2);
			_tcscat_s(lpExistingFileName, selectedFile2);
			_tcscpy_s(lpNewFileName, path1);
			_tcscat_s(lpNewFileName, selectedFile2);
			MoveFileWithProgress(lpExistingFileName, lpNewFileName, CopyProgressRoutine, GetDlgItem(hDlg, IDC_PROGRESS1), MOVEFILE_COPY_ALLOWED);
			reloadFileList = 1;
			//EndDialog(hDlg, LOWORD(wParam));
			break;
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			reloadFileList = 1;
			EndDialog(hDlg, LOWORD(wParam));
			lastListBox = 0;
			SetWindowText(hWndEdit1, path1);
			LoadFileList(hWndListBox1, path1);
			lastListBox = 0;
			SetWindowText(hWndEdit2, path2);
			LoadFileList(hWndListBox2, path2);
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//Dialog for deleting
INT_PTR CALLBACK Dialog_Progress_Bar_Delete(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR lpExistingFileName[MAX_PATH];
	TCHAR lpNewFileName[MAX_PATH];
	switch (message)
	{
	case WM_INITDIALOG:
		switch (lastListBox)
		{
		case 0:
			EndDialog(hDlg, LOWORD(IDCANCEL));
		case 1:
			_tcscpy_s(lpExistingFileName, path1);
			_tcscat_s(lpExistingFileName, selectedFile1);
			DeleteFile(lpExistingFileName);
			reloadFileList = 1;
			//EndDialog(hDlg, LOWORD(wParam));
			break;
		case 2:
			_tcscpy_s(lpExistingFileName, path2);
			_tcscat_s(lpExistingFileName, selectedFile2);
			DeleteFile(lpExistingFileName);
			reloadFileList = 1;
			//EndDialog(hDlg, LOWORD(wParam));
			break;
		}
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			reloadFileList = 1;
			EndDialog(hDlg, LOWORD(wParam));
			

			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//Progress Bar Handling
DWORD CALLBACK CopyProgressRoutine(
	_In_      LARGE_INTEGER TotalFileSize,
	_In_      LARGE_INTEGER TotalBytesTransferred,
	_In_      LARGE_INTEGER StreamSize,
	_In_      LARGE_INTEGER StreamBytesTransferred,
	_In_      DWORD dwStreamNumber,
	_In_      DWORD dwCallbackReason,
	_In_      HANDLE hSourceFile,
	_In_      HANDLE hDestinationFile,
	_In_opt_  LPVOID lpData
	)
{
	switch (dwCallbackReason)
	{
	case CALLBACK_CHUNK_FINISHED:
		SendMessage((HWND)lpData, PBM_SETPOS, (TotalBytesTransferred.QuadPart / TotalFileSize.QuadPart * 100), 0);
		break;
	}
	if (TotalBytesTransferred.QuadPart == TotalFileSize.QuadPart) {
		
		reloadFileList = 1;
	}
	//update file list
	lastListBox = 0;
	SetWindowText(hWndEdit1, path1);
	LoadFileList(hWndListBox1, path1);
	lastListBox = 0;
	SetWindowText(hWndEdit2, path2);
	LoadFileList(hWndListBox2, path2);
	return PROGRESS_CONTINUE;
}

HWND CreateListBox(int x, int y, int width, int heigth, HWND hWnd, HMENU id)
{
	HWND hWndListBox;
	LVCOLUMN lvc;	// column structure

	hWndListBox = CreateWindow(WC_LISTVIEW,
		_T(""),
		WS_CHILD
		| WS_VISIBLE	// Visible
		| WS_BORDER		// Frame
		| ES_READONLY
		| LVS_REPORT	// Table style
		//		| LVS_NOSORTHEADER 
		//		| WS_SIZEBOX	// Izmenyaemi razmer
		,
		x, y,
		width, heigth,
		hWnd,
		id,
		0,
		0);

	ListView_SetExtendedListViewStyle(hWndListBox,
		ListView_GetExtendedListViewStyle(hWndListBox)
		| LVS_EX_FULLROWSELECT	// Select the entire row
		//		| LVS_EX_GRIDLINES		// show table grid
		);
	//--color configuration--//
	ListView_SetBkColor(hWndListBox, 0x00DA70D6);	//0x00bbggrr
	ListView_SetTextColor(hWndListBox, 0x00000000);
	ListView_SetTextBkColor(hWndListBox, 0x00DA70D6);
	//--column addition--//
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = 0;			// column number
	lvc.pszText = _T("Имя");	// column name
	lvc.cx = 200;               // column width
	lvc.fmt = LVCFMT_LEFT;		// press text on the left side
	ListView_InsertColumn(hWndListBox, 0, &lvc);

	lvc.iSubItem = 1;			// column number
	lvc.pszText = _T("Размер");	// column name
	lvc.cx = 90;				// column width
	lvc.fmt = LVCFMT_RIGHT;		// press text on the right side

	ListView_InsertColumn(hWndListBox, 1, &lvc);

	lvc.iSubItem = 2;			// column number
	lvc.pszText = _T("Дата");	// column name
	lvc.cx = 100;				// column width
	ListView_InsertColumn(hWndListBox, 2, &lvc);

	return hWndListBox;
}

void LoadFileList(HWND hWndListBox, TCHAR *path)
{
	LVITEM lvi;					// структура текста в колонке
	WIN32_FIND_DATA fileInfo;	// переменная для загрузки данных об одном файле
	HANDLE findFile;			// Указатель на файл
	int i, j, k, iTmp;
	LARGE_INTEGER fileSize;		// Размер файла
	SYSTEMTIME fileDate;		// Дата изменения файла
	TCHAR cTmp[256], cTmp2[256];
	TCHAR path2[MAX_PATH];
	
	SendMessage(hWndListBox, LVM_DELETEALLITEMS, 0, 0);

	path2[0] = 0;
	_tcscat_s(path2, path);
	_tcscat_s(path2, _T("*"));

	//--Добавление текста--//
	memset(&lvi, 0, sizeof(lvi));	// Zero struct's Members
	lvi.mask = LVIF_IMAGE | LVIF_TEXT | LVIF_PARAM;;			// Text Style
	lvi.cchTextMax = 256;			// Максимальная длина текста

	findFile = FindFirstFile(path2, &fileInfo);	// Загрузка данных о первом файле
	if (findFile != INVALID_HANDLE_VALUE)
	{
		i = 0;
		do
		{
			//--Вывод имени файла--//
			lvi.iItem = i;							// номер строки
			lvi.iImage = i;
			lvi.iSubItem = 0;						// номер колонки
			lvi.pszText = fileInfo.cFileName;		// Текст строки
			lvi.lParam = i;
			ListView_InsertItem(hWndListBox, &lvi);

			//--Вывод размера файла--//
			if (fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				ListView_SetItemText(hWndListBox, i, 1, _T("<Папка>"));
			}
			else
			{
				fileSize.LowPart = fileInfo.nFileSizeLow;
				fileSize.HighPart = fileInfo.nFileSizeHigh;
				_stprintf_s(cTmp, 256, _T("%lld"), fileSize);
				iTmp = _tcslen(cTmp);
				if (iTmp > 3)
				{
					_tcscpy_s(cTmp2, cTmp);
					k = 0;
					for (j = 0; j < iTmp; j++)
					{
						if ((iTmp - j) % 3 == 0 && j)
						{
							cTmp[k] = ' ';
							k++;
						}
						cTmp[k] = cTmp2[j];
						k++;
					}
					cTmp[k] = 0;
				}
				ListView_SetItemText(hWndListBox, i, 1, cTmp);
			}
			//--Вывод даты файла--//
			FileTimeToSystemTime(&fileInfo.ftLastWriteTime, &fileDate);		// преобразование даты в другую структуру
			_stprintf_s(cTmp, 256, _T("%02d.%02d.%04d %02d:%02d"), fileDate.wDay, fileDate.wMonth, fileDate.wYear, fileDate.wHour, fileDate.wMinute);
			ListView_SetItemText(hWndListBox, i, 2, cTmp);

			i++;
		} while (FindNextFile(findFile, &fileInfo));		// Загрузка данных о следующем файле
	}
	AddIconToListBox(hWndListBox, i, path2);
	ListView_SortItemsEx(hWndListBox, SortUpDir, hWndListBox);
}

//Sorting directory
int CALLBACK SortUpDir(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	TCHAR word1[256], word2[256];
	LVFINDINFO find;
	int i1, i2;

	if (lParam1 == 0) return -1;
	else if (lParam2 == 0) return 1;

	find.flags = LVFI_PARAM;
	find.lParam = lParam1;
	ListView_GetItemText((HWND)lParamSort,
		i1 = ListView_FindItem((HWND)lParamSort, -1, &find),
		1, word1, 256);

	find.lParam = lParam2;
	ListView_GetItemText((HWND)lParamSort,
		i2 = ListView_FindItem((HWND)lParamSort, -1, &find),
		1, word2, 256);

	if (_tcscmp(word1, _T("<Папка>")) == 0 &&
		_tcscmp(word2, _T("<Папка>")) == 0)
	{
		return 0;
	}
	else if (_tcscmp(word1, _T("<Папка>")) == 0)
	{
		return -1;
	}
	else if (_tcscmp(word2, _T("<Папка>")) == 0)
	{
		return 1;
	}
	else return 0;
}

void AddIconToListBox(HWND hWndListBox, int size, TCHAR c_dir[MAX_PATH])
{
	HIMAGELIST hSmall;
	SHFILEINFO lp;
	TCHAR buf1[MAX_PATH];
	DWORD num;

	hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_MASK | ILC_COLOR32, size + 2, 1);
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(c_dir, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		MessageBox(0, _T("Ошибка"), _T("Не найден"), MB_OK | MB_ICONWARNING);
	}
	else
	{
		do 
		{//присваеваем атрибуты
			if (_tcscmp(FindFileData.cFileName, _T(".")) == 0)
			{//если диск
				_tcscpy_s(buf1, c_dir);
				_tcscat_s(buf1, FindFileData.cFileName);
				SHGetFileInfo(_T(""), FILE_ATTRIBUTE_DEVICE, &lp, sizeof(lp), SHGFI_ICONLOCATION | SHGFI_ICON | SHGFI_SMALLICON);

				ImageList_AddIcon(hSmall, lp.hIcon);
				DestroyIcon(lp.hIcon);

			}
			if (_tcscmp(FindFileData.cFileName, _T("..")) == 0)
			{//если фаилы,папки
				_tcscpy_s(buf1, c_dir);
				_tcscat_s(buf1, FindFileData.cFileName);
				SHGetFileInfo(_T(""), FILE_ATTRIBUTE_DIRECTORY, &lp, sizeof(lp), SHGFI_ICONLOCATION | SHGFI_ICON | SHGFI_SMALLICON);

				ImageList_AddIcon(hSmall, lp.hIcon);
				DestroyIcon(lp.hIcon);
			}
			//присваеваем иконки
			_tcscpy_s(buf1, c_dir);
			buf1[_tcslen(buf1) - 1] = 0;

			_tcscat_s(buf1, FindFileData.cFileName);
			num = GetFileAttributes(buf1);
			SHGetFileInfo(buf1, num, &lp, sizeof(lp), SHGFI_ICONLOCATION | SHGFI_ICON | SHGFI_SMALLICON);

			ImageList_AddIcon(hSmall, lp.hIcon);
			DestroyIcon(lp.hIcon);

		} while (FindNextFile(hFind, &FindFileData) != 0);

		FindClose(hFind);
	}
	ListView_SetImageList(hWndListBox, hSmall, LVSIL_SMALL);
}

