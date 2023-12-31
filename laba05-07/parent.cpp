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

using namespace std;

zmq::context_t context(1);
string adr = "tcp://127.0.0.1:300";
string command;
vector<int> childesId;
vector<unique_ptr<zmq::socket_t>> sockets;

void createChildFromMainNode(int childId) {
    auto socket = std::make_unique<zmq::socket_t>(context, ZMQ_REP);

    socket->bind(adr + to_string(childId));
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

    childesId.push_back(childId);
    sockets.push_back(std::move(socket));

    zmq::message_t message;
    sockets[sockets.size() - 1]->recv(message);


    string receiveMessage(static_cast<char*>(message.data()), message.size());
    cout << receiveMessage << endl;

    delete[] adr_;
    delete[] id_;
}

void funcCreate() {
    int childId, parentId;
    cin >> childId >> parentId;

    if (childesId.empty()) {

        if (parentId != -1) {
            cerr << "There is no such parent node" << endl;
            return;
        }

        createChildFromMainNode(childId);

    } else {
        if (parentId == -1) {

            bool wasChild = false;
            for (int indexInChildes = 0; indexInChildes < childesId.size(); ++indexInChildes) {
                if (childesId[indexInChildes] == childId) {
                    cout << "This id has already been created" << endl;
                    wasChild = true;
                    break;
                }
            }
            if (wasChild) {
                return;
            }

            createChildFromMainNode(childId);

        } else {
            string messageString = command + " " + to_string(childId) + " " + to_string(parentId);

            for (int indexOfSockets{0}; indexOfSockets < sockets.size(); ++indexOfSockets) {

                zmq::message_t message(messageString.size());
                memcpy(message.data(), messageString.c_str(), messageString.size());

                sockets[indexOfSockets]->send(message);
                sockets[indexOfSockets]->recv(message);
                string receiveMessage(static_cast<char*>(message.data()), message.size());

                if (receiveMessage[0] == 'O' && receiveMessage[1] == 'K') {
                    cout << receiveMessage << endl;
                    break;
                } else if (receiveMessage == "Error: Already exists") {
                    cout << receiveMessage << endl;
                    break;
                } else if (receiveMessage == "Error: this parent already has a child") {
                    cout << receiveMessage << endl;
                    break;
                } else if (receiveMessage == "Error: there is no such parent" &&
                           indexOfSockets == sockets.size() - 1) {
                    cout << receiveMessage << endl;
                    break;
                }
            }
        }
    }
}

void funcExec() {
    int id;
    string text, pattern;
    cin >> id;
    cin >> text >> pattern;

    string messageString = command + " " + to_string(id) + " " + text + " " + pattern;

    for (int indexOfSockets{0}; indexOfSockets < sockets.size(); ++indexOfSockets) {

        zmq::message_t message(messageString.size());
        memcpy(message.data(), messageString.c_str(), messageString.size());

        sockets[indexOfSockets]->send(message);
        sockets[indexOfSockets]->recv(message);
        string receiveMessage(static_cast<char*>(message.data()), message.size());

        if (receiveMessage[0] == 'T' && receiveMessage[1] == 'h' && receiveMessage[2] == 'e') {
            cout << receiveMessage << endl;
            break;
        } else if (receiveMessage == "Error: id: Not found" &&
                   indexOfSockets == sockets.size() - 1) {
            cout << receiveMessage << endl;
            break;
        }

    }

}

void funcPing() {
    int id;
    cin >> id;

    if (childesId.empty()) {
        cout << "OK: 0" << endl;
    } else {

        string messageString = command + " " + to_string(id);

        for (int indexOfSockets{0}; indexOfSockets < sockets.size(); ++indexOfSockets) {

            zmq::message_t message(messageString.size());
            memcpy(message.data(), messageString.c_str(), messageString.size());

            sockets[indexOfSockets]->send(message);
            sockets[indexOfSockets]->recv(message);
            string receiveMessage(static_cast<char*>(message.data()), message.size());

            if (receiveMessage == "OK: 1") {
                cout << receiveMessage << endl;
                break;
            } else if (receiveMessage == "OK: 0" &&
                       indexOfSockets == sockets.size() - 1) {
                cout << receiveMessage << endl;
                break;
            }

        }
    }
}

void funcKill() {
    int id;
    cin >> id;

    if (childesId.empty()) {
        cout << "Error: there isn't nodes" << endl;
    } else {

        for (int indexOfSockets{0}; indexOfSockets < sockets.size(); ++indexOfSockets) {

            if (childesId[indexOfSockets] == id) {
                string killMessage = "DIE";
                zmq::message_t message(killMessage.size());
                memcpy(message.data(), killMessage.c_str(), killMessage.size());
                sockets[indexOfSockets]->send(message);

                sockets[indexOfSockets]->unbind(adr + to_string(childesId[indexOfSockets]));

                childesId.erase(childesId.begin() + indexOfSockets);
                sockets.erase(sockets.begin() + indexOfSockets);

                cout << "Node deleted successfully" << endl;

                break;
            } else {
                string killMessage = command + " " + to_string(id);
                zmq::message_t message(killMessage.size());
                memcpy(message.data(), killMessage.c_str(), killMessage.size());

                sockets[indexOfSockets]->send(message);
                sockets[indexOfSockets]->recv(message);
                string receiveMessage(static_cast<char*>(message.data()), message.size());

                if (receiveMessage[0] == 'O' && receiveMessage[1] == 'K') {
                    cout << receiveMessage << endl;
                    break;
                } else if (receiveMessage == "Error: there isn`t node with this id" &&
                           indexOfSockets == sockets.size() - 1) {
                    cout << receiveMessage << endl;
                    break;
                }
            }
        }
    }
}

void funcExit() {
    for (int indexOfSockets{0}; indexOfSockets < sockets.size(); ++indexOfSockets) {

        if (childesId[indexOfSockets]) {
            string killMessage = "DIE";
            zmq::message_t message(killMessage.size());
            memcpy(message.data(), killMessage.c_str(), killMessage.size());

            sockets[indexOfSockets]->send(message);
        }
        sockets[indexOfSockets]->close();

    }

    cout << "All node was deleted" << endl;
    context.close();
    exit(0);
}

int main() {

    while (true) {

        cout << "command:";
        cin >> command;

        if (command == "create") {
            funcCreate();
        } else if (command == "exec") {
            funcExec();
        } else if (command == "ping") {
            funcPing();
        } else if (command == "kill") {
            funcKill();
        } else if (command == "exit") {
            funcExit();
        } else {
            cout << "Error: incorrect command" << endl;
        }

    }
}
