#include "zmq.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <zconf.h>
#include <vector>
#include <signal.h>
#include <sstream>
#include <set>
#include <algorithm>

// g++ parent.cpp -lzmq -o main -w
using namespace std;

int main() {
    zmq::context_t context(1); // служебная структура контекст
    zmq::socket_t mainSocket(context,
                             ZMQ_REP); // поднятие сокета в контексте ZMQ_REP для отправки запросов и получения ответов
    string adr = "tcp://127.0.0.1:300";
    string command;
    int child_id = 0;

    while (true) {
        cout << "command:";
        cin >> command;

        if (command == "create") {

            int childId, parentId;
            cin >> childId >> parentId;

            if (child_id == 0) {

                if (parentId != -1) {
                    cerr << "There is no such parent node" << endl;
                    continue;
                }

                // осталвяем все как есть

                mainSocket.bind(adr + to_string(childId));
                string new_adr = adr + to_string(childId);

                char* adr_ = new char[new_adr.size() + 1];
                memcpy(adr_, new_adr.c_str(), new_adr.size() + 1);

                char* id_ = new char[to_string(childId).size() + 1];
                memcpy(id_, to_string(childId).c_str(), to_string(childId).size() + 1);

                char* args[] = {"./child", adr_, id_, NULL};

                int processId = fork();
                if (processId < 0) {
                    cerr << "Unable to create first worker node" << endl;
                    childId = 0;
                    exit(1);
                } else if (processId == 0) {
                    execv("./child", args);
                }

                child_id = childId;

                zmq::message_t message;
                mainSocket.recv(message);


                string receiveMessage(static_cast<char*>(message.data()), message.size());
                cout << receiveMessage << endl;

                delete[] adr_;
                delete[] id_;

            } else {

                // добавляем информацию о родителе

                string messageString = command + " " + to_string(childId) + " " + to_string(parentId);

                zmq::message_t message(messageString.size());
                memcpy(message.data(), messageString.c_str(), messageString.size());
                mainSocket.send(message);

                mainSocket.recv(message);
                string receiveMessage(static_cast<char*>(message.data()), message.size());

                //TODO:: обработать полученное сообщение (если нет такого родителя)
                cout << receiveMessage << std::endl;
            }

        } else if (command == "exec") {
            int id;
            string text, pattern;
            cin >> id;
            cin >> text >> pattern;

            string messageString;
//            messageString.append(command + " " + std::to_string(id) + " " + text + " " + pattern);
            messageString.append(command);
            messageString.append(" ");
            messageString.append(to_string(id));
            messageString.append(" ");
            messageString.append(text);
            messageString.append(" ");
            messageString.append(pattern);

            zmq::message_t message(messageString.size());
            memcpy(message.data(), messageString.c_str(), messageString.size());
            mainSocket.send(message);

            // return value from map
            mainSocket.recv(message);
            string receiveMessage(static_cast<char*>(message.data()), message.size());
            cout << receiveMessage << endl;

        } else if (command == "ping") {
            int id;
            cin >> id;

            string messageString = command + " " + to_string(id);
            zmq::message_t message(messageString.size());
            memcpy(message.data(), messageString.c_str(), messageString.size());
            mainSocket.send(message);

            mainSocket.recv(message);
            string receiveMessage(static_cast<char*>(message.data()), message.size());
            cout << receiveMessage << endl;

        } else if (command == "kill") {
            int id;
            cin >> id;

            if (child_id == 0) {
                cout << "Error: there isn't nodes" << endl;
            } else if (child_id == id) {
                string killMessage = command + " " + to_string(id);
                zmq::message_t message(killMessage.size());
                memcpy(message.data(), killMessage.c_str(), killMessage.size());
                mainSocket.send(message);

                mainSocket.recv(message);
                string receiveMessage(static_cast<char*>(message.data()), message.size());
                cout << receiveMessage << endl;
                cout << "Tree deleted successfully" << endl;
                return 0;
            } else {
                string killMessage = command + " " + to_string(id);
                zmq::message_t message(killMessage.size());
                memcpy(message.data(), killMessage.c_str(), killMessage.size());
                mainSocket.send(message);

                mainSocket.recv(message);
                string receivedMessage(static_cast<char*>(message.data()), message.size());
                cout << receivedMessage << endl;
            }
        } else if (command == "exit") {
            if (child_id) {
                string killMessage = "DIE";
                zmq::message_t message(killMessage.size());
                memcpy(message.data(), killMessage.c_str(), killMessage.size());
                mainSocket.send(message);
                cout << "Tree was deleted" << endl;
            }

            mainSocket.close();
            context.close();
            break;
        } else {
            cout << "Error: incorrect command" << endl;
        }
    }
}
