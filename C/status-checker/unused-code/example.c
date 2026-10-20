/* Example on SFTP thing. Leaving it here cuz I'll see it. */

#include <libssh2.h>
#include <libssh2_sftp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    unsigned long hostaddr;
    int sock, auth_pw = 0;
    struct sockaddr_in sin;
    const char *fingerprint;
    int rc;
    LIBSSH2_SESSION *session;
    LIBSSH2_SFTP *sftp_session;
    LIBSSH2_SFTP_HANDLE *sftp_handle;
    
    // Server details
    const char *server_ip = "192.168.1.50";
    int server_port = 22;
    const char *username = "your_username";
    const char *password = "your_password";
    const char *local_file = "local.txt";
    const char *remote_file = "/remote/path/remote.txt";

    // 1. Initialize Winsock (if on Windows) or just libssh2
    rc = libssh2_init(0);
    if (rc != 0) {
        fprintf(stderr, "Initialization failed (%d)\n", rc);
        return 1;
    }

    // 2. Connect to the SSH server via standard TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(server_port);
    sin.sin_addr.s_addr = inet_addr(server_ip);
    
    if (connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "Failed to connect to %s:%d\n", server_ip, server_port);
        goto shutdown;
    }

    // 3. Create a session instance and start the SSH handshake
    session = libssh2_session_init();
    if (!session) return 1;

    if (libssh2_session_handshake(session, sock)) {
        fprintf(stderr, "Error establishing SSH session\n");
        goto shutdown;
    }

    // 4. Authenticate using username and password
    if (libssh2_userauth_password(session, username, password)) {
        fprintf(stderr, "Authentication by password failed.\n");
        goto shutdown;
    }

    // 5. Initialize SFTP session
    sftp_session = libssh2_sftp_init(session);
    if (!sftp_session) {
        fprintf(stderr, "Unable to init SFTP session\n");
        goto shutdown;
    }

    // 6. Open remote file for writing
    sftp_handle = libssh2_sftp_open(sftp_session, remote_file, 
                                    LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC, 
                                    LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_IWUSR|
                                    LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_IROTH);
    if (!sftp_handle) {
        fprintf(stderr, "Unable to open remote file with SFTP\n");
        goto shutdown;
    }

    // 7. Read local file and write to remote SFTP handle
    FILE *local = fopen(local_file, "rb");
    if (!local) {
        fprintf(stderr, "Can't open local file\n");
        goto shutdown;
    }

    char mem[1024];
    size_t nread;
    while ((nread = fread(mem, 1, sizeof(mem), local)) > 0) {
        char *ptr = mem;
        ssize_t nw;
        do {
            nw = libssh2_sftp_write(sftp_handle, ptr, nread);
            if (nw < 0) break;
            ptr += nw;
            nread -= nw;
        } while (nread > 0);
    }

    fclose(local);
    libssh2_sftp_closedir(sftp_handle);
    libssh2_sftp_shutdown(sftp_session);

    printf("File transfer completed successfully!\n");

shutdown:
    libssh2_session_disconnect(session, "Normal Shutdown");
    libssh2_session_free(session);
    close(sock);
    libssh2_exit();
    return 0;
}