#include <iostream>
#include <vector>
#include <unordered_map>
#include <utility>
#include <regex>
#include <fstream>
#include <stack>
#include <set>

using namespace std;

// Regex Expressions.
const regex START_REGEX("# (\\w+):");
const regex END_REGEX("# ---");
enum class LineType{IF, FI, NONE};

LineType get_command(const string &line, string &target){
/* Classifies Dockerfile lines and extracts conditional features. 

    Args:
        - line: dockerfile line.
        - target: string to place command name if found.
    
    Ret: 
        - LineType: which kind of line was passed to this function.
                    It can be the start, end of a conditional statement
                    or an ordinary valid Dockerfile line.
*/
    smatch m;
    regex_search(line, m, START_REGEX);
    if(m.length()){
        target = m[1];
        return LineType::IF;
    }
    regex_search(line, m, END_REGEX);
    if(m.length())
        return LineType::FI;
    return LineType::NONE;
}


// Filenames.
constexpr char YML_FILENAME[] = "images.yml";
constexpr char RM_TMP_DOCKERFILE[] = "rm .dockif";

void docker_build(const string &tagname){
    string cmd = "docker build -t % -f .dockif .";
    cmd.replace(16, 1, tagname);
    cout << cmd << endl;
    system(cmd.c_str());
    system(RM_TMP_DOCKERFILE);
}

int main(void){
    fstream fin;
    // Commands.
    unordered_map<string,vector<pair<unsigned, unsigned>>> commands;
    // Filestream - retrieve commands.
    fin.open("Dockerfile", ios::in); 
    string line, cmd_name;
    stack<string> command_stack;
    unsigned counter{0};
    while(getline(fin, line)){
        switch(get_command(line, cmd_name)){
            case LineType::IF:
                commands[cmd_name].push_back({counter++, 0});
                command_stack.push(cmd_name);
                break;
            case LineType::FI:
                commands[command_stack.top()].back().second = counter++;
                command_stack.pop();
                break;
            case LineType::NONE:
                counter++;
        }
    }
    // Simple YAML Parsing. TODO(lorsi): Regex it!
    fstream yaml;
    yaml.open(YML_FILENAME, ios::in);
    string yaml_line, image_name;
    unordered_map<string, set<string>> image_map;
    while(getline(yaml, yaml_line)){
        auto index = yaml_line.find('-');
        if(index != string::npos)
            image_map[image_name].insert(yaml_line.substr(index+2));
        else
            image_name = yaml_line.substr(0, yaml_line.size()-1);
    }
    yaml.close();
    // Commands filtering.
    for(auto image: image_map){
        set<int> l2skip;
        for (auto const& cmd: commands) {
            if(image.second.count(cmd.first)){
                for(auto block: cmd.second){
                    l2skip.insert(block.first);
                    l2skip.insert(block.second);
                }
            } else {
                for(auto block: cmd.second)
                    for(unsigned j=block.first; j<=block.second; j++)
                        l2skip.insert(j);
            }
        }
        //Create dockerfile
        ofstream outdockfile;
        fin.clear();
        fin.seekg(0,std::ios::beg);
        outdockfile.open(".dockif");
        counter = 0;
        while(getline(fin, line)){
            if(!l2skip.count(counter))
                outdockfile << line << endl;
            counter++;        
        }
        fin.clear();
        outdockfile.close();
        docker_build(image.first);
    }
    fin.close();
}
