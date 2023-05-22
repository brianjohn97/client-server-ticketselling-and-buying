#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct Connection {
    std::string IP;
    int Port;
    int Timeout;
};

int main() {
    std::ifstream file("test.txt");  // Replace "example.txt" with your file path

    if (file.is_open()) {
        std::vector<Connection> connections;
        std::string line;
        Connection currentConnection;

        while (std::getline(file, line)) {
            if (line == "[Connection]") {
                if (!currentConnection.IP.empty())
                    connections.push_back(currentConnection);

                currentConnection = Connection();
            }
            else if (line.substr(0, 4) == "IP =") {
                currentConnection.IP = line.substr(5);
            }
            else if (line.substr(0, 6) == "Port =") {
                currentConnection.Port = std::stoi(line.substr(7));
            }
            else if (line.substr(0, 9) == "Timeout =") {
                currentConnection.Timeout = std::stoi(line.substr(10));
            }
        }

        if (!currentConnection.IP.empty())
            connections.push_back(currentConnection);

        file.close();

        // Print the connections
        for (const auto& connection : connections) {
            std::cout << "IP: " << connection.IP << std::endl;
            std::cout << "Port: " << connection.Port << std::endl;
            std::cout << "Timeout: " << connection.Timeout << std::endl;
            std::cout << std::endl;
        }
    } else {
        std::cout << "Unable to open the file." << std::endl;
    }

    return 0;
}
