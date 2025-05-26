// StellarEcho.c
// Laboratorio de Buffer Overflow 32-bit para Windows
// Tema: “StellarEcho Service”
// Escucha en el puerto 9999 y copia sin límite al búfer local.

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")
// Deshabilita NX para permitir stacking shellcode (DEP off)
// (requiere enlazar con /NXCOMPAT:NO en MSVC o --nxcompat en MinGW)
#pragma comment(linker, "/NXCOMPAT:NO")

#define LISTEN_PORT 9999
#define REQ_BUFSIZE 512
#define SMBUFSIZE   256

void handle_client(SOCKET client) {
    char recvbuf[REQ_BUFSIZE];
    char smallbuf[SMBUFSIZE];

    // Recibe hasta 512 bytes de datos
    int len = recv(client, recvbuf, REQ_BUFSIZE, 0);
    if (len <= 0) {
        closesocket(client);
        return;
    }
    // Asegurar cadena
    recvbuf[(len < REQ_BUFSIZE) ? len : REQ_BUFSIZE-1] = '\0';

    // VULNERABILIDAD: copia sin comprobar tamaño (overflow si len ≥ 256)
    strcpy(smallbuf, recvbuf);

    // Respuesta sencilla
    const char *resp = "StellarEcho: recibido\n";
    send(client, resp, (int)strlen(resp), 0);

    closesocket(client);
}

int main() {
    WSADATA wsa;
    SOCKET listen_sock, client;
    struct sockaddr_in addr;

    // 1) Inicializar Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup fallo\n");
        return 1;
    }

    // 2) Crear socket TCP
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        printf("socket fallo\n");
        WSACleanup();
        return 1;
    }

    // 3) Bind en 0.0.0.0:9999
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(LISTEN_PORT);
    if (bind(listen_sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("bind fallo\n");
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }

    // 4) Escuchar y aceptar conexiones
    listen(listen_sock, 5);
    printf("StellarEcho escuchando en el puerto %d...\n", LISTEN_PORT);

    while (1) {
        client = accept(listen_sock, NULL, NULL);
        if (client != INVALID_SOCKET) {
            // Cada cliente en el mismo hilo, para simplicidad
            handle_client(client);
        }
    }

    // 5) Limpieza (nunca se llega aquí)
    closesocket(listen_sock);
    WSACleanup();
    return 0;
}
