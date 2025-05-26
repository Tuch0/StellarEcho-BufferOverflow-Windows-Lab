# **StellarEcho: Laboratorio de Buffer Overflow 32‑bits para Windows**

---

## 🚀 ¿Qué es StellarEcho?

StellarEcho es un servicio de comunicaciones inspirado en sistemas satelitales y naves espaciales ficticias. Imagina una estación orbital que responde a comandos e informa del estado de sus sensores mediante un protocolo sencillo de eco (echo). Este laboratorio te propone un reto de **seguridad**: StellarEcho contiene una vulnerabilidad clásica de **desbordamiento de búfer** en un entorno de **Windows 32‑bits**.

**Objetivo:** Practicar la explotación de un buffer overflow en un servicio realista, que al desbordarse abre la puerta a la ejecución de código arbitrario. En concreto, StellarEcho escucha en el puerto **9999**, copia hasta 512 bytes sin verificar a un búfer de solo 256, y permite controlar el flujo de ejecución.

---

## 🔧 Instalación y compilación

### Requisitos previos

* Windows 32‑bits (o máquina virtual con Windows x86).
* **DEP** y **ASLR** desactivados para facilitar la explotación (opcional, pero recomendable).
* Herramientas de compilación:

  * **MinGW** (msys2/x86).
  * **Visual Studio** (Developer Command Prompt).

### Compilar con MinGW

```bash
# Abrir MSYS2 MinGW 32‑bit
gcc -m32 StellarEcho.c -o StellarEcho.exe \
    -lws2_32 \
    -Wl,--nxcompat \
    -fno-stack-protector
```

### Compilar con Visual Studio

```bat
cl /nologo /W3 /GX StellarEcho.c ws2_32.lib /link /NXCOMPAT:NO /GS-
```

* `/GS-` desactiva el *stack cookie*.
* `/NXCOMPAT:NO` marca el binario sin *DEP*, permitiendo memoria de pila ejecutable.

---

## ▶️ Ejecución y uso

1. Inicia **StellarEcho.exe** en tu VM Windows.
2. Desde tu máquina atacante (o desde la misma VM) conecta con netcat:

   ```bash
   nc <IP-víctima> 9999
   ```
3. Envía una cadena mayor de **256 bytes**. StellarEcho copiará esos datos en un búfer de solo 256, desencadenando un **buffer overflow**.
4. Si preparas un *payload* adecuado (por ejemplo, un shellcode o un return-to-libc), podrás controlar la instrucción **EIP** y ejecutar código arbitrario.

> 🛡️ **Nota:** Este servicio está diseñado **solo** con fines educativos. No lo uses en entornos de producción ni contra sistemas sin permiso.

---

## 🔍 ¿Qué simula StellarEcho?

StellarEcho emula el software de comunicaciones de una sonda espacial de exploración profunda. En misiones reales, satélites o sondas repiten comandos y envían telemetría básica de estado. Con este laboratorio, reproducimos:

* **Conexión TCP** a un puerto dedicado (9999).
* **Manejo de comandos**: eco de texto recibido.
* **Desbordamiento controlado**: copia insegura de datos en un búfer de tamaño limitado.

La idea es recrear cómo un fallo en un módulo de telemetría o de recepción de comandos, si no valida correctamente la longitud de los mensajes, puede comprometer toda la misión.

---

## 📖 Explicación del código vulnerable

```c
void handle_client(SOCKET client) {
    char recvbuf[512];     // búfer grande para recibir datos
    char smallbuf[256];    // búfer vulnerable — solo 256 bytes

    int len = recv(client, recvbuf, 512, 0);
    if (len <= 0) return;
    recvbuf[(len < 512 ? len : 511)] = '\0';

    // VULNERABILIDAD: copia sin límite
    strcpy(smallbuf, recvbuf);
    send(client, "StellarEcho: recibido\n", 19, 0);
}
```

1. **Buffers desiguales**: `recvbuf` acepta hasta 512 bytes, pero `smallbuf` solo 256.
2. **Uso de `strcpy`**: no comprueba tamaño de origen, por lo que si recibes >256 bytes, se sobrescribe el espacio de pila adyacente.
3. **Sobrescritura de pila**: la memoria de retorno (`EIP`) puede corromperse, redirigiendo la ejecución.

### ¿Por qué es vulnerable?

* Lenguaje C/C++ sin comprobaciones de límites en funciones clásicas.
* Protección mínima desactivada (`/GS-`, DEP off) para practicar explotaciones de pila.
* Arquitectura de **32‑bits**: direcciones de 4 bytes, salto al shellcode en pila más sencillo de calcular.

---

## 📂 Estructura del repositorio

```
StellarEcho-Lab/
├── StellarEcho.c      # Código fuente del servicio
├── exploit.py        # Ejemplo de exploit con pwntools
├── README.md         # Este documento
└── docs/
    └── stack_layout.png  # Diagrama de la pila y offsets
```

---

¡Bienvenido a la misión **StellarEcho**! Prepárate para investigar la seguridad de sistemas espaciales y dominar el arte del *buffer overflow* en un entorno Windows 32‑bits. 🛰️✨
