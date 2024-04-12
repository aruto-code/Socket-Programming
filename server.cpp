#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <json/json.h>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

// Function to count frequency of a word in a string
int countFrequency(const std::string& content, const std::string& word) {
    int frequency = 0;
    size_t pos = content.find(word, 0);
    while (pos != std::string::npos) {
        frequency++;
        pos = content.find(word, pos + word.size());
    }
    return frequency;
}

int main() {
    // Create server socket
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return 1;
    }

    std::cout << "Server socket created" << std::endl;

    // Bind socket to port
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080); // Port number
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        return 1;
    }

    std::cout << "Socket bound to port 8080" << std::endl;

    // Listen for connections
    listen(serverSocket, 5);
    std::cout << "Server listening on port 8080" << std::endl;

    // Accept connection
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
    if (clientSocket < 0) {
        std::cerr << "Error accepting connection" << std::endl;
        return 1;
    }

    std::cout << "Connection accepted from client" << std::endl;
   std::cout <<"_________________________________________________________________________________________________________________________________________________________" <<std::endl;
std::cout <<"" <<std::endl;
    // Receive data from client
    char buffer[1024];
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::string jsonStr(buffer);

    std::cout << "Received JSON data from client: " << std::endl << jsonStr << std::endl;
    std::cout <<"" <<std::endl;
    std::cout <<"_________________________________________________________________________________________________________________________________________________________" <<std::endl;
    // Parse JSON data
    Json::CharReaderBuilder jsonBuilder;
    Json::Value jsonData;
    JSONCPP_STRING errs;
    std::istringstream jsonStream(jsonStr);
    if (!Json::parseFromStream(jsonBuilder, jsonStream, &jsonData, &errs)) {
        std::cerr << "Error parsing JSON: " << errs << std::endl;
        return 1;
    }
    std::cout <<"" <<std::endl;
    std::cout << "JSON data parsed successfully" << std::endl;

    // Perform frequency counting
    Json::Value result;
    int totalFrequency = 0;
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        std::string filename = it.key().asString();
        if (filename != "fileS") {
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error opening file: " << filename << std::endl;
                continue;
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string content = buffer.str();

            int frequency = countFrequency(content, jsonData["fileS"].asString());
            totalFrequency += frequency;
           // result[filename + " content"] = content;
            result[filename + " frequency"] = frequency;
        }
    }
    result["TotalFrequency"] = totalFrequency;

    std::cout << "Frequency counting completed" << std::endl;
    std::cout <<"" <<std::endl;
    std::cout <<"_________________________________________________________________________________________________________________________________________________________" <<std::endl;
     std::cout <<"" <<std::endl;

    // Convert result to JSON string
    Json::StreamWriterBuilder builder;
    builder["indentation"] = "\t"; // Set indentation for better readability
    std::string resultStr = Json::writeString(builder, result);
    

    std::cout << "Result converted to JSON string: " << std::endl << resultStr << std::endl;

    // Send resultStr to client
    send(clientSocket, resultStr.c_str(), resultStr.size(), 0);
    
         std::cout <<"" <<std::endl;
    std::cout <<"_________________________________________________________________________________________________________________________________________________________" <<std::endl;
     std::cout <<"" <<std::endl;


    std::cout << "Result sent to client" << std::endl;

    // Do not close sockets here

    return 0;
}

