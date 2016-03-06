//--------------------------------------------------------------------------------
// Written by Justin Murphy
//--------------------------------------------------------------------------------
#include "./MainWindow.h"
#include "./IWindProc.h"
#include "../Log.h"
//--------------------------------------------------------------------------------
LRESULT CALLBACK GlobalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	LONG_PTR ObjPtr = GetWindowLongPtr(hWnd, 0);

	if (0 == ObjPtr) {
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	else {
		return reinterpret_cast<IWindProc *>(ObjPtr)->WindowProc(hWnd, uMsg, wParam, lParam);
	}
}
//--------------------------------------------------------------------------------
void CMainWindow::SetSize(int width, int height) {
	m_nWindowWidth = width;
	m_nWindowHeight = height;
}
//--------------------------------------------------------------------------------
CMainWindow::CMainWindow(int nWindowWidth, int nWindowHeight, const wchar_t* szWindowName) : m_nWindowWidth( nWindowWidth ), m_nWindowHeight( nWindowHeight ),
													m_hWnd(0), m_dStyle(WS_OVERLAPPEDWINDOW | WS_VISIBLE), m_sCaption(szWindowName), m_bIsActive(false),
													m_szWindowName(szWindowName) {
	// Only one primary window should be created
}
//--------------------------------------------------------------------------------
CMainWindow::~CMainWindow() {
	//A thread cannot use DestroyWindow to destroy a window created by a different thread. 
	//
	//if (!DestroyWindow(m_hWnd)) {
	//	LogError();
	//}
	//Console, spawned from this window, makes this call fail
	//if (!UnregisterClass(m_szWindowName, m_hInstance)) {
	//	LogError();
	//}
}
//--------------------------------------------------------------------------------
void CMainWindow::Initialize(IWindProc* _object) {
	WNDCLASSEX wc;

	// Setup the window class
	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	//wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = GlobalWndProc;
	wc.cbWndExtra = sizeof(&GlobalWndProc);
	wc.hInstance = GetModuleHandle(NULL);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = m_szWindowName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	_object->BeforeRegisterWindowClass(wc);

	// Register the window class
	RegisterClassEx(&wc);

	// Create an instance of the window
	m_hWnd = CreateWindowEx(
		NULL,							// extended style
		wc.lpszClassName, 				// class name
		m_sCaption.c_str(),				// instance title
		m_dStyle,						// window style
		(GetSystemMetrics(SM_CXSCREEN) - m_nWindowWidth) >> 1, //Starting X
		(GetSystemMetrics(SM_CYSCREEN) - m_nWindowHeight) >> 1, //Starting Y
		m_nWindowWidth,					// initial width
		m_nWindowHeight,				// initial height
		NULL,							// handle to parent 
		NULL,							// handle to menu
		NULL,							// instance of this application
		NULL);							// extra creation parms

	if (m_hWnd) {
		// Set in the "extra" bytes the pointer to the IWindowProc object
		// which handles messages for the window
		SetWindowLongPtr(m_hWnd, 0, reinterpret_cast<LONG_PTR>(_object) );

		m_hInstance = wc.hInstance;

		Log("Created The windows window");
		return;
	}
	
	Log("error Creating the windows window");
}
//--------------------------------------------------------------------------------
void CMainWindow::DebugFlags() {

	LONG lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	LONG lExStyle = GetWindowLong(m_hWnd, GWL_EXSTYLE);
	RECT rect;
	GetClientRect(m_hWnd, &rect);
	Log("DebugFlags Style: ", lStyle, " EXstyle: ", lExStyle, " Right ", rect.right, " Bottom ", rect.bottom, " top", rect.top, " left ", rect.left);
}
//--------------------------------------------------------------------------------
bool CMainWindow::HasMessages() {
	MSG msg;
	if( 0 == PeekMessage( &msg, 0, 0U, 0U, PM_NOREMOVE ) )
		return false;
	return true;
}
//--------------------------------------------------------------------------------
const HWND CMainWindow::GetHWnd() const {
	return m_hWnd;
}
//--------------------------------------------------------------------------------
bool CMainWindow::IsActive() { 
	return m_bIsActive; 
}
//--------------------------------------------------------------------------------
bool CMainWindow::MessagePump() {
	MSG msg;
	PeekMessage( &msg, 0, 0U, 0U, PM_REMOVE );

	if( WM_QUIT == msg.message )
		return false;

	TranslateMessage( &msg );
	DispatchMessage( &msg );

	return true;
}
//--------------------------------------------------------------------------------