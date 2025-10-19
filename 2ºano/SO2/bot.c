// bot.c - Versão atualizada com shared memory e protocolo estruturado
#include "..\\arbitro\util.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    HANDLE hPipe;
    TCHAR username[MAX_USERNAME];
    SDATA* pShm;
    HANDLE hMutex;
    HANDLE hEvento;
    HANDLE hMap;
    int pontuacao;
    int tempo_reacao;
    BOOL continua;
} BotState;


DWORD WINAPI ThreadLeitura(LPVOID data) {
    HANDLE hPipe = (HANDLE)data;
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

BOOL podeFormarPalavra(const TCHAR* palavra, const TCHAR* letras) {
    TCHAR letrasTemp[MAXLETRAS_LIMITE];
    _tcscpy_s(letrasTemp, MAXLETRAS_LIMITE, letras);

    for (int i = 0; palavra[i]; i++) {
        BOOL achou = FALSE;
        for (int j = 0; letrasTemp[j]; j++) {
            if (palavra[i] == letrasTemp[j]) {
                letrasTemp[j] = _T('_');
                achou = TRUE;
                break;
            }
        }
        if (!achou) return FALSE;
    }
    return TRUE;
}

int _tmain(int argc, TCHAR* argv[]) {
#ifdef UNICODE
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
#endif

    if (argc < 2) {
        _tprintf_s(_T("Uso: bot <username>\n"));
        return 1;
    }

    srand((unsigned int)time(NULL));

    BotState bot = { 0 };
    _tcscpy_s(bot.username, MAX_USERNAME, argv[1]);
    bot.tempo_reacao = (rand() % 26) + 5; // 5-30 segundos
    bot.pontuacao = 0;
    bot.continua = TRUE;

    _tprintf_s(_T("[BOT] %s iniciado. Tempo reação: %d seg\n"), bot.username, bot.tempo_reacao);

    // Conectar ao pipe
    while (1) {
        bot.hPipe = CreateFile(PIPE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
        if (bot.hPipe != INVALID_HANDLE_VALUE) break;
        Sleep(1000);
    }

    // Acessar shared memory
    bot.hMap = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, SHM_NAME);
    if (!bot.hMap) {
        _tprintf_s(_T("[BOT] Erro ao abrir shared memory: %d\n"), GetLastError());
        CloseHandle(bot.hPipe);
        return 1;
    }

    bot.pShm = (SDATA*)MapViewOfFile(bot.hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!bot.pShm) {
        _tprintf_s(_T("[BOT] Erro ao mapear memória: %d\n"), GetLastError());
        CloseHandle(bot.hMap);
        CloseHandle(bot.hPipe);
        return 1;
    }

    bot.hMutex = OpenMutex(SYNCHRONIZE, FALSE, MUTEX_LETRAS);
    if (!bot.hMutex) {
        _tprintf_s(_T("[BOT] Erro ao abrir mutex: %d\n"), GetLastError());
        UnmapViewOfFile(bot.pShm);
        CloseHandle(bot.hMap);
        CloseHandle(bot.hPipe);
        return 1;
    }
	// abrir o evento de shared memory para sincronização das letras
    bot.hEvento = OpenEvent(SYNCHRONIZE, FALSE, EVENTO_LETRAS);
    if (!bot.hEvento) {
        _tprintf_s(_T("[BOT] Erro ao abrir evento: %d\n"), GetLastError());
        CloseHandle(bot.hMutex);
        UnmapViewOfFile(bot.pShm);
        CloseHandle(bot.hMap);
        CloseHandle(bot.hPipe);
        return 1;
    }


    // Registrar no árbitro
    DADOS mensagem = { 0 };
    _tcscpy_s(mensagem.username, TAM_BUFFER, bot.username);
    _tcscpy_s(mensagem.comando, TAM_BUFFER, _T(":login"));
    _tcscpy_s(mensagem.msg, TAM_BUFFER, _T("Bot conectado"));

    OVERLAPPED ovWrite = { 0 };
    ovWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    BOOL writeSuccess = WriteFile(bot.hPipe, &mensagem, sizeof(DADOS), NULL, &ovWrite);
    _tprintf(_T("[CLIENTE/BOT] Vai enviar mensagem para o árbitro\n"));


    HANDLE hThreadLeitura = CreateThread(NULL, 0, ThreadLeitura, (LPVOID)bot.hPipe, 0, NULL);

    // Carregar dicionário (abordagem botteste: mapeamento + parsing linha a linha)
    TCHAR dicionario[MAX_PALAVRA][MAX_TAMANHO_PALAVRA];
    int num_palavras = 0;

    HANDLE hFile = CreateFile(_T("dicionario.txt"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        _tprintf(_T("[BOT] Erro ao abrir dicionario.txt (CreateFile): %lu\n"), GetLastError());
        UnmapViewOfFile(bot.pShm);
        CloseHandle(bot.hMap);
        CloseHandle(bot.hMutex);
        CloseHandle(bot.hEvento);
        CloseHandle(bot.hPipe);
        return 1;
    }

    HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hMap == NULL) {
        _tprintf(_T("[BOT] Erro ao criar mapeamento de arquivo: %lu\n"), GetLastError());
        CloseHandle(hFile);
        UnmapViewOfFile(bot.pShm);
        CloseHandle(bot.hMap);
        CloseHandle(bot.hMutex);
        CloseHandle(bot.hEvento);
        CloseHandle(bot.hPipe);
        return 1;
    }

    char* pStr = (char*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
    if (pStr == NULL) {
        _tprintf(_T("[BOT] Erro ao mapear arquivo: %lu\n"), GetLastError());
        CloseHandle(hMap);
        CloseHandle(hFile);
        UnmapViewOfFile(bot.pShm);
        CloseHandle(bot.hMap);
        CloseHandle(bot.hMutex);
        CloseHandle(bot.hEvento);
        CloseHandle(bot.hPipe);
        return 1;
    }

    DWORD tam = GetFileSize(hFile, NULL);

    
    int i = 0, j = 0;
    char palavra[MAX_TAMANHO_PALAVRA];

    while (i < (int)tam && num_palavras < MAX_PALAVRA) {
        if (pStr[i] == '\n' || pStr[i] == '\r') {
            if (j > 0) {
                palavra[j] = '\0';
                // Converter para TCHAR (Unicode ou ANSI)
                MultiByteToWideChar(CP_UTF8, 0, palavra, -1, dicionario[num_palavras], MAX_TAMANHO_PALAVRA);
                num_palavras++;
                j = 0;
            }
            // Pular \r\n juntos
            if (pStr[i] == '\r' && i + 1 < (int)tam && pStr[i + 1] == '\n') i++;
        }
        else {
            if (j < MAX_TAMANHO_PALAVRA - 1) {
                palavra[j++] = pStr[i];
            }
        }
        i++;
    }
    // Última palavra (caso não termine com \n)
    if (j > 0 && num_palavras < MAX_PALAVRA) {
        palavra[j] = '\0';
        MultiByteToWideChar(CP_UTF8, 0, palavra, -1, dicionario[num_palavras], MAX_TAMANHO_PALAVRA);
        num_palavras++;
    }

    UnmapViewOfFile(pStr);
    CloseHandle(hMap);
    CloseHandle(hFile);

    _tprintf(_T("[BOT] Dicionário carregado com %d palavras\n"), num_palavras);
    _tprintf(_T("sizeof(DADOS) = %d\n"), (int)sizeof(DADOS));


    // Loop principal
    while (bot.continua) {
        // Verificar mensagens do árbitro primeiro
        DADOS resposta;
        DWORD lidos;
        OVERLAPPED ovRead = { 0 };
        ovRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        if (PeekNamedPipe(bot.hPipe, NULL, 0, NULL, &lidos, NULL) && lidos > 0) {
            ResetEvent(ovRead.hEvent);
            BOOL rSuccess = ReadFile(bot.hPipe, &resposta, sizeof(DADOS), NULL, &ovRead);

            if (!rSuccess && GetLastError() == ERROR_IO_PENDING) {
                WaitForSingleObject(ovRead.hEvent, 100); // Timeout de 100ms
            }

            if (GetOverlappedResult(bot.hPipe, &ovRead, &lidos, FALSE)) {
                if (lidos == sizeof(DADOS)) {
                    // Processar mensagem
                    if (_tcscmp(resposta.comando, _T(":pont")) == 0) {
                        _tprintf_s(_T("[BOT %s] Pontuação atual: %s\n"), bot.username, resposta.msg);
                    }
                    else if (_tcscmp(resposta.comando, _T(":sair")) == 0) {
                        _tprintf_s(_T("[BOT %s] Recebeu comando para sair\n"), bot.username);
                        bot.continua = FALSE;
                        break;
                    }
                }
            }
        }

        if (!bot.continua) break;
        // Verificar letras atuais
        TCHAR letrasAtuais[MAXLETRAS_LIMITE + 1] = { 0 };
        if (WaitForSingleObject(bot.hMutex, INFINITE) == WAIT_OBJECT_0) {
            for (int i = 0;  i < bot.pShm->maxLetras; i++) {
                letrasAtuais[i] = bot.pShm->letras[i];
            }
            ReleaseMutex(bot.hMutex);
        }
        else {
            _tprintf_s(_T("[BOT] Erro ao esperar mutex\n"));
            break;
        }

        // Esperar tempo de reação
        Sleep(bot.tempo_reacao * 1000);

        // Procurar palavra válida
        TCHAR palavraEscolhida[MAX_TAMANHO_PALAVRA] = _T("");
        for (int i = 0; i < num_palavras; i++) {
            if (podeFormarPalavra(dicionario[i], letrasAtuais)) {
                _tcscpy_s(palavraEscolhida, MAX_TAMANHO_PALAVRA, dicionario[i]);
                break;
            }
        }

        if (palavraEscolhida[0] != _T('\0')) {
            // Enviar palavra
            OVERLAPPED ovWriteMsg = { 0 };
            ovWriteMsg.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            DADOS mensagem = { 0 };
            _tcscpy_s(mensagem.username, TAM_BUFFER, bot.username); // Preencher username
            _tcscpy_s(mensagem.comando, TAM_BUFFER, _T(":msg"));
            _tcscpy_s(mensagem.msg, TAM_BUFFER, palavraEscolhida);
            WriteFile(bot.hPipe, &mensagem, sizeof(DADOS), NULL, &ovWriteMsg);
            _tprintf(_T("[CLIENTE/BOT] Vai enviar mensagem para o árbitro2\n"));

            CloseHandle(ovWriteMsg.hEvent);
            _tprintf_s(_T("[BOT %s] Enviou palavra: %s\n"), bot.username, palavraEscolhida);
        }

        // Verificar mensagens do árbitro

        OVERLAPPED ovReadMsg = { 0 };
        ovReadMsg.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        while (PeekNamedPipe(bot.hPipe, NULL, 0, NULL, &lidos, NULL) && lidos > 0) {
            DADOS resposta;
            ResetEvent(ovReadMsg.hEvent);
            BOOL rSuccess = ReadFile(bot.hPipe, &resposta, sizeof(DADOS), NULL, &ovReadMsg);
            if (!rSuccess && GetLastError() == ERROR_IO_PENDING) {
                if (WaitForSingleObject(ovReadMsg.hEvent, 100) == WAIT_OBJECT_0) {
                    if (_tcscmp(resposta.comando, _T(":sair")) == 0) {
                        bot.continua = FALSE;
                        _tprintf_s(_T("[BOT %s] Recebeu comando para sair\n"), bot.username);
                        break;
                    }
                }
            }
            if (lidos == sizeof(DADOS)) {
                if (_tcscmp(resposta.comando, _T(":pont")) == 0)
                    _tprintf_s(_T("[BOT %s] Pontuação atual: %s\n"), bot.username, resposta.msg);
                else if (_tcscmp(resposta.comando, _T(":sair")) == 0) {
                    _tprintf_s(_T("[BOT %s] Recebeu comando para sair\n"), bot.username);
                    bot.continua = FALSE;
                }
            }
        }
        CloseHandle(ovReadMsg.hEvent);
    }
    WaitForSingleObject(hThreadLeitura, INFINITE);
    CloseHandle(hThreadLeitura);

    UnmapViewOfFile(bot.pShm);
    CloseHandle(bot.hMap);
    CloseHandle(bot.hMutex);
    CloseHandle(bot.hEvento);
    CloseHandle(bot.hPipe);

    return 0;
}