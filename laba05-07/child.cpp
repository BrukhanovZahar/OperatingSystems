#include "zmq.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <zconf.h>
#include <vector>
#include <signal.h>
#include <fstream>
#include <algorithm>
#include <thread>

// g++ child.cpp -lzmq -o child -w

using namespace std;


void sendMessage(const string& messageString, zmq::socket_t& socket) {
    zmq::message_t messageBack(messageString.size());
    memcpy(messageBack.data(), messageString.c_str(), messageString.size());

    if (!socket.send(messageBack)) {
        cerr << "Error: can't send message from node with pid " << getpid() << endl;
    }
}

void calculationSeparateThread(string receivedMessage, string idProcString, int idProc) {
    cout << "Function started in thread: " << std::this_thread::get_id() << endl;
    sleep(10);

    int flag = 0;
    string text, pattern, returnMessage;
    vector<int> answer;

    // разделяем text и pattern
    for (int i = 6 + idProcString.size(); i < receivedMessage.size(); ++i) {
        if (receivedMessage[i] == ' ') {
            ++flag;
        } else if ((receivedMessage[i] != ' ') && (flag == 0)) {
            text += receivedMessage[i];
        } else if ((receivedMessage[i] != ' ') && (flag == 1)) {
            pattern += receivedMessage[i];
        }
    }

    if (text.size() >= pattern.size()) {
        int start = 0;
        while (text.find(pattern, start) != -1) {
            start = text.find(pattern, start);
            answer.push_back(start);
            ++start;
        }
    }

    if (answer.empty()) {
        returnMessage = "-1";
    } else {
        returnMessage = to_string(answer[0]);
        for (int i = 1; i < answer.size(); ++i) {
            returnMessage = returnMessage + ";" + to_string(answer[i]);
        }
    }

    cout << endl << "OK:" << to_string(idProc) + ":" + returnMessage << endl;

    cout << "Function completed in thread: " << std::this_thread::get_id() << endl;
}


int main(int argc, char* argv[]) {
    string adr = argv[1];
    zmq::context_t context(1);
    zmq::socket_t mainSocket(context, ZMQ_REQ);
    mainSocket.connect(argv[1]);

    sendMessage("OK: " + to_string(getpid()), mainSocket);

    int idThisNode = stoi(argv[2]);
    int childNodeId = 0;
    zmq::context_t contextChild(1);
    zmq::socket_t childSocket(contextChild, ZMQ_REP);
    string adrChild = "tcp://127.0.0.1:300";

    while (true) {

        zmq::message_t messageMain;
        mainSocket.recv(messageMain);
        string receivedMessage(static_cast<char*>(messageMain.data()), messageMain.size());
        string command;

        for (char element: receivedMessage) {
            if (element != ' ') {
                command += element;
            } else {
                break;
            }
        }

        if (command == "exec") {


            int idProc; // id of node for adding
            string idProcString;

            for (int i = 5; i < receivedMessage.size(); ++i) {
                if (receivedMessage[i] != ' ') {
                    idProcString += receivedMessage[i];
                } else {
                    break;
                }
            }

            idProc = stoi(idProcString);

            if (idProc == idThisNode) {

                thread workThread(calculationSeparateThread, receivedMessage, idProcString, idProc);

                workThread.detach();

                cout << "Main thread continues execution without waiting for myThread to finish." << endl;


                string returnMessage = "The child process performs calculations and outputs them when it finishes calculations";
                sendMessage(returnMessage, mainSocket);

            } else {

                if (childNodeId == 0) {
                    sendMessage("Error: id: Not found", mainSocket);
                } else {
                    zmq::message_t message(receivedMessage.size());
                    memcpy(message.data(), receivedMessage.c_str(), receivedMessage.size());

                    if (!childSocket.send(message)) {
                        cerr << "Error: can't send message to child node from node with pid: " << getpid() << endl;
                    }
                    if (!childSocket.recv(message)) {
                        cerr << "Error: can't receive message from child node in node with pid: " << getpid() << endl;
                    }
                    if (!mainSocket.send(message)) {
                        cerr << "Error: can't send message to main node from node with pid: " << getpid() << endl;
                    }
                }
            }


        } else if (command == "create") {


            bool isSpace = false;
            int idNewProc, parentIdNewProc;
            string idNewProcString, parentIdNewProcString;

            for (int i = 7; i < receivedMessage.size(); ++i) {
                if (receivedMessage[i] == ' ') {
                    isSpace = true;
                } else if (receivedMessage[i] != ' ' && !isSpace) {
                    idNewProcString += receivedMessage[i];
                } else if (receivedMessage[i] != ' ' && isSpace) {
                    parentIdNewProcString += receivedMessage[i];
                }
            }

            idNewProc = stoi(idNewProcString);
            parentIdNewProc = stoi(parentIdNewProcString);

            if (idNewProc == idThisNode) {
                sendMessage("Error: Already exists", mainSocket);
            } else {

                if (childNodeId == 0 && parentIdNewProc == idThisNode) {

                    // ничего не трогаем
                    childNodeId = idNewProc;
                    childSocket.bind(adrChild + to_string(childNodeId));
                    adrChild += to_string(childNodeId);

                    char* adrChildTmp = new char[adrChild.size() + 1];
                    memcpy(adrChildTmp, adrChild.c_str(), adrChild.size() + 1);
                    char* childIdTmp = new char[to_string(childNodeId).size() + 1];
                    memcpy(childIdTmp, to_string(childNodeId).c_str(), to_string(childNodeId).size() + 1);
                    char* args[] = {"./child", adrChildTmp, childIdTmp, NULL};

                    int procesId = fork();

                    if (procesId == 0) {
                        execv("./child", args);
                    } else if (procesId < 0) {
                        cerr << "Error in forking in node with pid: " << getpid() << endl;
                    } else {
                        zmq::message_t messageFromNode;

                        if (!childSocket.recv(messageFromNode)) {
                            cerr << "Error: can't receive message from child node in node with pid:" << getpid()
                                 << endl;
                        }

                        if (!mainSocket.send(messageFromNode)) {
                            cerr << "Error: can't send message to main node from node with pid:" << getpid() << endl;
                        }
                    }

                    delete[] adrChildTmp;
                    delete[] childIdTmp;

                } else if (childNodeId == 0 && parentIdNewProc != idThisNode) {

                    sendMessage("Error: there is no such parent", mainSocket);

                } else if (childNodeId != 0 && parentIdNewProc == idThisNode) {

                    sendMessage("Error: this parent already has a child", mainSocket);

                } else {
                    sendMessage(receivedMessage, childSocket);
                    zmq::message_t message;
                    if (!childSocket.recv(message)) {
                        cerr << "Error: can't receive message from child node in node with pid: " << getpid() << endl;
                    }

                    if (!mainSocket.send(message)) {
                        cerr << "Error: can't send message to main node from node with pid: " << getpid() << endl;
                    }
                }
            }

        } else if (command == "ping") {
            int idProc;
            string idProcString;

            for (int i = 5; i < receivedMessage.size(); ++i) {
                if (receivedMessage[i] != ' ') {
                    idProcString += receivedMessage[i];
                } else {
                    break;
                }
            }

            idProc = stoi(idProcString);

            if (idProc == idThisNode) {
                sendMessage("OK: 1", mainSocket);
            } else {

                if (childNodeId == 0) {
                    sendMessage("OK: 0", mainSocket);
                } else {
                    childSocket.send(messageMain);
                    zmq::message_t answer;
                    childSocket.recv(answer);
                    mainSocket.send(answer);
                }
            }

        } else if (command == "kill") {

            int idProcToKill;
            string idProcToKillString;

            for (int i = 5; i < receivedMessage.size(); ++i) {
                if (receivedMessage[i] != ' ') {
                    idProcToKillString += receivedMessage[i];
                } else {
                    break;
                }
            }

            idProcToKill = stoi(idProcToKillString);

            if (childNodeId == 0) {
                sendMessage("Error: there isn`t node with this id", mainSocket);
            } else {
                if (childNodeId == idProcToKill) {
                    sendMessage("Ok: " + to_string(childNodeId), mainSocket);
                    sendMessage("DIE", childSocket);
                    childSocket.unbind(adrChild);
                    adrChild = "tcp://127.0.0.1:300";
                    childNodeId = 0;
                } else {
                    childSocket.send(messageMain);
                    zmq::message_t message;
                    childSocket.recv(message);
                    mainSocket.send(message);
                }
            }

        } else if (command == "DIE") {
            if (childNodeId != 0) {
                sendMessage("DIE", childSocket);
                childSocket.unbind(adrChild);
            }
            mainSocket.unbind(adr);
            return 0;
        }
    }

}