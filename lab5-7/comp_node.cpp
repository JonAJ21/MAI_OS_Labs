#include <iostream>
#include <string>

#include <chrono>
#include <thread>

#include <unistd.h>

#include "SimplePocoHandler.h"
#include "amqp-cpp.h"

using namespace std::chrono_literals;

std::string get_command1(std::string const & msg) {
    std::string res = "";
    for (char ch : msg) {
        if (ch == ' ') {
            return res;
        }
        res += ch;
    }
    return "";
}

void get_command3(std::string const & msg, std::string& command, std::string& arg1) {
    int flag = 0;
    for (char ch : msg) {
        if (ch == ' ') {
            flag++;
            continue;
        }
        if (flag == 0) {
            command += ch;
        }
        if (flag == 1) {
            arg1 += ch;
        }
    }
}


void get_command4(std::string const & msg, std::string& command, std::string& arg1, std::string& arg2) {
    int flag = 0;
    for (char ch : msg) {
        if (ch == ' ') {
            flag++;
            continue;
        }
        if (flag == 0) {
            command += ch;
        }
        if (flag == 1) {
            arg1 += ch;
        }
        if (flag == 2) {
            arg2 += ch;
        }
    }
}


class LocalTimer final {
private:
    size_t _time;
    std::chrono::_V2::system_clock::time_point _start;
    bool was_started;
public:
    LocalTimer() : _time(0), was_started(false) {}
    void start(){
        _start = std::chrono::high_resolution_clock::now();
        was_started = true;
    }
    void stop() {
        auto end = std::chrono::high_resolution_clock::now();
        _time += std::chrono::duration_cast<std::chrono::seconds>(end - _start).count();
        was_started = false;
        
    }
    std::string time() {
        auto end = std::chrono::high_resolution_clock::now();
        if (!was_started) _start = end;
        _time += std::chrono::duration_cast<std::chrono::seconds>(end - _start).count();
        _start = std::chrono::high_resolution_clock::now();
        return std::to_string(_time);
    };
};




int main(int argc, char * argv[]) {
    SimplePocoHandler handler("localhost", 5672);
    AMQP::Connection connection(&handler, AMQP::Login("computing_node", "qwerty"), "/");
    AMQP::Channel channel(&connection);
    
    std::string child_node_id_str{argv[2]};
    std::string this_node_id_str{argv[3]};
    std::string parent_node_id_str{argv[4]};

    std::string up_out_queue = parent_node_id_str;
    std::string in_queue = this_node_id_str;

    std::string down_out_queue = child_node_id_str;
    // std::string down_in_queue = up_in_queue;
    LocalTimer timer;

    channel.onReady([&]() {
        std::string msg = "up created Computing node created: " + this_node_id_str + " from " + parent_node_id_str;
        
        channel.publish("", up_out_queue.c_str(), msg.c_str());
    });

    channel.consume(in_queue, AMQP::noack).onReceived(
            [&] (const AMQP::Message & message, uint64_t deliveryTag, bool redelivered) 
        {
            std::string received_message{message.body()};
            received_message.resize(message.bodySize());

            std::string command = get_command1(received_message);
            if (command == "create") {
                std::string node_id_to_create_str = "";
                std::string node_id_who_create_str = "";
                get_command4(received_message, command, node_id_to_create_str, node_id_who_create_str);
                if (child_node_id_str == "-1") {
                    child_node_id_str = node_id_to_create_str;
                    down_out_queue = child_node_id_str;
                    channel.declareQueue(down_out_queue.c_str());
                    int pid = fork();
                    if (pid == 0) {
                        std::string comp_node = "comp_node";
                        char *argv[] = {const_cast<char*>(comp_node.c_str()),
                                        const_cast<char*>(comp_node.c_str()),
                                        const_cast<char*>("-1"),
                                        const_cast<char*>(child_node_id_str.c_str()),
                                        const_cast<char*>(this_node_id_str.c_str()),
                                        NULL};
                        if (execv(argv[0], argv) == -1) {
                            std::cout << "Error: exec computing_node" << std::endl;
                        }
                    }
                } else {
                    channel.publish("", down_out_queue.c_str(), received_message.c_str());
                }

            }

            if (command == "kill") {
                std::string node_to_kill_str = "";
                get_command3(received_message, command, node_to_kill_str);

                if (node_to_kill_str == child_node_id_str) {
                    channel.publish("", down_out_queue.c_str(), received_message.c_str());
                    child_node_id_str = "-1";
                    
                } else if (node_to_kill_str == this_node_id_str) {
                    // std::string msg = "up killed " + received_message + "from " + this_node_id_str;
                    std::string msg = "up killed Computing node killed: " + this_node_id_str;
                    channel.publish("", up_out_queue.c_str(), msg.c_str());
                    // handler.quit();
                } else {
                    channel.publish("", down_out_queue.c_str(), received_message.c_str());
                }
            }

            if (command == "start") {
                std::string node_id_str = "";
                get_command3(received_message, command, node_id_str);
                if (node_id_str == this_node_id_str) {
                    timer.start();
                    std::string msg = "up exec Timer started: " + node_id_str;
                    channel.publish("", up_out_queue.c_str(), msg.c_str());
                } else {
                    channel.publish("", down_out_queue.c_str(), received_message.c_str());
                }
            }

            if (command == "time") {
                std::string node_id_str = "";
                get_command3(received_message, command, node_id_str);
                if (node_id_str == this_node_id_str) {
                    std::string msg = "up exec Time: " + timer.time();
                    channel.publish("", up_out_queue.c_str(), msg.c_str());
                } else {
                    channel.publish("", down_out_queue.c_str(), received_message.c_str());
                }
            }

            if (command == "stop") {
                std::string node_id_str = "";
                get_command3(received_message, command, node_id_str);
                if (node_id_str == this_node_id_str) {
                    std::string msg = "up exec Timer stopped: " + node_id_str;
                    timer.stop();
                    channel.publish("", up_out_queue.c_str(), msg.c_str());
                } else {
                    channel.publish("", down_out_queue.c_str(), received_message.c_str());
                }
            }

            if (command == "heartbit") {
                std::string node_id_str = "";
                std::string heartbit_time = "";
                get_command4(received_message, command, node_id_str, heartbit_time);
                if (node_id_str == this_node_id_str) {
                    std::string msg = "up " + received_message;
                    channel.publish("", up_out_queue.c_str(), msg.c_str());
                
                } else {
                    channel.publish("", down_out_queue.c_str(), received_message.c_str());
                }
            }

            if (command == "up") {
                channel.publish("", up_out_queue.c_str(), received_message.c_str());
            }


        }
    );



    handler.loop();
    return 0;
}