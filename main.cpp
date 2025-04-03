#include <iostream>
#include <thread>
#include <cstring>
#include <arpa/inet.h>
#include <fstream>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <termios.h>
#include <unistd.h>
#include <queue>
#include <condition_variable>

#include <atomic>
#include <regex>
#include <sys/stat.h>
#include <fcntl.h>
#include <random>
#include "EVP_CIPHER_CTX.h"
#include "EVP_EncryptInit_ex.h"
#include "EVP_DecryptInit_ex.h"

using namespace std;

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_FILE_SIZE 104857600 // 100MB
#define COLOR_MSG    "\033[1;34m"
#define COLOR_RECV   "\033[1;32m"
#define COLOR_ERROR  "\033[1;31m"
#define COLOR_WARN   "\033[1;33m"
#define COLOR_RESET  "\033[0m"
#define SALT_SIZE 32
#define AES_KEY_SIZE 32
#define IV_SIZE 16

static const string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

// Thread-safe message queue
template<typename T>
class MessageQueue {
private:
    queue<T> queue_;
    mutex mutex_;
    condition_variable cond_;
public:
    void push(T item) {
        lock_guard<mutex> lock(mutex_);
        queue_.push(move(item));
        cond_.notify_one();
    }

    T pop() {
        unique_lock<mutex> lock(mutex_);
        cond_.wait(lock, [this]{ return !queue_.empty(); });
        T item = move(queue_.front());
        queue_.pop();
        return item;
    }

    bool empty() {
        lock_guard<mutex> lock(mutex_);
        return queue_.empty();
    }
};

struct Message {
    int id = 0;
    string timestamp;
    string sender;
    string content;
    string room;
    bool encrypted = false;
    bool is_file = false;
};

struct UserSession {
    string username;
    string password_hash;
    string salt;
    bool authenticated = false;
    int failed_attempts = 0;
    time_t last_attempt = 0;
};

struct FileTransfer {
    string filename;
    string sender;
    string recipient;
    size_t total_size;
    size_t received = 0;
    ofstream file_stream;
    bool in_progress = false;
};

int SHA256_DIGEST_LENGTH;

// Enhanced encryption functions
class CryptoHandler {
private:
    unsigned char aes_key[AES_KEY_SIZE];
    unsigned char iv[IV_SIZE];

    void generate_random_bytes(unsigned char* buffer, size_t length) {

        class random_device rd;
        uniform_int_distribution<int> dist(0, 255);
        for (size_t i = 0; i < length; ++i) {
            buffer[i] = static_cast<unsigned char>(dist(rd));
        }
    }

public:
    static void
    PKCS5_PBKDF2_HMAC_SHA1(const char *str, unsigned long length, const unsigned char *string, unsigned long length1,
                           int i,
                           int i1, unsigned char key[32]) {

    }

    CryptoHandler(const string& password, const string& salt) {
        // Derive key from password using PBKDF2
        PKCS5_PBKDF2_HMAC_SHA1(password.c_str(), password.length(),
                               reinterpret_cast<const unsigned char*>(salt.c_str()), salt.length(),
                               10000, AES_KEY_SIZE, aes_key);

        // Generate random IV
        generate_random_bytes(iv, IV_SIZE);
    }

    string base64_encode(string basicString) {
        return std::string();
    }

    void EVP_aes_256_cbc() {

    }

    string encrypt(const string& plaintext) {
    }

    void EVP_CIPHER_CTX_free(EVP_CIPHER_CTX pCtx) {

    }

    EVP_CIPHER_CTX *EVP_CIPHER_CTX_new() {
        return nullptr;
    }

    string base64_decode(const string &basicString) {
        return std::string();
    }

    void EVP_DecryptUpdate(EVP_CIPHER_CTX *pCtx, unsigned char plaintext[], int *pInt, const unsigned char *string,
                           unsigned long i) {

    }

    void EVP_DecryptFinal_ex(EVP_CIPHER_CTX *pCtx, unsigned char string[], int *pInt) {

    }

    string decrypt(const string& encrypted) {
        string decoded = base64_encode(encrypted);
        if (decoded.length() < IV_SIZE) return "";

        // Extract IV from beginning
        unsigned char iv[IV_SIZE];
        memcpy(iv, decoded.c_str(), IV_SIZE);

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

        int len;
        int plaintext_len;
        const int EVP_MAX_BLOCK_LENGTH = 32;  // Use the correct value here

        unsigned char plaintext[decoded.length() - IV_SIZE + EVP_MAX_BLOCK_LENGTH];

        EVP_DecryptUpdate(ctx, plaintext, &len, reinterpret_cast<const unsigned char*>(decoded.c_str()) + IV_SIZE, decoded.length() - IV_SIZE);
        plaintext_len = len;

        EVP_DecryptFinal_ex(ctx, plaintext + len, &len);
        plaintext_len += len;

        EVP_CIPHER_CTX_free(reinterpret_cast<const EVP_CIPHER_CTX &>(ctx));

        return string(reinterpret_cast<char*>(plaintext), plaintext_len);
    }

    static string generate_salt(size_t length = SALT_SIZE) {
        random_device rd;
        uniform_int_distribution<int> dist(0, 255);
        string salt;
        salt.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            salt += static_cast<char>(dist(rd));
        }
        return salt;
    }

    static string hash_password(const string& password, const string& salt) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        PKCS5_PBKDF2_HMAC_SHA1(password.c_str(), password.length(),
                               reinterpret_cast<const unsigned char*>(salt.c_str()), salt.length(),
                               10000, SHA256_DIGEST_LENGTH, hash);
        return string(reinterpret_cast<char*>(hash), SHA256_DIGEST_LENGTH);
    }
};

void EVP_cleanup();

void ERR_load_crypto_strings();

void ERR_free_strings();

void OpenSSL_add_all_algorithms();

string base64_encode(const string &in) {
    string out;
    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) {
        out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    }
    while (out.size() % 4) {
        out.push_back('=');
    }
    return out;
}

string base64_decode(const string &in) {
    string out;
    vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;

    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

vector<int> clientSockets;
unordered_map<string, UserSession> userSessions;
unordered_map<string, int> usernameToSocket;
unordered_map<string, vector<int>> chatRooms;
unordered_map<string, bool> userStatus;
unordered_map<int, string> clientToRoom;
unordered_map<string, FileTransfer> activeTransfers;
vector<Message> messageHistory;
MessageQueue<string> broadcastQueue;
atomic<bool> serverRunning(true);
atomic<int> messageIdCounter(1);
mutex clientSocketMutex, usernameMutex, chatRoomMutex,
        messageHistoryMutex, userStatusMutex, transferMutex,
        sessionMutex, logMutex;
thread broadcastThread;
thread receiveThread;

string getTimestamp() {
    auto now = chrono::system_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
    time_t now_time = chrono::system_clock::to_time_t(now);
    char buf[80];
    strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&now_time));
    snprintf(buf + strlen(buf), 5, ".%03d", static_cast<int>(ms.count()));
    return string(buf);
}

void logMessage(const string& message, const string& level = "INFO") {
    lock_guard<mutex> lock(logMutex);
    ofstream logFile("chat.log", ios::app);
    if (logFile.is_open()) {
        logFile << "[" << getTimestamp() << "] [" << level << "] " << message << endl;
        logFile.close();
    }
}

void clearScreen() {
    system("clear || cls");
}

void playNotificationSound() {
    system("echo -n '\a'");
}

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

bool validateUsername(const string& username) {
    regex pattern("^[a-zA-Z0-9_]{3,20}$");
    return regex_match(username, pattern);
}

bool validatePassword(const string& password) {
    // At least 8 chars, contains digit, lowercase, uppercase and special char
    regex pattern("^(?=.*[0-9])(?=.*[a-z])(?=.*[A-Z])(?=.*[@#$%^&+=])(?=\\S+$).{8,}$");
    return regex_match(password, pattern);
}

void broadcastWorker() {
    while (serverRunning) {
        string message = broadcastQueue.pop();
        if (!serverRunning) break;

        lock_guard<mutex> lock(clientSocketMutex);
        for (int sock : clientSockets) {
            send(sock, message.c_str(), message.size(), 0);
        }
    }
}

void broadcastToRoom(const string& message, int senderSocket, const string& room) {
    lock_guard<mutex> lock(clientSocketMutex);
    auto& members = chatRooms[room];
    for (int socket : members) {
        if (socket != senderSocket) {
            send(socket, message.c_str(), message.size(), 0);
        }
    }
}

void sendFileChunk(int sock, const string& filename, const string& recipient) {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file) {
        string error = "FILE_ERROR: Could not open file " + filename;
        send(sock, error.c_str(), error.size(), 0);
        return;
    }

    streamsize size = file.tellg();
    if (size > MAX_FILE_SIZE) {
        string error = "FILE_ERROR: File too large (max " + to_string(MAX_FILE_SIZE/1048576) + "MB)";
        send(sock, error.c_str(), error.size(), 0);
        return;
    }

    file.seekg(0, ios::beg);
    const size_t chunk_size = 4096;
    vector<char> buffer(chunk_size);
    size_t total_sent = 0;

    string init_msg = "/file_start " + recipient + " " + filename + " " + to_string(size);
    send(sock, init_msg.c_str(), init_msg.size(), 0);

    while (file.read(buffer.data(), chunk_size) || file.gcount()) {
        string chunk(buffer.data(), file.gcount());
        string encoded = base64_encode(chunk);
        string file_msg = "/file_chunk " + recipient + " " + encoded;
        send(sock, file_msg.c_str(), file_msg.size(), 0);
        total_sent += file.gcount();
    }

    string end_msg = "/file_end " + recipient + " " + filename;
    send(sock, end_msg.c_str(), end_msg.size(), 0);
}

void handleClient(int clientSocket) {
    char authBuffer[BUFFER_SIZE];
    memset(authBuffer, 0, BUFFER_SIZE);
    int authBytes = recv(clientSocket, authBuffer, BUFFER_SIZE, 0);
    if (authBytes <= 0) {
        close(clientSocket);
        return;
    }

    vector<string> authParts = split(string(authBuffer, authBytes), '\x1F');
    if (authParts.size() < 3) {
        close(clientSocket);
        return;
    }

    string username = authParts[0];
    string password = authParts[1];
    string clientSalt = authParts[2];

    // Authentication
    bool authenticated = false;
    string sessionSalt;
    CryptoHandler* crypto = nullptr;

    {
        lock_guard<mutex> lock(sessionMutex);
        if (userSessions.count(username)) {
            UserSession& session = userSessions[username];
            time_t now = time(nullptr);

            // Check for brute force
            if (session.failed_attempts >= 5 && now - session.last_attempt < 300) {
                string response = "AUTH_ERROR: Too many failed attempts. Try again later.";
                send(clientSocket, response.c_str(), response.size(), 0);
                close(clientSocket);
                return;
            }

            string computed_hash = CryptoHandler::hash_password(password, session.salt);
            if (computed_hash == session.password_hash) {
                authenticated = true;
                session.authenticated = true;
                session.failed_attempts = 0;
                sessionSalt = session.salt;
            } else {
                session.failed_attempts++;
                session.last_attempt = now;
            }
        } else {
            // New user registration
            if (!validateUsername(username)) {
                string response = "AUTH_ERROR: Invalid username format";
                send(clientSocket, response.c_str(), response.size(), 0);
                close(clientSocket);
                return;
            }

            if (!validatePassword(password)) {
                string response = "AUTH_ERROR: Password must be at least 8 chars with digit, lowercase, uppercase and special char";
                send(clientSocket, response.c_str(), response.size(), 0);
                close(clientSocket);
                return;
            }

            sessionSalt = CryptoHandler::generate_salt();
            string password_hash = CryptoHandler::hash_password(password, sessionSalt);
            userSessions[username] = {username, password_hash, sessionSalt, true, 0, time(nullptr)};
            authenticated = true;
        }
    }

    if (!authenticated) {
        string response = "AUTH_ERROR: Invalid credentials";
        send(clientSocket, response.c_str(), response.size(), 0);
        close(clientSocket);
        return;
    }

    crypto = new CryptoHandler(password, sessionSalt);
    string authSuccess = "AUTH_SUCCESS";
    send(clientSocket, authSuccess.c_str(), authSuccess.size(), 0);

    {
        lock_guard<mutex> lock(usernameMutex);
        usernameToSocket[username] = clientSocket;
    }
    {
        lock_guard<mutex> lock(userStatusMutex);
        userStatus[username] = true;
    }

    cout << COLOR_RECV << username << " connected!" << COLOR_RESET << endl;
    logMessage("User " + username + " connected", "INFO");

    // Send MOTD
    ifstream motdFile("motd.txt");
    if (motdFile) {
        string motd((istreambuf_iterator<char>(motdFile)), istreambuf_iterator<char>());
        string encrypted = crypto->encrypt("MOTD: " + motd);
        send(clientSocket, encrypted.c_str(), encrypted.size(), 0);
    }

    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) {
            break;
        }

        string decryptedMsg;
        try {
            decryptedMsg = crypto->decrypt(string(buffer, bytesReceived));
            if (decryptedMsg.empty()) {
                throw runtime_error("Decryption failed");
            }
        } catch (...) {
            logMessage("Decryption failed for message from " + username, "ERROR");
            continue;
        }

        if (decryptedMsg.find("/pm ") == 0) {
            vector<string> parts = split(decryptedMsg, ' ');
            if (parts.size() >= 3) {
                string target = parts[1];
                string content = decryptedMsg.substr(parts[0].length() + parts[1].length() + 2);
                lock_guard<mutex> lock(usernameMutex);
                if (usernameToSocket.count(target)) {
                    string formatted = "[PM from " + username + "] " + content;
                    string encrypted = crypto->encrypt(formatted);
                    send(usernameToSocket[target], encrypted.c_str(), encrypted.size(), 0);
                } else {
                    string error = "User not found!";
                    send(clientSocket, crypto->encrypt(error).c_str(), error.size(), 0);
                }
            }
        }
        else if (decryptedMsg.find("/join ") == 0) {
            vector<string> parts = split(decryptedMsg, ' ');
            if (parts.size() >= 2) {
                string room = parts[1];
                {
                    lock_guard<mutex> lock(chatRoomMutex);
                    for (auto& pair : chatRooms) {
                        auto& vec = pair.second;
                        vec.erase(remove(vec.begin(), vec.end(), clientSocket), vec.end());
                    }
                    chatRooms[room].push_back(clientSocket);
                    clientToRoom[clientSocket] = room;
                }
                string response = "Joined room: " + room;
                send(clientSocket, crypto->encrypt(response).c_str(), response.size(), 0);
            }
        }
        else if (decryptedMsg.find("/sendfile ") == 0) {
            vector<string> parts = split(decryptedMsg, ' ');
            if (parts.size() >= 3) {
                string target = parts[1];
                string filepath = parts[2];
                thread(sendFileChunk, clientSocket, filepath, target).detach();
            }
        }
        else if (decryptedMsg.find("/file_start ") == 0) {
            vector<string> parts = split(decryptedMsg, ' ');
            if (parts.size() >= 4) {
                string sender = username;
                string recipient = parts[1];
                string filename = parts[2];
                size_t total_size = stoull(parts[3]);

                lock_guard<mutex> lock(transferMutex);
                activeTransfers[filename] = {filename, sender, recipient, total_size, 0, ofstream(), true};
                activeTransfers[filename].file_stream.open(filename, ios::binary);
            }
        }
        else if (decryptedMsg.find("/file_chunk ") == 0) {
            vector<string> parts = split(decryptedMsg, ' ');
            if (parts.size() >= 3) {
                string filename = parts[1];
                string chunk_data = base64_decode(parts[2]);

                lock_guard<mutex> lock(transferMutex);
                if (activeTransfers.count(filename) && activeTransfers[filename].in_progress) {
                    FileTransfer& transfer = activeTransfers[filename];
                    transfer.file_stream.write(chunk_data.c_str(), chunk_data.size());
                    transfer.received += chunk_data.size();

                    // Progress update every 10%
                    if (transfer.total_size > 0) {
                        int progress = (transfer.received * 100) / transfer.total_size;
                        if (progress % 10 == 0) {
                            string progress_msg = "FILE_PROGRESS: " + filename + " " + to_string(progress) + "%";
                            send(clientSocket, crypto->encrypt(progress_msg).c_str(), progress_msg.size(), 0);
                        }
                    }
                }
            }
        }
        else if (decryptedMsg.find("/file_end ") == 0) {
            vector<string> parts = split(decryptedMsg, ' ');
            if (parts.size() >= 2) {
                string filename = parts[1];

                lock_guard<mutex> lock(transferMutex);
                if (activeTransfers.count(filename)) {
                    activeTransfers[filename].file_stream.close();
                    activeTransfers[filename].in_progress = false;
                    string completion = "FILE_COMPLETE: " + filename;
                    send(clientSocket, crypto->encrypt(completion).c_str(), completion.size(), 0);
                    activeTransfers.erase(filename);
                }
            }
        }
        else {
            string room;
            {
                lock_guard<mutex> lock(chatRoomMutex);
                room = clientToRoom[clientSocket];
            }

            Message msg;
            msg.id = messageIdCounter++;
            msg.timestamp = getTimestamp();
            msg.sender = username;
            msg.content = decryptedMsg;
            msg.room = room;
            msg.encrypted = true;
            {
                lock_guard<mutex> lock(messageHistoryMutex);
                messageHistory.push_back(msg);
            }

            string formatted = "[" + room + "] " + username + ": " + decryptedMsg;
            string encrypted = crypto->encrypt(formatted);

            if (!room.empty()) {
                broadcastToRoom(encrypted, clientSocket, room);
            } else {
                broadcastQueue.push(encrypted);
            }
        }
    }

    {
        lock_guard<mutex> lock(clientSocketMutex);
        clientSockets.erase(remove(clientSockets.begin(), clientSockets.end(), clientSocket), clientSockets.end());
    }
    {
        lock_guard<mutex> lock(usernameMutex);
        usernameToSocket.erase(username);
    }
    {
        lock_guard<mutex> lock(userStatusMutex);
        userStatus[username] = false;
    }

    delete crypto;
    close(clientSocket);
    logMessage("User " + username + " disconnected", "INFO");
}

void startServer(const string& username) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Set socket to non-blocking
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (::bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    cout << COLOR_MSG << "Server started. Waiting for connections..." << COLOR_RESET << endl;
    logMessage("Server started", "INFO");

    // Start broadcast worker thread
    broadcastThread = thread(broadcastWorker);

    fd_set read_fds;
    int max_sd;
    struct timeval timeout;

    while (serverRunning) {
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        max_sd = server_fd;

        {
            lock_guard<mutex> lock(clientSocketMutex);
            for (int sock : clientSockets) {
                FD_SET(sock, &read_fds);
                if (sock > max_sd) max_sd = sock;
            }
        }

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(max_sd + 1, &read_fds, NULL, NULL, &timeout);
        if (activity < 0 && errno != EINTR) {
            perror("select error");
            continue;
        }

        if (FD_ISSET(server_fd, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int new_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
            if (new_socket < 0) {
                perror("accept failed");
                continue;
            }

            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
            logMessage("New connection from " + string(client_ip), "INFO");

            {
                lock_guard<mutex> lock(clientSocketMutex);
                clientSockets.push_back(new_socket);
            }
            thread(handleClient, new_socket).detach();
        }
    }

    // Cleanup
    {
        lock_guard<mutex> lock(clientSocketMutex);
        for (int sock : clientSockets) {
            close(sock);
        }
    }
    close(server_fd);
    logMessage("Server stopped", "INFO");
}

void receiveMessages(int sock, CryptoHandler* crypto) {
    char buffer[BUFFER_SIZE];
    unordered_map<string, ofstream> file_transfers;

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(sock, buffer, BUFFER_SIZE, 0);
        if (bytesReceived <= 0) {
            cout << COLOR_RECV << "\nConnection closed.\n" << COLOR_RESET;
            break;
        }

        string decrypted;
        try {
            decrypted = crypto->decrypt(string(buffer, bytesReceived));
            if (decrypted.empty()) {
                throw runtime_error("Decryption failed");
            }
        } catch (...) {
            cout << COLOR_ERROR << "\nError decrypting message\n" << COLOR_RESET;
            continue;
        }

        if (decrypted.find("/file_start ") == 0) {
            vector<string> parts = split(decrypted, ' ');
            if (parts.size() >= 4) {
                string filename = parts[2];
                size_t total_size = stoull(parts[3]);

                // Sanitize filename
                filename = filename.substr(filename.find_last_of("/\\") + 1);

                cout << COLOR_RECV << "\nReceiving file: " << filename
                     << " (" << total_size/1024 << " KB)" << COLOR_RESET << endl;

                file_transfers[filename].open(filename, ios::binary);
                if (!file_transfers[filename]) {
                    cout << COLOR_ERROR << "Failed to create file: " << filename << COLOR_RESET << endl;
                    file_transfers.erase(filename);
                }
            }
        }
        else if (decrypted.find("/file_chunk ") == 0) {
            vector<string> parts = split(decrypted, ' ');
            if (parts.size() >= 3) {
                string filename = parts[1];
                string chunk = base64_decode(parts[2]);

                if (file_transfers.count(filename)) {
                    file_transfers[filename].write(chunk.c_str(), chunk.size());
                }
            }
        }
        else if (decrypted.find("/file_end ") == 0) {
            vector<string> parts = split(decrypted, ' ');
            if (parts.size() >= 2) {
                string filename = parts[1];
                if (file_transfers.count(filename)) {
                    file_transfers[filename].close();
                    file_transfers.erase(filename);
                    cout << COLOR_RECV << "\nFile transfer complete: " << filename << COLOR_RESET << endl;
                }
            }
        }
        else if (decrypted.find("/file_progress ") == 0) {
            vector<string> parts = split(decrypted, ' ');
            if (parts.size() >= 3) {
                string filename = parts[1];
                string progress = parts[2];
                cout << COLOR_RECV << "\nFile transfer progress (" << filename << "): "
                     << progress << "%" << COLOR_RESET << endl;
            }
        }
        else {
            cout << COLOR_RECV << "\n[" << getTimestamp() << "] " << COLOR_RESET
                 << decrypted << endl;
        }
        playNotificationSound();
    }

    // Cleanup any unfinished file transfers
    for (auto& pair : file_transfers) {
        pair.second.close();
        remove(pair.first.c_str());
    }
}

void startClient(const string& serverIP, string& username, string& password) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, serverIP.c_str(), &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Generate salt and perform authentication
    string salt = CryptoHandler::generate_salt();
    string authMsg = username + "\x1F" + password + "\x1F" + salt;
    send(sock, authMsg.c_str(), authMsg.size(), 0);

    char authResponse[BUFFER_SIZE];
    memset(authResponse, 0, BUFFER_SIZE);
    int authBytes = recv(sock, authResponse, BUFFER_SIZE, 0);
    if (authBytes <= 0) {
        cout << COLOR_ERROR << "Connection failed" << COLOR_RESET << endl;
        close(sock);
        exit(EXIT_FAILURE);
    }

    string authStatus(authResponse, authBytes);
    if (authStatus.find("AUTH_ERROR") == 0) {
        cout << COLOR_ERROR << authStatus << COLOR_RESET << endl;
        close(sock);
        exit(EXIT_FAILURE);
    }

    CryptoHandler crypto(password, salt);
    receiveThread = thread(receiveMessages, sock, &crypto);
    receiveThread.detach();

    string message;
    while (true) {
        cout << COLOR_MSG << "You [" << getTimestamp() << "]: " << COLOR_RESET;
        getline(cin, message);

        if (message == "/exit") {
            send(sock, crypto.encrypt("/exit").c_str(), crypto.encrypt("/exit").size(), 0);
            break;
        }
        else if (message == "/help") {
            cout << "Commands:\n"
                 << "/pm <user> <msg> - Private message\n"
                 << "/join <room> - Join room\n"
                 << "/status <user> - Check status\n"
                 << "/history <n> - Get message history\n"
                 << "/sendfile <user> <filepath> - Send file\n"
                 << "/clear - Clear screen\n"
                 << "/encrypt <msg> - Send encrypted message\n"
                 << "/who - List online users\n";
        }
        else if (message == "/clear") {
            clearScreen();
        }
        else if (message == "/who") {
            send(sock, crypto.encrypt("/who").c_str(), crypto.encrypt("/who").size(), 0);
        }
        else if (message.find("/sendfile ") == 0) {
            send(sock, crypto.encrypt(message).c_str(), crypto.encrypt(message).size(), 0);
        }
        else if (message.find("/encrypt ") == 0) {
            string toEncrypt = message.substr(8);
            string encrypted = crypto.encrypt(toEncrypt);
            cout << "Encrypted: " << encrypted << endl;
        }
        else {
            send(sock, crypto.encrypt(message).c_str(), crypto.encrypt(message).size(), 0);
        }
    }
    close(sock);
}

int main() {
    // Initialize OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    string username, password;
    cout << "Enter username: ";
    getline(cin, username);

    cout << "Enter password: ";
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    getline(cin, password);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    cout << endl;

    cout << "1. Start server\n2. Connect to server\nChoice: ";
    int choice;
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        startServer(username);
    } else if (choice == 2) {
        string serverIP;
        cout << "Server IP: ";
        getline(cin, serverIP);
        startClient(serverIP, username, password);
    } else {
        cout << "Invalid choice!" << endl;
    }

    // Clean up OpenSSL
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}

void OpenSSL_add_all_algorithms() {

}

void ERR_free_strings() {

}

void ERR_load_crypto_strings() {

}

void EVP_cleanup() {

}
