#include "..\\arbitro\util.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>

DWORD WINAPI ThreadLeitura(LPVOID param) {
    HANDLE hPipe = (HANDLE)param;
    DADOS resposta;
    DWORD lidos;
    while (1) {
        OVERLAPPED ovRead = { 0 };
        ovRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        BOOL ok = ReadFile(hPipe, &resposta, sizeof(DADOS), &lidos, &ovRead);
        if (!ok && GetLastError() == ERROR_IO_PENDING) {
            WaitForSingleObject(ovRead.hEvent, INFINITE);
            GetOverlappedResult(hPipe, &ovRead, &lidos, FALSE);
        }
        CloseHandle(ovRead.hEvent);
        if (lidos == sizeof(DADOS)) {
            _tprintf(_T("\n[MENSAGEM] %s\n> "), resposta.msg);
            fflush(stdout);
        }
        else if (lidos == 0) {
            break; // Pipe fechado
        }
    }
    return 0;
}


int _tmain(int argc, TCHAR* argv[]) {
#ifdef UNICODE
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
#endif

    if (argc < 2) {
        _tprintf(_T("[USO] jogoui <username>\n"));
        return 1;
    }

    const TCHAR* username = argv[1];
    HANDLE hPipe;

    // Conectar ao pipe
    while (1) {
        hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        if (hPipe != INVALID_HANDLE_VALUE)
            break;
        _tprintf(_T("[INFO] Aguardando árbitro...\n"));
        Sleep(1000);
    }


    // Enviar estrutura DADOS para login
    DADOS login = { 0 };
    _tcscpy_s(login.username, TAM_BUFFER, username);
    _tcscpy_s(login.comando, TAM_BUFFER, _T(":login"));
    _tcscpy_s(login.msg, TAM_BUFFER, _T("Entrar"));

    OVERLAPPED ov = { 0 };
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);


    if (!WriteFile(hPipe, &login, sizeof(DADOS), NULL, &ov)) {
        if (GetLastError() == ERROR_IO_PENDING) {
            WaitForSingleObject(ov.hEvent, INFINITE);
        }
    }
    else {
        _tprintf(_T("[CLIENTE/BOT] Vai enviar mensagem para o árbitroas\n"));
    }
    CloseHandle(ov.hEvent);
    HANDLE hThreadLeitura = CreateThread(NULL, 0, ThreadLeitura, (LPVOID)hPipe, 0, NULL);

    _tprintf(_T("[INFO] Bem-vindo ao jogo, %s!\n"), username);

    TCHAR input[BUFFER_SIZE];

    while (1) {
        _tprintf(_T("\nComandos: :pont (pontuação), :jogs (jogadores), :sair\n> "));
        _fgetts(input, BUFFER_SIZE, stdin);
        input[_tcslen(input) - 1] = _T('\0'); // Remove newline

        DADOS mensagem = { 0 };
        _tcscpy_s(mensagem.username, TAM_BUFFER, argv[1]);

        OVERLAPPED ovWrite = { 0 };
        ovWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        if (_tcscmp(input, _T(":sair")) == 0) {
            _tcscpy_s(mensagem.comando, TAM_BUFFER, _T(":sair"));
            DWORD written = 0;
            if (!WriteFile(hPipe, &mensagem, sizeof(DADOS), &written, &ovWrite)) {
                if (GetLastError() == ERROR_IO_PENDING) {
                    WaitForSingleObject(ovWrite.hEvent, INFINITE);
                }
            }
            else
                _tprintf(_T("[CLIENTE/BOT] Vai enviar mensagem para o árbitro3\n"));

            CloseHandle(ovWrite.hEvent);
            break;
        }
        else if (_tcscmp(input, _T(":pont")) == 0 || _tcscmp(input, _T(":jogs")) == 0) {
            _tcscpy_s(mensagem.comando, TAM_BUFFER, input);
            DWORD written = 0;
            if (!WriteFile(hPipe, &mensagem, sizeof(DADOS), &written, &ovWrite)) {
                if (GetLastError() == ERROR_IO_PENDING) {
                    WaitForSingleObject(ovWrite.hEvent, INFINITE);
                }
            }
            _tprintf(_T("[CLIENTE/BOT] Vai enviar mensagem para o árbitro2\n"));

            CloseHandle(ovWrite.hEvent);

        }
        else {
            _tcscpy_s(mensagem.comando, TAM_BUFFER, _T(":msg"));
            _tcscpy_s(mensagem.msg, TAM_BUFFER, input);
            DWORD written = 0;
            if (!WriteFile(hPipe, &mensagem, sizeof(DADOS), &written, &ovWrite)) {
                if (GetLastError() == ERROR_IO_PENDING) {
                    WaitForSingleObject(ovWrite.hEvent, INFINITE);
                }
            }
            else
                _tprintf(_T("[CLIENTE/BOT] Vai enviar mensagem para o árbitro4\n"));
            CloseHandle(ovWrite.hEvent);
        }
    }


    WaitForSingleObject(hThreadLeitura, INFINITE);
    CloseHandle(hThreadLeitura);
    CloseHandle(hPipe);
    return 0;
}
