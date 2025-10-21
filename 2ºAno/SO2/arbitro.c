// arbitro.c - Reescrita moderna e robusta do programa árbitro para Windows
// Trabalho prático SO2 - ISEC 2024/25

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include "..\\arbitro\util.h"


typedef struct EstadoJogo EstadoJogo; // Forward declaration

typedef struct {
    HANDLE hPipe;
    TCHAR username[MAX_USERNAME];
    BOOL is_bot;
    int pontuacao;
    int tempo_reacao;
    BOOL continua;
    BOOL ativo; // Indica se o jogador está ativo
    EstadoJogo* estado; // Ponteiro para o estado do jogo
} Jogador;

typedef struct EstadoJogo {
    Jogador jogadores[MAX_JOGADORES];
    int num_jogadores;
    HANDLE hMutexJogadores;
    BOOL jogoAtivo;
    int ritmo;
    int max_letras;
    int num_palavras;
    TCHAR dicionario[MAX_PALAVRA][MAX_TAMANHO_PALAVRA];
    SDATA* pMem;                // Ponteiro para memória compartilhada
    HANDLE hMap;                // Handle do file mapping
    HANDLE hMutexSM;            // Mutex para shared memory
    HANDLE hEventoLetras;       // Evento para notificar mudanças
    HANDLE hEventoJogadores;
}EstadoJogo;


int JogadoresAtivos(EstadoJogo* estado) {
    int ativos = 0;
    for (int i = 0; i < estado->num_jogadores; i++) {
        if (estado->jogadores[i].ativo && estado->jogadores[i].continua)
            ativos++;
    }
    return ativos;
}

BOOL InicializarSharedMemory(EstadoJogo* estado) {
    estado->hMap = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(SDATA),
        SHM_NAME);

    if (!estado->hMap) {
        _tprintf(_T("[ERRO] CreateFileMapping: %d\n"), GetLastError());
        return FALSE;
    }

    estado->pMem = (SDATA*)MapViewOfFile(estado->hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!estado->pMem) {
        _tprintf(_T("[ERRO] MapViewOfFile: %d\n"), GetLastError());
        CloseHandle(estado->hMap);
        return FALSE;
    }

    estado->hMutexSM = CreateMutex(NULL, FALSE, MUTEX_LETRAS);
    estado->hEventoLetras = CreateEvent(NULL, TRUE, FALSE, EVENTO_LETRAS);

    // Inicializar dados
    estado->pMem->maxLetras = estado->max_letras;
    estado->pMem->ritmo = estado->ritmo;
    for (int i = 0; i < estado->max_letras; i++) {
        estado->pMem->letras[i] = _T('_');
        estado->pMem->tempo[i] = 0;
    }

    return TRUE;
}
BOOL NomeValido(EstadoJogo* estado, const TCHAR* nome) {
    // Verifica se o nome começa com "bot" (case insensitive)
    if (_tcsnicmp(nome, _T("bot"), 3) == 0) {
        _tprintf_s(_T("[ÁRBITRO] Nomes não podem começar com 'bot'\n"));
        return FALSE;
    }

    // Verifica se o nome já existe
    WaitForSingleObject(estado->hMutexJogadores, INFINITE);

    for (int i = 0; i < estado->num_jogadores; i++) {
        if (_tcsicmp(estado->jogadores[i].username, nome) == 0) {
            _tprintf_s(_T("[ÁRBITRO] Nome '%s' já está em uso\n"), nome);
            ReleaseMutex(estado->hMutexJogadores);
            return FALSE;
        }
    }
    ReleaseMutex(estado->hMutexJogadores);
    return TRUE;
}



BOOL CarregarDicionario(EstadoJogo* estado) {
    HANDLE hFile = CreateFile(_T("dicionario.txt"), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        _tprintf(_T("[ERRO] Erro ao abrir dicionario.txt (CreateFile): %lu\n"), GetLastError());
        return FALSE;
    }

    DWORD tam = GetFileSize(hFile, NULL);
    if (tam == INVALID_FILE_SIZE || tam == 0) {
        _tprintf(_T("[ERRO] Ficheiro dicionario.txt vazio ou erro ao obter tamanho.\n"));
        CloseHandle(hFile);
        return FALSE;
    }

    HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hMap == NULL) {
        _tprintf(_T("[ERRO] Erro ao criar mapeamento de arquivo: %lu\n"), GetLastError());
        CloseHandle(hFile);
        return FALSE;
    }

    char* pStr = (char*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
    if (pStr == NULL) {
        _tprintf(_T("[ERRO] Erro ao mapear arquivo: %lu\n"), GetLastError());
        CloseHandle(hMap);
        CloseHandle(hFile);
        return FALSE;
    }

    estado->num_palavras = 0;
    int i = 0, j = 0;
    char palavra[MAX_TAMANHO_PALAVRA];

    while (i < (int)tam && estado->num_palavras < MAX_PALAVRA) {
        if (pStr[i] == '\n' || pStr[i] == '\r') {
            if (j > 0) {
                palavra[j] = '\0';
                // Converter para TCHAR (Unicode ou ANSI)
                MultiByteToWideChar(CP_UTF8, 0, palavra, -1, estado->dicionario[estado->num_palavras], MAX_TAMANHO_PALAVRA);
                estado->num_palavras++;
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
    if (j > 0 && estado->num_palavras < MAX_PALAVRA) {
        palavra[j] = '\0';
        MultiByteToWideChar(CP_UTF8, 0, palavra, -1, estado->dicionario[estado->num_palavras], MAX_TAMANHO_PALAVRA);
        estado->num_palavras++;
    }

    UnmapViewOfFile(pStr);
    CloseHandle(hMap);
    CloseHandle(hFile);

    _tprintf(_T("[ÁRBITRO] Dicionário carregado com %d palavras\n"), estado->num_palavras);
    return TRUE;
}

BOOL CarregarConfiguracoes(EstadoJogo* estado) {
    HKEY hKey;
    DWORD valor, tamanho = sizeof(DWORD);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\TrabSO2"), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        // Valores padrão
        _tprintf_s(_T("[ÁRBITRO] Chave de registro não encontrada, usando valores padrão.\n"));
        estado->max_letras = 6;
        estado->ritmo = 3;
        return TRUE;
    }

    if (RegQueryValueEx(hKey, _T("MAXLETRAS"), NULL, NULL, (LPBYTE)&valor, &tamanho) == ERROR_SUCCESS) {
        estado->max_letras = valor > MAXLETRAS_LIMITE ? MAXLETRAS_LIMITE : valor;
    }
    else
        estado->max_letras = 6;


    if (RegQueryValueEx(hKey, _T("RITMO"), NULL, NULL, (LPBYTE)&valor, &tamanho) == ERROR_SUCCESS) {
        estado->ritmo = valor;
    }
    else {
        estado->ritmo = 3;
    }

    RegCloseKey(hKey);
    return TRUE;
}
void BroadcastMensagem(EstadoJogo* estado, const TCHAR* comando, const TCHAR* msg, HANDLE hPipeExcluir) {
    DADOS resposta = { 0 };
    _tcscpy_s(resposta.comando, TAM_BUFFER, comando);
    _tcscpy_s(resposta.msg, TAM_BUFFER, msg);

    WaitForSingleObject(estado->hMutexJogadores, INFINITE);
    for (int i = 0; i < estado->num_jogadores; i++) {
        if (estado->jogadores[i].hPipe != hPipeExcluir && estado->jogadores[i].ativo) {
            OVERLAPPED ovWrite = { 0 };
            ovWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            BOOL wSuccess = WriteFile(estado->jogadores[i].hPipe, &resposta, sizeof(DADOS), NULL, &ovWrite);
            if (!wSuccess && GetLastError() == ERROR_IO_PENDING) {
                WaitForSingleObject(ovWrite.hEvent, INFINITE);
                DWORD escritos;
                GetOverlappedResult(estado->jogadores[i].hPipe, &ovWrite, &escritos, FALSE);
            }
            CloseHandle(ovWrite.hEvent);
        }
    }
    ReleaseMutex(estado->hMutexJogadores);
}

void AnunciarVencedor(EstadoJogo* estado) {
    WaitForSingleObject(estado->hMutexJogadores, INFINITE);

    int maxPontos = -999999;
    int idxVencedor = -1;
    for (int i = 0; i < estado->num_jogadores; i++) {
        if (estado->jogadores[i].ativo && estado->jogadores[i].continua) {
            if (estado->jogadores[i].pontuacao > maxPontos) {
                maxPontos = estado->jogadores[i].pontuacao;
                idxVencedor = i;
            }
            else if (estado->jogadores[i].pontuacao == maxPontos) 
                // Se houver empate, não declare vencedor
                idxVencedor = -1;
        }
    }
    
    if (idxVencedor >= 0) {
        TCHAR msg[TAM_BUFFER];
        _stprintf_s(msg, TAM_BUFFER, _T("VENCEDOR: %s com %d pontos!"),
            estado->jogadores[idxVencedor].username,
            estado->jogadores[idxVencedor].pontuacao);
        BroadcastMensagem(estado, _T(":msg"), msg, NULL);
        _tprintf(_T("[ÁRBITRO] %s\n"), msg);
    }
    else {
        BroadcastMensagem(estado, _T(":msg"), _T("Sem vencedor (Empate)."), NULL);
        //_tprintf(_T("[ÁRBITRO] Sem vencedor (Empate).\n"));
    }

    ReleaseMutex(estado->hMutexJogadores);
}

DWORD WINAPI ThreadLetras(LPVOID data) {
    EstadoJogo* estado = (EstadoJogo*)data;
    srand((unsigned int)time(NULL));

    while (estado->jogoAtivo) {
        WaitForSingleObject(estado->hEventoJogadores, INFINITE);
        WaitForSingleObject(estado->hMutexSM, INFINITE);


        // Procurar posição vazia ou letra mais antiga
        int pos = 0;
        DWORD tempoMaisAntigo = estado->pMem->tempo[0];
        BOOL encontrouVazio = FALSE;

        for (int i = 0; i < estado->pMem->maxLetras; i++) {
            if (estado->pMem->letras[i] == _T('_')) {
                pos = i;
                encontrouVazio = TRUE;
                break;
            }
            if (estado->pMem->tempo[i] < tempoMaisAntigo) {
                tempoMaisAntigo = estado->pMem->tempo[i];
                pos = i;
            }
        }

        // Gerar nova letra
        estado->pMem->letras[pos] = _T('A') + (rand() % 26);
        estado->pMem->tempo[pos] = (DWORD)time(NULL);

        // Notificar mudanças
        SetEvent(estado->hEventoLetras);
        ResetEvent(estado->hEventoLetras);
        ReleaseMutex(estado->hMutexSM);

        // Log para debug
        _tprintf(_T("[LETRAS] Atual: "));
        for (int i = 0; i < estado->pMem->maxLetras; i++) {
            _tprintf(_T("%c "), estado->pMem->letras[i]);
        }
        _tprintf(_T("\n"));

        Sleep(estado->pMem->ritmo * 3000);
    }

    return 0;
}


DWORD WINAPI TratarJogador(LPVOID data) {
    Jogador* jogador = (Jogador*)data;
    EstadoJogo* estado = jogador->estado;
    DADOS mensagem;
    DWORD lidos;
    _tprintf_s(_T("\n[ÁRBITRO] Jogador conectado: %s\n"), jogador->username);

    OVERLAPPED ovRead = { 0 };
    ovRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    _tprintf(_T("sizeof(DADOS) = %d\n"), (int)sizeof(DADOS));

    while (jogador->continua && estado->jogoAtivo && jogador->ativo) {
        BOOL success = ReadFile(jogador->hPipe, &mensagem, sizeof(DADOS), NULL, &ovRead);
        if (!success) {
            DWORD err = GetLastError();
            if (err == ERROR_IO_PENDING) {
                WaitForSingleObject(ovRead.hEvent, INFINITE);
                success = GetOverlappedResult(jogador->hPipe, &ovRead, &lidos, FALSE);
                if (!success) {
                    err = GetLastError();
                    //_tprintf_s(_T("[ÁRBITRO] GetOverlappedResult falhou (err=%d)\n"), err);
                    if (err == ERROR_BROKEN_PIPE || err == ERROR_NO_DATA) {
                        _tprintf(_T("[ÁRBITRO] %s desconectado\n"), jogador->username);
                        break;
                    }
                    continue;
                }
            }
            else {
                if (err == ERROR_BROKEN_PIPE || err == ERROR_NO_DATA) {
                    _tprintf(_T("[ÁRBITRO] %s desconectado\n"), jogador->username);
                    break;
                }
                continue;
            }
        }
        else {
            // Em modo overlapped, lidos não é preenchido aqui, só no GetOverlappedResult
            continue;
        }

        // Aqui, success == TRUE e lidos contém o número de bytes lidos
        if (lidos != sizeof(DADOS)) {
            _tprintf(_T("[ÁRBITRO] Mensagem incompleta recebida (%lu bytes)\n"), lidos);
            continue;
        }

        // Processar mensagem recebida
        if (_tcscmp(mensagem.comando, _T(":msg")) == 0) {
            _tprintf_s(_T("[JOGADA] %s: %s\n"), jogador->username, mensagem.msg);
            VerificarPalavra(estado, mensagem.msg, jogador->username);
        }
        else if (_tcscmp(mensagem.comando, _T(":sair")) == 0) {
            _tprintf_s(_T("[ÁRBITRO] %s solicitou sair\n"), jogador->username);
            jogador->continua = FALSE;
            break;
        }
        else if (_tcscmp(mensagem.comando, _T(":pont")) == 0) {
            DADOS resposta = { 0 };
            _tcscpy_s(resposta.comando, TAM_BUFFER, _T(":pont"));
            _stprintf_s(resposta.msg, TAM_BUFFER, _T("Pontuação de %s: %d"),
                jogador->username, jogador->pontuacao);

            OVERLAPPED ovWrite = { 0 };
            ovWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            WriteFile(jogador->hPipe, &resposta, sizeof(DADOS), NULL, &ovWrite);
            if (GetLastError() == ERROR_IO_PENDING) {
                WaitForSingleObject(ovWrite.hEvent, INFINITE);
            }
            CloseHandle(ovWrite.hEvent);
        }
        else if (_tcscmp(mensagem.comando, _T(":jogs")) == 0) {
            DADOS resposta = { 0 };
            _tcscpy_s(resposta.comando, TAM_BUFFER, _T(":jogs"));
            TCHAR msg[TAM_BUFFER] = { 0 };

            WaitForSingleObject(estado->hMutexJogadores, INFINITE);
            for (int i = 0; i < estado->num_jogadores; i++) {
                if (estado->jogadores[i].ativo) {
                    _stprintf_s(msg + _tcslen(msg), TAM_BUFFER - _tcslen(msg),
                        _T("%s (%d pontos)\n"),
                        estado->jogadores[i].username,
                        estado->jogadores[i].pontuacao);
                }
            }
            ReleaseMutex(estado->hMutexJogadores);

            _tcscpy_s(resposta.msg, TAM_BUFFER, msg);

            OVERLAPPED ovWrite = { 0 };
            ovWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            WriteFile(jogador->hPipe, &resposta, sizeof(DADOS), NULL, &ovWrite);
            if (GetLastError() == ERROR_IO_PENDING) {
                WaitForSingleObject(ovWrite.hEvent, INFINITE);
            }
            CloseHandle(ovWrite.hEvent);
        }

        ResetEvent(ovRead.hEvent); // Preparar para próxima leitura
    }
    CloseHandle(ovRead.hEvent);

    WaitForSingleObject(estado->hMutexJogadores, INFINITE);
    jogador->ativo = FALSE;
    jogador->continua = FALSE;
    int ativos = JogadoresAtivos(estado);
    if (ativos < 2) {
        ResetEvent(estado->hEventoJogadores);
    }
    else {
        SetEvent(estado->hEventoJogadores);
    }
    ReleaseMutex(estado->hMutexJogadores);
    if (jogador->hPipe != INVALID_HANDLE_VALUE) {
        CloseHandle(jogador->hPipe);
        jogador->hPipe = INVALID_HANDLE_VALUE;
    }

    return 0;
}



BOOL VerificarPalavra(EstadoJogo* estado, const TCHAR* palavra, const TCHAR* username) {
    // Verificar se a palavra está no dicionário
    WaitForSingleObject(estado->hMutexJogadores, INFINITE);

    // Verificar se o jogador ainda está ativo
    BOOL jogadorAtivo = FALSE;
    for (int i = 0; i < estado->num_jogadores; i++) {
        if (_tcscmp(estado->jogadores[i].username, username) == 0 && estado->jogadores[i].ativo) {
            jogadorAtivo = TRUE;
            break;
        }
    }

    ReleaseMutex(estado->hMutexJogadores);

    if (!jogadorAtivo) {
        _tprintf(_T("[ÁRBITRO] Jogador %s não está mais ativo\n"), username);
        return FALSE;
    }
    BOOL palavraValida = FALSE;
    for (int i = 0; i < estado->num_palavras; i++) {
        if (_tcsicmp(palavra, estado->dicionario[i]) == 0) {
            palavraValida = TRUE;
            break;
        }
    }

    if (!palavraValida) {
        HANDLE hPipeRemetente = NULL;
        for (int i = 0; i < estado->num_jogadores; i++) {
            if (_tcscmp(estado->jogadores[i].username, username) == 0) {
                hPipeRemetente = estado->jogadores[i].hPipe;
                break;
            }
        }
        BroadcastMensagem(estado, _T(":msg"), _T("Palavra inválida!"), hPipeRemetente);
        return FALSE;
    }

    TCHAR palavraUpper[MAX_TAMANHO_PALAVRA];
    _tcscpy_s(palavraUpper, MAX_TAMANHO_PALAVRA, palavra);
    for (int i = 0; palavraUpper[i]; i++) {
        palavraUpper[i] = _totupper(palavraUpper[i]);
    }
    // Verificar letras disponíveis (sem reutilização)
    WaitForSingleObject(estado->hMutexSM, INFINITE);

    BOOL letrasDisponiveis = TRUE;
    int lenPalavra = (int)_tcslen(palavraUpper);
    int posUsadas[MAXLETRAS_LIMITE] = { 0 };
    int usadas = 0;

    for (int i = 0; i < lenPalavra; i++) {
        BOOL letraEncontrada = FALSE;
        TCHAR letra = palavraUpper[i];
        for (int j = 0; j < estado->pMem->maxLetras; j++) {
            // Verifica se a letra está disponível e ainda não foi usada nesta jogada
            BOOL jaUsada = FALSE;
            for (int k = 0; k < usadas; k++) {
                if (posUsadas[k] == j) {
                    jaUsada = TRUE;
                    break;
                }
            }
            if (!jaUsada && estado->pMem->letras[j] == letra) {
                posUsadas[usadas++] = j;
                letraEncontrada = TRUE;
                break;
            }
        }
        if (!letraEncontrada) {
            letrasDisponiveis = FALSE;
            break;
        }
    }

    if (letrasDisponiveis) {
        // Atualizar pontuação e letras
        int pontos = lenPalavra;
        for (int i = 0; i < estado->num_jogadores; i++) {
            if (_tcscmp(estado->jogadores[i].username, username) == 0) {
                estado->jogadores[i].pontuacao += pontos;
                break;
            }
        }

        // Remover letras usadas
        for (int i = 0; i < usadas; i++) {
            int pos = posUsadas[i];
            estado->pMem->letras[pos] = _T('_');
            estado->pMem->tempo[pos] = 0;
        }

        // Notificar acerto
        TCHAR msg[TAM_BUFFER];
        _stprintf_s(msg, TAM_BUFFER, _T("%s acertou a palavra '%s' (+%d pontos)"), username, palavra, pontos);
        BroadcastMensagem(estado, _T(":msg"), msg, NULL);
    }
    else {
        // Penalizar por palavra inválida
        for (int i = 0; i < estado->num_jogadores; i++) {
            if (_tcscmp(estado->jogadores[i].username, username) == 0) {
                estado->jogadores[i].pontuacao -= (int)(lenPalavra / 2);
                break;
            }
        }

        TCHAR msg[TAM_BUFFER];
        _stprintf_s(msg, TAM_BUFFER, _T("%s errou a palavra '%s'"), username, palavra);
        BroadcastMensagem(estado, _T(":msg"), msg, NULL);
    }

    SetEvent(estado->hEventoLetras);
    ReleaseMutex(estado->hMutexSM);
    return letrasDisponiveis;
}



DWORD WINAPI ThreadAdmin(LPVOID data) {
    EstadoJogo* estado = (EstadoJogo*)data;
    TCHAR comando[BUFFER_SIZE];
    TCHAR parametro[BUFFER_SIZE];
    TCHAR input[BUFFER_SIZE];

    while (estado->jogoAtivo) {
        _tprintf(_T("\n[ADMIN] Comandos disponíveis:\n"));
        _tprintf(_T("  listar - Listar jogadores conectados\n"));
        _tprintf(_T("  excluir <nome> - Remover jogador\n"));
        _tprintf(_T("  iniciarbot <nome> - Adicionar bot\n"));
        _tprintf(_T("  acelerar - Aumentar ritmo do jogo\n"));
        _tprintf(_T("  travar - Diminuir ritmo do jogo\n"));
        _tprintf(_T("  encerrar - Terminar o jogo\n"));
        _tprintf(_T("\n>> "));

        _fgetts(input, BUFFER_SIZE, stdin);
        input[_tcslen(input) - 1] = _T('\0'); // Remove newline

        // Parse do comando
        int args = _stscanf_s(input, _T("%s %s"), comando, (unsigned)_countof(comando),
            parametro, (unsigned)_countof(parametro));


        if (_tcscmp(comando, _T("listar")) == 0) {
            WaitForSingleObject(estado->hMutexJogadores, INFINITE);

            if (estado->num_jogadores == 0) {
                _tprintf_s(_T("[ADMIN] Nenhum jogador conectado.\n"));
            }
            else {
                _tprintf_s(_T("[ADMIN] Jogadores conectados:\n"));
                for (int i = 0; i < estado->num_jogadores; i++) {
                    if (estado->jogadores[i].continua && estado->jogadores[i].ativo) {
                        // Verifica se já foi listado (proteção extra contra duplicados)
                        BOOL duplicado = FALSE;
                        for (int j = 0; j < i; j++) {
                            if (_tcsicmp(estado->jogadores[j].username, estado->jogadores[i].username) == 0 &&
                                estado->jogadores[j].continua && estado->jogadores[j].ativo) {
                                duplicado = TRUE;
                                break;
                            }
                        }
                        if (!duplicado) {
                            _tprintf(_T("  %s (%s) - %d pontos\n"),
                                estado->jogadores[i].username,
                                estado->jogadores[i].is_bot ? _T("BOT") : _T("Jogador"),
                                estado->jogadores[i].pontuacao);
                        }
                    }
                }
            }

            ReleaseMutex(estado->hMutexJogadores);
        }

        else if (_tcscmp(comando, _T("excluir")) == 0) {
            if (args < 2) {
                _tprintf_s(_T("[ERRO] Uso: excluir <nome>\n"));
                continue;
            }

            WaitForSingleObject(estado->hMutexJogadores, INFINITE);

            BOOL encontrou = FALSE;
            for (int i = 0; i < estado->num_jogadores; i++) {
                // Adicione o campo ativo em Jogador!
                if (_tcscmp(estado->jogadores[i].username, parametro) == 0 && estado->jogadores[i].continua) {
                    BroadcastMensagem(estado, _T(":sair"), _T("Removido pelo administrador"), estado->jogadores[i].hPipe);
                    //CloseHandle(estado->jogadores[i].hPipe);
                    estado->jogadores[i].continua = FALSE;
                    estado->jogadores[i].ativo = FALSE;
                    int ativos = JogadoresAtivos(estado);
                    if (ativos < 2) {
                        ResetEvent(estado->hEventoJogadores);
                    }
                    else {
                        SetEvent(estado->hEventoJogadores);
                    }
                    _tprintf_s(_T("[ADMIN] Jogador %s removido.\n"), parametro);
                    encontrou = TRUE;
                    break;

                }
            }

            if (!encontrou) {
                _tprintf(_T("[ADMIN] Jogador %s não encontrado.\n"), parametro);
            }

            ReleaseMutex(estado->hMutexJogadores);
        }

        else if (_tcscmp(comando, _T("iniciarbot")) == 0) {
            if (args < 2) {
                _tprintf(_T("[ERRO] Uso: iniciarbot <nome>\n"));
                continue;
            }

            STARTUPINFO si = { sizeof(si) };
            PROCESS_INFORMATION pi;
            TCHAR cmdLine[256];

            _stprintf_s(cmdLine, _countof(cmdLine), _T("bot.exe %s"), parametro);

            if (CreateProcess(NULL, cmdLine, NULL, NULL, FALSE,
                CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
                _tprintf(_T("[ADMIN] Bot %s iniciado.\n"), parametro);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            else {
                _tprintf(_T("[ERRO] Falha ao iniciar bot (Erro: %d)\n"), GetLastError());
            }
        }
        else if (_tcscmp(comando, _T("acelerar")) == 0) {
            WaitForSingleObject(estado->hMutexSM, INFINITE);

            if (estado->pMem->ritmo > 1) {
                estado->pMem->ritmo--;
                _tprintf(_T("[ADMIN] Ritmo aumentado para %d segundos.\n"), estado->pMem->ritmo);

                // Notificar jogadores
                TCHAR msg[32];
                _stprintf_s(msg, _countof(msg), _T("Ritmo aumentado: %ds"), estado->pMem->ritmo);
                BroadcastMensagem(estado, _T(":msg"), msg, NULL);
            }
            else {
                _tprintf(_T("[ADMIN] Ritmo já está no mínimo (1s).\n"));
            }

            ReleaseMutex(estado->hMutexSM);
        }
        else if (_tcscmp(comando, _T("travar")) == 0) {
            WaitForSingleObject(estado->hMutexSM, INFINITE);

            if (estado->pMem->ritmo < 10) {
                estado->pMem->ritmo++;
                _tprintf(_T("[ADMIN] Ritmo diminuído para %d segundos.\n"), estado->pMem->ritmo);

                // Notificar jogadores
                TCHAR msg[32];
                _stprintf_s(msg, _countof(msg), _T("Ritmo diminuído: %ds"), estado->pMem->ritmo);
                BroadcastMensagem(estado, _T(":msg"), msg, NULL);
            }
            else {
                _tprintf(_T("[ADMIN] Ritmo já está no máximo (10s).\n"));
            }

            ReleaseMutex(estado->hMutexSM);
        }
        else if (_tcscmp(comando, _T("encerrar")) == 0) {
            _tprintf(_T("[ADMIN] Encerrando jogo...\n"));

            AnunciarVencedor(estado);
            // Notificar todos os jogadores
            BroadcastMensagem(estado, _T(":sair"), _T("Jogo encerrado pelo administrador"), NULL);

            // Encerrar o jogo
            estado->jogoAtivo = FALSE;
            WaitForSingleObject(estado->hMutexJogadores, INFINITE);
            for (int i = 0; i < estado->num_jogadores; i++) {
                if (estado->jogadores[i].hPipe != INVALID_HANDLE_VALUE) {
                    CloseHandle(estado->jogadores[i].hPipe);
                    estado->jogadores[i].hPipe = INVALID_HANDLE_VALUE;
                }
                estado->jogadores[i].ativo = FALSE;
                estado->jogadores[i].continua = FALSE;
            }
            ReleaseMutex(estado->hMutexJogadores);
            HANDLE hFake = CreateFile(
                PIPE_NAME,
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL
            );
            if (hFake != INVALID_HANDLE_VALUE) {
                CloseHandle(hFake);
            }
            break;
        }
        else {
            _tprintf(_T("[ERRO] Comando desconhecido: %s\n"), comando);
        }
    }

    _tprintf(_T("[ADMIN] Thread de administração encerrada.\n"));
    return 0;
}



int _tmain() {
#ifdef UNICODE
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
#endif

    EstadoJogo estado = { 0 };
    estado.jogoAtivo = TRUE;
    estado.hMutexJogadores = CreateMutex(NULL, FALSE, NULL);
    if (estado.hMutexJogadores == NULL) {
        _tprintf(_T("[ERRO] Falha ao criar mutex para jogadores\n"));
        return 1;
    }
    estado.hEventoJogadores = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (estado.hEventoJogadores == NULL) {
        _tprintf(_T("[ERRO] Falha ao criar evento para jogadores\n"));
        CloseHandle(estado.hMutexJogadores);
        return 1;
    }

    // Inicialização do sistema
    if (!CarregarConfiguracoes(&estado)) {
        _tprintf(_T("[ERRO] Falha ao carregar configurações\n"));
        return 1;
    }

    if (!CarregarDicionario(&estado)) {
        _tprintf(_T("[ERRO] Falha ao carregar dicionário\n"));
        return 1;
    }

    if (!InicializarSharedMemory(&estado)) {
        _tprintf(_T("[ERRO] Falha ao inicializar shared memory\n"));
        return 1;
    }

    // Iniciar thread de administração
    HANDLE hThreadAdmin = CreateThread(NULL, 0, ThreadAdmin, &estado, 0, NULL);

    // Iniciar thread de gerenciamento de letras
    HANDLE hThreadLetras = CreateThread(NULL, 0, ThreadLetras, &estado, 0, NULL);

    _tprintf(_T("[ÁRBITRO] Sistema inicializado. Aguardando jogadores...\n"));

    // Loop principal para aceitar conexões
    while (estado.jogoAtivo) {
        HANDLE hPipe = CreateNamedPipe(
            PIPE_NAME,
            PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            MAX_JOGADORES,
            sizeof(DADOS),
            sizeof(DADOS),
            TEMPO_ESPERA_CONEXAO,
            NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            _tprintf(_T("[ERRO] Erro ao criar pipe: %d\n"), GetLastError());
            Sleep(1000);
            continue;
        }

        // Aguardar conexão
        BOOL conectado = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

        if (conectado) {
            DADOS dadosLogin = { 0 };
            DWORD lidos = 0;
            OVERLAPPED ov = { 0 };
            ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            BOOL loginOk = ReadFile(hPipe, &dadosLogin, sizeof(DADOS), &lidos, &ov);
            if (!loginOk && GetLastError() == ERROR_IO_PENDING) {
                WaitForSingleObject(ov.hEvent, INFINITE);
                GetOverlappedResult(hPipe, &ov, &lidos, FALSE);
            }
            CloseHandle(ov.hEvent);

            // Protege toda a verificação e adição de jogador
            WaitForSingleObject(estado.hMutexJogadores, INFINITE);

            if (!NomeValido(&estado, dadosLogin.username)) {
                _tprintf_s(_T("[ÁRBITRO] Nome inválido ou duplicado: %s\n"), dadosLogin.username);
                DADOS resposta = { 0 };
                _tcscpy_s(resposta.comando, TAM_BUFFER, _T(":sair"));
                _tcscpy_s(resposta.msg, TAM_BUFFER, _T("Nome inválido ou já em uso"));
                DWORD escritos;
                WriteFile(hPipe, &resposta, sizeof(DADOS), &escritos, NULL);
                DisconnectNamedPipe(hPipe);
                CloseHandle(hPipe);
                ReleaseMutex(estado.hMutexJogadores);
                _tprintf(_T("[ÁRBITRO] Jogador %s não pôde se conectar (nome inválido)\n"), dadosLogin.username);
                continue;
            }

            if (estado.num_jogadores >= MAX_JOGADORES) {
                _tprintf_s(_T("[ÁRBITRO] Limite de jogadores atingido\n"));
                DADOS resposta = { 0 };
                _tcscpy_s(resposta.comando, TAM_BUFFER, _T(":sair"));
                _tcscpy_s(resposta.msg, TAM_BUFFER, _T("Limite de jogadores atingido"));
                DWORD escritos;
                WriteFile(hPipe, &resposta, sizeof(DADOS), &escritos, NULL);
                DisconnectNamedPipe(hPipe);
                CloseHandle(hPipe);
                ReleaseMutex(estado.hMutexJogadores);
                _tprintf(_T("[ÁRBITRO] Jogador %s não pôde se conectar (limite atingido)\n"), dadosLogin.username);
                continue;
            }

            // Adicionar novo jogador
            Jogador* novoJogador = &estado.jogadores[estado.num_jogadores];
            novoJogador->hPipe = hPipe;
            novoJogador->continua = TRUE;
            novoJogador->ativo = TRUE;
            novoJogador->pontuacao = 0;
            novoJogador->is_bot = (_tcsstr(dadosLogin.username, _T("bot")) != NULL);
            novoJogador->estado = &estado;
            _tcscpy_s(novoJogador->username, MAX_USERNAME, dadosLogin.username);

            _tprintf(_T("[ÁRBITRO] Jogador conectado: %s\n"), novoJogador->username);

            // Criar thread para o jogador
            HANDLE hThread = CreateThread(NULL, 0, TratarJogador, novoJogador, 0, NULL);
            if (hThread == NULL) {
                _tprintf(_T("[ERRO] Falha ao criar thread para jogador: %d\n"), GetLastError());
                CloseHandle(hPipe);
                ReleaseMutex(estado.hMutexJogadores);
                continue;
            }

            estado.num_jogadores++;
            int ativos = JogadoresAtivos(&estado);
            if (ativos >= 2) {
                SetEvent(estado.hEventoJogadores);
            }
            else {
                ResetEvent(estado.hEventoJogadores);
            }
            CloseHandle(hThread); // Não precisamos do handle

            ReleaseMutex(estado.hMutexJogadores);
        }
        else {
            CloseHandle(hPipe);
        }
    }


    // Limpeza
    WaitForSingleObject(hThreadAdmin, INFINITE);
    WaitForSingleObject(hThreadLetras, INFINITE);

    CloseHandle(hThreadAdmin);
    CloseHandle(hThreadLetras);
    CloseHandle(estado.hMutexJogadores);
    CloseHandle(estado.hMutexSM);
    CloseHandle(estado.hEventoLetras);
    CloseHandle(estado.hEventoJogadores);
    UnmapViewOfFile(estado.pMem);
    CloseHandle(estado.hMap);

    _tprintf(_T("[ÁRBITRO] Sistema encerrado\n"));
    return 0;
}


