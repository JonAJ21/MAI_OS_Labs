#include <iostream>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <memory>

#include <unistd.h>

#include "SimplePocoHandler.h"
#include "amqp-cpp.h"

using namespace std::chrono_literals;

class HeartbitNode final {
public:
    int64_t _id;
    bool _available;
    std::chrono::_V2::system_clock::time_point _start;
    HeartbitNode(int64_t id,
                 bool available,  
                 std::chrono::_V2::system_clock::time_point start) : _id(id), 
                                                                      _available(available),  
                                                                      _start(start) {}
};

bool search_for_elem(std::map<int64_t,
                     std::list<int64_t>> const & m, 
                     int64_t id) 
{
    for (auto i : m) {
        for (auto j : i.second) {
            if (j == id) {
                return true;
            }
        }
    }
    return false;
} 


std::string get_main_node(std::map<int64_t,
                          std::list<int64_t>> m,
                          int64_t id) 
{
    for (auto i : m) {
        std::string way = "";
        for (auto j : i.second) {
            way += std::to_string(j) + "-";
            if (j == id) {
                std::string main_node_id_str = "";
                for (char ch : way) {
                    if (ch == '-') {
                        return main_node_id_str;
                    }
                    main_node_id_str += ch;
                }
            }
        }
    }
    return "";                   
}



void get_msg4(std::string const & msg, std::string& command1, std::string& command2, std::string& str) {
    int flag = 0;
    for (char ch : msg) {
        if (ch == ' ' && flag < 2) {
            flag += 1;
            continue;
        }
        if (flag == 0) {
            command1 += ch;
        }
        if (flag == 1) {
            command2 += ch;
        }
        if (flag == 2) {
            str += ch;
        }
    }
}

std::string get_last_arg(std::string const & msg) {
    std::string res = "";
    for (int i = msg.size() - 1; i > 0; --i) {
        if (msg[i] == ' ') {
            return res;
        }
        res = msg[i] + res;
    }
    return res;
}

std::string get_first_arg(std::string const & msg) {
    std::string res = "";
    for (char ch : msg) {
        if (ch == ' ') {
            return res;
        }
        res += ch;
    }
    return res;
}


void create_computing_node(AMQP::Channel& channel,
                           std::map<int64_t, std::list<int64_t>>& node_id_map,
                           std::vector<HeartbitNode>& hb,
                           int64_t node_id_child,
                           int64_t node_id_parent = 0) 
{   

    if (!search_for_elem(node_id_map, node_id_parent) && node_id_parent != 0) {
        std::cout << "There is not such parrent node" << std::endl;
        
        return;
    }
    if (search_for_elem(node_id_map, node_id_child)) {
        std::cout << "Node with id " << node_id_child << " has been already created" << std::endl;
        
        return;
    }

    std::string node_id_child_str = std::to_string(node_id_child);
    std::string node_id_parent_str = std::to_string(node_id_parent);
    std::string out_queue = node_id_child_str;
    std::string in_queue = node_id_parent_str;
    
    if (node_id_parent == 0) {
        channel.declareQueue(out_queue.c_str());
        int pid = fork();
        if (pid == 0) {
            std::string comp_node = "comp_node";
            char *argv[] = {const_cast<char*>(comp_node.c_str()),
                            const_cast<char*>(comp_node.c_str()),
                            const_cast<char*>("-1"),
                            const_cast<char*>(node_id_child_str.c_str()),
                            const_cast<char*>(node_id_parent_str.c_str()),
                            NULL};
            if (execv(argv[0], argv) == -1) {
                std::cout << "Error: exec computing_node" << std::endl;
            }
        }
        node_id_map.insert(std::pair<int64_t, std::list<int64_t>>(node_id_child, {node_id_child}));
        hb.push_back(HeartbitNode(node_id_child, true, std::chrono::high_resolution_clock::now()));

    } else if (node_id_parent > 0) {
        std::string main_node_id_str = get_main_node(node_id_map, node_id_parent);
        int64_t main_node_id = stoi(main_node_id_str);
        std::string main_out_queue = main_node_id_str;

        if (node_id_map[main_node_id].back() == node_id_parent) {
            node_id_map[main_node_id].push_back(node_id_child);
            hb.push_back(HeartbitNode(node_id_child, true, std::chrono::high_resolution_clock::now()));
            std::string msg = "create " + node_id_child_str + " " + node_id_parent_str; 
            channel.publish("", main_out_queue.c_str(), msg.c_str());
        } else {
            std::cout << "Error: it is not tail of the list" << std::endl;
            
        }
    }
    
}

void kill_computing_node(AMQP::Channel& channel,
                         std::map<int64_t, std::list<int64_t>>& node_id_map,
                         std::vector<HeartbitNode>& hb,
                         int64_t node_id) 
{
    if (!search_for_elem(node_id_map, node_id)) {
        std::cout << "Node with id " << node_id << " has not been created" << std::endl;
        return;
    }

    
    std::string main_node_id_str = get_main_node(node_id_map, node_id);
    std::string msg = "kill " + std::to_string(node_id); 
    channel.publish("", main_node_id_str.c_str(), msg.c_str());
    
    auto list_begin = node_id_map[stoi(main_node_id_str)].begin();
    auto list_end = node_id_map[stoi(main_node_id_str)].end();
    
    for (auto it = list_begin; it != list_end; ++it) {
        if (*it == node_id) {
            node_id_map[stoi(main_node_id_str)].erase(it);
            break;
        }
    }
    
    for (auto it = hb.begin(); it != hb.end(); ++it) {
        if (it->_id == node_id) {
            hb.erase(it);
            break;
        }
    }
}

void exec_timer(AMQP::Channel& channel,
           std::map<int64_t, std::list<int64_t>>& node_id_map,
           int64_t node_id, std::string action) 
{
    if (!search_for_elem(node_id_map, node_id)) {
        std::cout << "Node with id " << node_id << " has not been created" << std::endl;
        
        return;
    }
    std::string main_node_id_str = get_main_node(node_id_map, node_id);
    std::string msg = action + " " + std::to_string(node_id);
    channel.publish("", main_node_id_str.c_str(), msg.c_str());
}



void heartbit(AMQP::Channel& channel,
              std::map<int64_t, std::list<int64_t>>& node_id_map,
              int64_t node_id,
              std::vector<HeartbitNode>& hb,
              int64_t heartbit_time)
{
    std::string main_node_id_str = get_main_node(node_id_map, node_id);
    std::string msg = "heartbit " + std::to_string(node_id) + " " + std::to_string(heartbit_time);
    channel.publish("", main_node_id_str.c_str(), msg.c_str());
}



int main() {
    SimplePocoHandler handler("localhost", 5672);

    AMQP::Connection connection(&handler, AMQP::Login("control_node", "qwerty"), "/");
    AMQP::Channel channel(&connection);
    
    channel.declareQueue("0");
    std::map<int64_t, std::list<int64_t>> node_id_map;
    std::vector<HeartbitNode> hb_list;
    bool hb_started = false;

    std::thread th1([&]() {
        channel.onReady([&]() {
            std::cout << "Control node created" << std::endl;
        });
        std::this_thread::sleep_for(100ms); 
        while(true) {
            std::cout << "Menu:" << std::endl;

            std::string menu_command;
            std::cin >> menu_command;
            if (menu_command == "create") {
                std::string arg1;
                std::string arg2;
                std::cin >> arg1 >> arg2;
                create_computing_node(channel, node_id_map, hb_list, stoi(arg1), stoi(arg2));
            } else if (menu_command == "kill") {
                std::string arg1;
                std::cin >> arg1;
                kill_computing_node(channel, node_id_map, hb_list, stoi(arg1));
            } else if (menu_command == "exec") {
                std::string arg1;
                std::string arg2;
                std::cin >> arg1 >> arg2;
                std::cout << arg1 << " " << arg2 << std::endl;
                exec_timer(channel, node_id_map, stoi(arg1), arg2);
            } else if (menu_command == "heartbit") {
                std::cout << "Ok" << std::endl;
                int64_t arg1;
                std::cin >> arg1;
                hb_started = true;
                // for (auto node : hb_list) {
                //     heartbit(channel, node_id_map, node._id, hb_list, arg1);
                //     node._start = std::chrono::high_resolution_clock::now();
                // }
                for (size_t i = 0; i < hb_list.size(); ++i) {
                    heartbit(channel, node_id_map, hb_list[i]._id, hb_list, arg1);
                    hb_list[i]._start = std::chrono::high_resolution_clock::now();
                }
            } else if (menu_command == "print_nodes") {
                std::cout << "map:" << std::endl;
                for (auto map_el : node_id_map) {
                    for (auto list_el : map_el.second) {
                        std::cout << list_el << ' ';
                    }
                    std::cout << std::endl;
                }
                std::cout << "list:" << std::endl;
                for (auto el : hb_list) {
                    std::cout << el._id << ' ';
                }
                    std::cout << std::endl;
            } else if (menu_command == "ping") {
                std::string node_id_str;
                std::cin >> node_id_str;
                int64_t node_id = stoi(node_id_str);
                for (auto el : hb_list) {
                    if (el._id == node_id) {
                        if (el._available == true) {
                            std::cout << "Ok: 1" << std::endl;
                        } else {
                            std::cout << "Ok: 0" << std::endl;
                        }
                        break;
                    }
               }
            } else {
                std::cout << "(" << menu_command << ")" << std::endl;
                std::cout << "Invalid command" << std::endl;
            }
        }
    });

    channel.consume("0", AMQP::noack).onReceived(
            [&] (const AMQP::Message& message, 
                    uint64_t deliveryTag,
                    bool redelivered)
        {
            std::string received_message{message.body()};
            received_message.resize(message.bodySize());
            
            std::string command1 = "";
            std::string command2 = "";
            std::string msg = "";
            get_msg4(received_message, command1, command2, msg);
            // std::cout << command1 << " " << command2 << "+" << msg << std::endl;
            if (command1 == "up") {
                if (command2 == "created") {
                    std::cout << "Received " << msg << std::endl;
                    
                }
                if (command2 == "killed") {
                    std::string killed_node_id_str = get_last_arg(msg);
                    channel.removeQueue(killed_node_id_str);
                    std::cout << "Received " << msg << std::endl;
                    
                }
                if (command2 == "exec") {
                    std::cout << "Received " << msg << std::endl;
                   
                } 
                if (command2 == "heartbit") {
                    // std::cout << "Received " << msg << std::endl;
                    std::string node_id_str = get_first_arg(msg);
                    std::string heartbit_time = get_last_arg(msg);

                    for (size_t i = 0; i < hb_list.size(); ++i) {
                        if (hb_list[i]._id == stoi(node_id_str)) {
                            auto end = std::chrono::high_resolution_clock::now();
                            int64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(end - hb_list[i]._start).count(); 
                            // std::cout << "time: " << time << std::endl; //Uncomment to see time
                            
                            if (time > stoi(heartbit_time) * 4.032) {
                                std::cout << "Heartbit: node " + std::to_string(hb_list[i]._id) + " is unavailable now" << std::endl; //Uncomment to see time
                                hb_list[i]._start = end;
                                hb_list[i]._available = false;
                                break;
                            }
                            hb_list[i]._start = end;
                            if (hb_list[i]._available == false) {
                                hb_list[i]._available = true;
                                std::cout << "Heartbit: node " + std::to_string(hb_list[i]._id) + " is available now" << std::endl;
                            }
                            break;
                        }
                    }
                    heartbit(channel, node_id_map, stoi(node_id_str), hb_list, stoi(heartbit_time));
                    std::this_thread::sleep_for(std::chrono::milliseconds(stoi(heartbit_time)));

                }
            }            
        }    
    );
    handler.loop();
    th1.join();
    return 0;
}


