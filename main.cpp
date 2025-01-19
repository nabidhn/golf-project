/* Golf
 *
 * Desc:
 *   This program reads input data concerning golf statistics from
 * an input file, the lines of which are of the form:
 * location;club_name;player_name;score.
 * The program checks if each line consists of the abovementioned four
 * data fields.
 *   The data written in the file is collected into an appropriate data
 * structure consisting of STL containers and structs. After that the user
 * can make searches for the data structure by using the commands
 * places, clubs, rounds, played, and compare. These commands
 * have a varying number of parameters, and the program checks if the
 * user gave a correct number of parameters and if the given parameters
 * (place, club, player) can be found from the data structure.
 *   The program terminates with the command quit.
 *
 * Program author
 * Name: Nabid Hasan
 * Student number: 152754330
 * UserID: vmn388
 * E-Mail: nabid.hasan@tuni.fi
 *
 * Notes about the program and it's implementation (if any):
 *
 * */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>

using namespace std;

// structure to represent player's name and score
struct Round {
    string name;
    int score;
};

// map for storing golf data
using GolfData = map<string, map<string, vector<Round>>>;

//split function for splitting string
vector<string> split(const string& str, char delimiter) {
    vector<string> result;
    string current;
    bool inQuotes = false;
    for (char c : str) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == delimiter && !inQuotes) {
            result.push_back(current);
            current = "";
        } else {
            current += c;
        }
    }
    result.push_back(current);
    return result;
}

// trim function to remove spaces from a string
void trim(string &str) {
    str.erase(0, min(str.find_first_not_of(" "), str.length() - 1));
    str.erase(str.find_last_not_of(" ") + 1);
}

//a function to read data from a file and key value pairs for map
bool readFile(const string& filename, GolfData& data) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: The specified file cannot be opened!" << endl;
        return false;
    }

    string line;
    while (getline(file, line)) {
        vector<string> fields = split(line, ';');
        if (fields.size() != 4) {
            cerr << "Error: The specified file has an erroneous line!" << endl;
            return false;
        }

        for(const string& field : fields){
            if(field.empty()){
                cerr << "Error: A line has an empty value!" << endl;
                return false;
            }
        }

        string location = fields[0];
        string club = fields[1];
        string player = fields[2];
        int score = stoi(fields[3]);

        data[location][club].push_back({player, score});
    }
    return true;
}

// a function to display all locations and clubs
void h_Places(const GolfData& data) {
    for (const auto& locationPair : data) {
        cout << locationPair.first << endl;
        for (const auto& clubPair : locationPair.second) {
            cout << "--" << clubPair.first << endl;
        }
    }
}

// handleClubs function to show all clubs for a specified location
void hClubs(const GolfData& data, const string& location) {
    if (data.count(location) == 0) {
        cerr << "Error: The given location not found!" << endl;
        return;
    }
    for (const auto& clubPair : data.at(location)) {
        cout << "--" << clubPair.first << endl;
    }
}

//function to display rounds for each club(arranged by score)
void E_rounds(const GolfData& data, const string& club) {
    bool found = false;
    for (const auto& locationPair : data) {
        if (locationPair.second.count(club) > 0) {
            found = true;
            vector<Round> rounds = locationPair.second.at(club);
            sort(rounds.begin(), rounds.end(), [](const Round& a, const Round& b) {
                return a.score < b.score;
            });
            for (const auto& round : rounds) {
                cout << round.name << " : ";
                if (round.score > 0) {
                    cout << "+";
                }
                cout << round.score << endl;
            }
            break;
        }
    }
    if (!found) {
        cerr << "Error: The given club not found!" << endl;
    }
}

//a function that can calculate handicap based on score
double calculateHcp(const vector<int>& scores) {
    if (scores.size() < 3) {
        return 55.0;
    }

    vector<int> sortedScores = scores;
    sort(sortedScores.begin(), sortedScores.end());

    int numScoresToUse = min((int)scores.size(), 5);
    double sum = 0;
    for (int i = 0; i < numScoresToUse; ++i) {
        sum += sortedScores[i];
    }
    return sum / numScoresToUse;
}

//format HCP to display integers without decimal
string formatHCP(double hcp) {
    if (hcp == static_cast<int>(hcp)) {
        return to_string(static_cast<int>(hcp));
    } else {
        stringstream ss;
        ss << fixed << setprecision(1) << hcp;
        return ss.str();
    }
}

//function for displayingall rounds(wih handicap)
void r_Played(const GolfData& data, const string& player) {
    vector<pair<pair<string, string>, int>> playerRounds;



    for (const auto& locationPair : data) {
        for (const auto& clubPair : locationPair.second) {
            for (const auto& round : clubPair.second) {
                if (round.name == player) {
                    playerRounds.push_back({{locationPair.first, clubPair.first}, round.score});
                }
            }
        }
    }

    if (playerRounds.empty()) {
        cerr << "Error: The player hasn't played any rounds!" << endl;
        return;
    }

    sort(playerRounds.begin(), playerRounds.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) {
            return a.second < b.second;
        }
        return a.first.second < b.first.second;
    });

    vector<int> scores;
    for (const auto& round : playerRounds) {
        scores.push_back(round.second);
    }

    double hcp = calculateHcp(scores);
    std::string formatted_hcp = formatHCP(hcp); // Format HCP

    if (hcp == 55.0) {
        cout << player << " has too few rounds for a handicap:" << endl;
    } else {
        cout << player << " has a HCP of " << fixed << setprecision(1) << formatted_hcp << " with following results:" << endl;
    }

    for (const auto& round : playerRounds) {
        cout << round.first.first << " : " << round.first.second << " : ";
        if (round.second > 0) {
            cout << "+"; // adds + sign for positive scores
        }
        cout << round.second << endl;
    }
}

//function to comPare per formance of 2 player
void handleCompare(const GolfData& data, const string& player1, const string& player2) {
    auto getPlayerScores = [&](const string& player) {
        vector<int> scores;
        for (const auto& locationPair : data) {
            for (const auto& clubPair : locationPair.second) {
                for (const auto& round : clubPair.second) {
                    if (round.name == player) {
                        scores.push_back(round.score);
                    }
                }
            }
        }
        return scores;
    };

    vector<int> scores1 = getPlayerScores(player1);
    vector<int> scores2 = getPlayerScores(player2);

    double hcp1 = calculateHcp(scores1);
    double hcp2 = calculateHcp(scores2);
//format HCP to display integers without decimal
    string formatted_hcp1 = formatHCP(hcp1);
    string formatted_hcp2 = formatHCP(hcp2);


    if (scores1.empty()) {
        cout << player1 << " has played no rounds of golf" << endl;
    } else if (hcp1 == 55.0) {
        cout << player1 << " has played " << scores1.size() << " rounds of golf, but hasn't played enough for a handicap" << endl;
    } else {
        cout << player1 << " has played " << scores1.size() << " rounds of golf, with HCP of " << fixed << setprecision(1) << formatted_hcp1 << endl;
    }

    if (scores2.empty()) {
        cout << player2 << " has played no rounds of golf" << endl;
    } else if (hcp2 == 55.0) {
        cout << player2 << " has played " << scores2.size() << " rounds of golf, but hasn't played enough for a handicap" << endl;
    } else {
        cout << player2 << " has played " << scores2.size() << " rounds of golf, with HCP of " << fixed << setprecision(1) << formatted_hcp2 << endl;
    }

    if (scores1.empty() && scores2.empty()) {
        cout << "Either hasn't played enough golf" << endl;
    } else if (hcp1 == 55.0 && hcp2 == 55.0) {
        cout << "Either hasn't played enough golf" << endl;
    } else if (hcp1 == 55.0) {
        cout << player2 << " has played better golf" << endl;
    } else if (hcp2 == 55.0) {
        cout << player1 << " has played better golf" << endl;
    } else if (hcp1 < hcp2) {
        cout << player1 << " has played better golf" << endl;
    } else if (hcp2 < hcp1) {
        cout << player2 << " has played better golf" << endl;
    } else {
        cout << "Both have played as good golf" << endl;
    }
}

//main function for handing various user commands
int main() {

    string filename;
    cout << "Input file: ";
    cin >> filename;

    GolfData data;
    if (!readFile(filename, data)) {
        return EXIT_FAILURE;
    }

    string input;
    cout << "> ";
    getline(cin >> ws, input);

    while (input != "quit") {
        vector<string> commands = split(input, ' ');

        if (!commands.empty()) {
            string command = commands[0];

            if (command == "places") {
                if (commands.size() != 1) {
                    cerr << "Error: In command: places" << endl;
                } else {
                    h_Places(data);
                }
            }
            else if(command == "clubs") {
                if (commands.size() != 2) {
                    cerr << "Error: In command: clubs" << endl;
                } else {
                    hClubs(data, commands[1]);
                }
            }
            else if(command == "rounds") {
                if (commands.size() != 2) {
                    cerr << "Error: In command: rounds" << endl;
                } else {
                    E_rounds(data, commands[1]);
                }
            }
            else if(command == "played") {
                if (commands.size() != 2) {
                    cerr << "Error: In command: played" << endl;
                } else {
                    r_Played(data, commands[1]);
                }
            }
            else if(command == "compare") {
                if (commands.size() != 3) {
                    cerr << "Error: In command: compare" << endl;
                } else {
                    handleCompare(data, commands[1], commands[2]);
                }
            }
            else {
                cerr << "Error: Unknown command: " << command << endl;
            }
        }
        cout << "> ";
        getline(cin >> ws, input);
    }

    return EXIT_SUCCESS;
}

