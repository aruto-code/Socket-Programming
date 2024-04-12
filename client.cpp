#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <json/json.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h> // Include dirent.h for directory operations
#include <vector>

// Function to list all files with a specific extension in a directory
std::vector<std::string> listFilesWithExtension(const std::string& dirPath, const std::string& extension) {
    std::vector<std::string> files;
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(dirPath.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string filename = ent->d_name;
            if (filename.length() >= extension.length() && filename.compare(filename.length() - extension.length(), extension.length(), extension) == 0) {
                files.push_back(filename);
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Error opening directory: " << dirPath << std::endl;
    }
    return files;
}

int main() {
    // Create client socket
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080); // Server port
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address

    // Connect to server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed\n";
        return 1;
    }
   
    // Read fileS.txt to get the word
    std::ifstream fileS("fileS.txt");
    std::string word;
    if (!fileS.is_open()) {
        std::cerr << "Error opening fileS.txt\n";
        return 1;
    }
    fileS >> word;

    // Read and prepare JSON data
    Json::Value jsonData;
    
    // List all txt files in the directory
    std::vector<std::string> txtFiles = listFilesWithExtension(".", ".txt");

    // Include content of each txt file into JSON data
    for (const std::string& filename : txtFiles) {
        if (filename != "fileS.txt") {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << filename << std::endl;
                continue;
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            jsonData[filename] = buffer.str();
        }
    }
    
    jsonData["fileS"] = word;

    // Convert JSON data to string
    Json::StreamWriterBuilder builder;
    std::string jsonStr = Json::writeString(builder, jsonData);

    // Send jsonStr to server
    send(clientSocket, jsonStr.c_str(), jsonStr.size(), 0);

    // Receive resultStr from server
    char buffer[1024];
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::string resultStr(buffer);

    // Print received JSON string
    std::cout << "Received JSON from server:\n" << resultStr << std::endl;
         std::cout <<"" <<std::endl;
    std::cout <<"_________________________________________________________________________________________________________________________________________________________" <<std::endl;
     std::cout <<"" <<std::endl;



    // Parse JSON data
    Json::CharReaderBuilder jsonBuilder;
    Json::Value result;
    JSONCPP_STRING errs;
    std::istringstream resultStream(resultStr);
    if (!Json::parseFromStream(jsonBuilder, resultStream, &result, &errs)) {
        std::cerr << "Error parsing JSON: " << errs << std::endl;
        return 1;
    }

    // Process result
    std::cout << "Received result from server:\n";
    for (auto it = result.begin(); it != result.end(); ++it) {
        std::cout << it.key().asString() << ": " << it->asString() << std::endl;
    }
          std::cout <<"" <<std::endl;
    std::cout <<"_________________________________________________________________________________________________________________________________________________________" <<std::endl;
     std::cout <<"" <<std::endl;

    // Close socket
    close(clientSocket);

    return 0;
}
  
