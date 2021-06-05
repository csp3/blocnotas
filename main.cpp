#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif
#include <tchar.h>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <CommCtrl.h>

#define TXTcontenedor 1010 
#define LBXnumeros 1011
#define BTNbold 1012
#define ID_EDITCHILD 1013

using namespace std;

/*handlers*/
HWND hTXTcontenedor;
HWND hLBXnumeros;
HWND hBTNbold;

/*procedures*/
WNDPROC DefineEditProcedure;

/*declaracion de procedures y funciones*/
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT EditProcedure(HWND, UINT, WPARAM, LPARAM);
LPWSTR strTOlpw(string data);

/*nombre de la Clase, como variable global*/
TCHAR szClassName[] = _T("CodeBlocksWindowsApp");

void teclaVirtual(int);
void PosicionEnLinea(HWND hwnd, int& numlinea, int& indexPenultima, DWORD& wc, int& carAvaLin);

int numeroLineas = 1, capturaLinea = 1;

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    HWND hwnd;               /*  manipulador para la ventana principal */
    MSG messages;            /* para manipular los mensajes de nuestra aplicacion */
    WNDCLASSEX wincl;        /* Data structure para registrar el tipo de clase de ventana de nuestra aplicacion */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* Procesa los Mensajes (esta abajo) */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* no tenemos información adicional para la ventana */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = (HBRUSH)COLOR_BACKGROUND; /* usaremos color de fondo por defecto */

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx(&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx(
        0,                   /* estilo extendido de la ventana */
        szClassName,         /* Classname */
        _T("Code::Blocks Template Windows Apps"),       /* Titulo de la ventana */
        WS_OVERLAPPEDWINDOW, /* default window */
        CW_USEDEFAULT,       /* posicion x */
        CW_USEDEFAULT,       /* posicion y */
        800,                 /* ancho */
        500,                 /* alto */
        HWND_DESKTOP,        /* manejador de la ventana padre o propietaria */
        NULL,                /* No menu */
        hThisInstance,       /* manejador de la instancia de la aplicación */
        NULL                 /* puntero a los datos de creación de la ventana */
    );

    /* Make the window visible on the screen */
    ShowWindow(hwnd, SW_MAXIMIZE);
    UpdateWindow(hwnd);

    /*bucles de mensajes, Se ejecutará hasta que GetMessage () devuelva 0 */
    /*BOOL GetMessage(
        LPMSG lpMsg, // puntero a la estructura que contendrá el mensaje
        HWND hWnd, // manipulador de ventana
        UINT wMsgFilterMin, // primer mensaje
        UINT wMsgFilterMax // último mensaje
      );
    */
    while (TRUE == GetMessage(&messages, NULL, 0, 0))
    {
        /* Traducir mensajes de teclas virtuales a mensajes de caracteres */
        TranslateMessage(&messages);

        /* Enviar mensaje al WindowProcedure */
        DispatchMessage(&messages);
    }

    /*salimos retornando, he program return-value is 0 */
    return messages.wParam;
}


/*Función es la encargada de procesar todo desde los comandos o mensajes*/
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HFONT hFont;

    switch (message)
    {
    case WM_SIZE:
    {
        RECT re;
        GetClientRect(hwnd, &re);
        SetWindowPos(hTXTcontenedor, NULL, 60, 50, re.right - 60, re.bottom - 50, SWP_NOZORDER);
        SetWindowPos(hLBXnumeros, NULL, 0, 50, 60, re.bottom - 50, SWP_NOZORDER);
        break;
    }
    case WM_CREATE:
    {
        hTXTcontenedor = CreateWindowEx(0, WC_EDIT, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_TABSTOP, 0, 0, 80, 40, hwnd, NULL, NULL, NULL);

        DefineEditProcedure = (WNDPROC)SetWindowLongA(hTXTcontenedor, -4, (long)EditProcedure);

        hLBXnumeros = CreateWindowEx(0, WC_LISTBOX, TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | ES_AUTOVSCROLL, 300, 10, 70, 200, hwnd, NULL, NULL, NULL);

        SendMessage(hLBXnumeros, LB_ADDSTRING, 0, (LPARAM)strTOlpw("1"));

        hBTNbold = CreateWindowEx(0, WC_BUTTON, TEXT("B"), WS_CHILD | WS_VISIBLE | ES_CENTER | WS_VISIBLE | BS_PUSHBUTTON, 10, 0, 20, 20, hwnd, (HMENU)BTNbold, NULL, NULL);

        SetFocus(hTXTcontenedor);

        //--fuente--//
        hFont = CreateFont(13, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("arial"));
        SendMessage(hTXTcontenedor, WM_SETFONT, (WPARAM)hFont, 0);
        SendMessage(hLBXnumeros, WM_SETFONT, (WPARAM)hFont, 0);
        //------------------------------------------------------------------------------------------//
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case BTNbold:
            switch (HIWORD(wParam))
            {
            case BN_CLICKED:
            {
                hFont = CreateFont(13, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_DONTCARE, TEXT("arial"));
                SendMessage(hTXTcontenedor, WM_SETFONT, (WPARAM)hFont, 0);
                SetFocus(hTXTcontenedor);
                teclaVirtual(1);
                teclaVirtual(2);
                break;
            }
            }
            break;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}
/*Then add this template*/
LRESULT EditProcedure(HWND hwndTxt, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int numlinea = 0, indexPenultima = 0, indexAntepenultima = 0, indexActual = 0, indexPosterior = 0, nuevaPos = 0, carAvaLin = 0;
    DWORD wc = 0, lc = 0;

    switch (uMsg)
    {
        case WM_KEYDOWN:
        {
            /*numero de lineas de Edit lo da EM_GETLINECOUNT*/
            numeroLineas = SendMessage(hwndTxt, EM_GETLINECOUNT, 0, 0);
            if (capturaLinea < numeroLineas && wParam != VK_BACK) /*no contar tecla BACK*/
            {
                /*limpiar listbox*/
                SendMessage(hLBXnumeros, LB_RESETCONTENT, 0, 0);
                /*agrega item*/
                capturaLinea = numeroLineas;
                for (int i = 0; i < numeroLineas; i++)
                    SendMessage(hLBXnumeros, LB_ADDSTRING, 0, (LPARAM)strTOlpw(to_string(i + 1)));
            }
            else
            {
                /*elimina item desde abajo*/
                if (capturaLinea >= numeroLineas)
                {
                    capturaLinea = numeroLineas;
                    SendMessage(hLBXnumeros, LB_DELETESTRING, capturaLinea, 0);
                }
                if (wParam == 0x56)
                {
                    teclaVirtual(1);
                    teclaVirtual(2);
                    teclaVirtual(1);
                    teclaVirtual(2);
                }
            }

            if (wParam == VK_UP)
            {
                numlinea = 0; indexPenultima = 0; indexAntepenultima = 0; nuevaPos = 0; carAvaLin = 0;
                wc = 0;

                PosicionEnLinea(hwndTxt, numlinea, indexPenultima, wc, carAvaLin);

                /*cantidad de caracteres antepenultima linea*/
                indexAntepenultima = SendMessage(hwndTxt, EM_LINEINDEX, (WPARAM)(numlinea - 1), 0);

                if (indexPenultima - indexAntepenultima - 2 < carAvaLin)
                    nuevaPos = indexPenultima - 2;
                else
                    nuevaPos = carAvaLin + indexAntepenultima;

                SendMessage(hwndTxt, EM_SETSEL, (WPARAM)nuevaPos, (LPARAM)nuevaPos);
            }

            if (wParam == VK_DOWN)
            {
                numlinea = 0; indexPenultima = 0; indexActual = 0; indexPosterior = 0, nuevaPos = 0, carAvaLin = 0;
                wc = 0;

                PosicionEnLinea(hwndTxt, numlinea, indexPenultima, wc, carAvaLin);

                /*cantidad de caracteres actual linea*/
                indexActual = SendMessage(hwndTxt, EM_LINEINDEX, (WPARAM)(numlinea + 1), 0);
                /*cantidad de caracteres posterior linea*/
                indexPosterior = SendMessage(hwndTxt, EM_LINEINDEX, (WPARAM)(numlinea + 2), 0);

                if (indexPosterior - indexActual < carAvaLin)
                    nuevaPos = (indexPosterior - indexActual - 2) + indexActual;
                else
                    nuevaPos = carAvaLin + indexActual;

                SendMessage(hwndTxt, EM_SETSEL, (WPARAM)nuevaPos, (LPARAM)nuevaPos);
            }

            if (wParam == VK_RIGHT)
            {
                /*cantidad de caracteres hasta donde estoy posicionado*/
                SendMessage(hwndTxt, EM_GETSEL, (WPARAM)&wc, (LPARAM)&lc);
                wc = wc + 1;
                SendMessage(hwndTxt, EM_SETSEL, (WPARAM)wc, (LPARAM)wc);
            }

            if (wParam == VK_LEFT)
            {
                /*cantidad de caracteres hasta donde estoy posicionado*/
                SendMessage(hwndTxt, EM_GETSEL, (WPARAM)&wc, (LPARAM)&lc);
                wc = wc - 1;
                if (wc < 0) wc = 0;
                SendMessage(hwndTxt, EM_SETSEL, (WPARAM)wc, (LPARAM)wc);
            }
        }
            break;
        case WM_VSCROLL:
        {
            MessageBox(hwndTxt, _T("mm"), _T("mm"), MB_OK);
        }
            break;
    default:
        return CallWindowProc(DefineEditProcedure, hwndTxt, uMsg, wParam, lParam);
    }

    return 0;
}

void PosicionEnLinea(HWND hwnd, int& numlinea, int& indexPenultima, DWORD& wc, int& carAvaLin)
{
    /*numero de linea donde estoy posicionado*/
    /*el indice de EM_LINEFROMCHAR empieza en 0*/
    numlinea = SendMessage(hwnd, EM_LINEFROMCHAR, (WPARAM)-1, 0);
    /*cantidad de caracteres hasta la penultima linea donde estoy*/
    /*el indice de EM_LINEINDEX empieza en 1*/
    indexPenultima = SendMessage(hwnd, EM_LINEINDEX, (WPARAM)numlinea, 0);
    /*cantidad de caracteres hasta donde estoy posicionado*/
    SendMessage(hwnd, EM_GETSEL, (WPARAM)&wc, (LPARAM)0);

    carAvaLin = wc - indexPenultima;
}

/*convertir string A lpwstr*/
LPWSTR strTOlpw(string data)
{
    LPWSTR datos = new wchar_t[data.size() + 1];
    datos[data.size()] = 0;
    copy(data.begin(), data.end(), datos);
    return datos;
}

void teclaVirtual(int tecla1)
{
    /*Esta estructura se utilizará para crear el teclado
      Evento de entrada*/
    INPUT ip;
    /*Pausa por 0.3 segundos*/
    Sleep(100);
    /*Configura un evento de teclado genérico*/
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0; /*código de escaneo de hardware para la llave*/
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    /*tecla codigo-virtual*/
    switch (tecla1)
    {
    case 1: // espacio 20
        ip.ki.wVk = 0x20;
        break;
    case 2: // retroceso 08
        ip.ki.wVk = 0x08;
        break;
    default:
        break;
    }
    ip.ki.dwFlags = 0; /*0 para presionar una tecla*/
    SendInput(1, &ip, sizeof(INPUT));
    /*Suelta la tecla*/
    ip.ki.dwFlags = KEYEVENTF_KEYUP; /*KEYEVENTF_KEYUP para liberación de clave*/
    SendInput(1, &ip, sizeof(INPUT));
}

/*
const char texto[128] = {}, car[128] = {};
GetKeyNameText(lParam, (LPWSTR)car, 64);
MessageBox(hwnd,(LPWSTR)car,strTOlpw(to_string((int)wParam)),MB_OK);
switch (wParam)
{
case VK_UP:
    MessageBox(hwnd, TEXT("11"), TEXT("22"), MB_OK);
    break;
default:
    break;
}
if ( wParam == 0x41 )
{
    char nn[4] = {};
    GetWindowTextA(hTXTlineas, nn, sizeof(nn));
    MessageBox(hwnd, strTOlpw(nn), TEXT(""), MB_OK);
}
*/