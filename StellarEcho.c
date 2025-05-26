// StellarEcho.c
// Laboratorio de Buffer Overflow 32-bit para Windows
// Tema: “StellarEcho Service” – un servicio de comunicaciones orbitales

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>

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

    printf("🔭 [StellarEcho] Señal entrante: procesando datos de la estación terrestre...\n");
    
    // Recibe hasta 512 bytes de datos
    int len = recv(client, recvbuf, REQ_BUFSIZE, 0);
    if (len <= 0) {
        printf("❌ [StellarEcho] Conexión perdida o error al recibir. Cerrando enlace.\n");
        closesocket(client);
        return;
    }

    // Asegurar cadena
    recvbuf[(len < REQ_BUFSIZE) ? len : REQ_BUFSIZE-1] = '\0';
    printf("📡 [StellarEcho] Datos recibidos (%d bytes): %s\n", len, recvbuf);

    // VULNERABILIDAD: copia sin comprobar tamaño (overflow si len ≥ 256)
    strcpy(smallbuf, recvbuf);

    // Respuesta sencilla
    const char *resp = "🛰️ StellarEcho: eco orbital enviado\n";
    send(client, resp, (int)strlen(resp), 0);
    
    printf("✅ [StellarEcho] Eco enviado de vuelta a la estación terrestre.\n");
    closesocket(client);
}

int main() {
    WSADATA wsa;
    SOCKET listen_sock, client;
    struct sockaddr_in addr;

    printf("🌌 Iniciando StellarEcho Orbital Service...\n");

    // 1) Inicializar Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("❌ [StellarEcho] WSAStartup fallido.\n");
        return 1;
    }

    // 2) Crear socket TCP
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        printf("❌ [StellarEcho] Creación de socket fallida.\n");
        WSACleanup();
        return 1;
    }

    // 3) Bind en 0.0.0.0:9999
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(LISTEN_PORT);
    if (bind(listen_sock, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        printf("❌ [StellarEcho] Fallo en bind. No se pudo en órbita.\n");
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }

    // 4) Escuchar y aceptar conexiones
    if (listen(listen_sock, 5) == SOCKET_ERROR) {
        printf("❌ [StellarEcho] Fallo al escuchar en puerto %d.\n", LISTEN_PORT);
        closesocket(listen_sock);
        WSACleanup();
        return 1;
    }
    printf("🛰️ [StellarEcho] En órbita y escuchando en el puerto %d...\n", LISTEN_PORT);

    while (1) {
        client = accept(listen_sock, NULL, NULL);
        if (client != INVALID_SOCKET) {
            printf("📶 [StellarEcho] Enlace con estación terrestre establecido.\n");
            handle_client(client);
        }
    }

    // 5) Limpieza (nunca se llega aquí)
    closesocket(listen_sock);
    WSACleanup();
    return 0;
}
