#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <regex>
#include <cstdlib> 

using namespace std;

/* Menu for file selection */
string ChooseFileMenu(){
    char buffer[1024]; string result;
    FILE* fp = popen(
        "osascript -e 'POSIX path of (choose file of type {\"public.comma-separated-values-text\"} with prompt \"Select a CSV file\")'", 
        "r"
    );
    if (!fp) return "";

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        result += buffer;
    }
    pclose(fp);

    // Remove trailing newline
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

int main() {
    /* Store item to vector */
    int ncol, nrow; string col_status, line; vector<string> values;

    // getting user input
    string file_path = ChooseFileMenu(); ifstream file(file_path); 
    cout << "Enter number of column: "; cin >> ncol;
    cout << "Enter number of row: "; cin >> nrow;
    cout << "Your data sheet got column name? (y/n): "; cin >> col_status;
    if (col_status != "y" && col_status != "n") {
        cout << "[Error] Please make sure your answer is valid!";
        return 0;
    }
    while (getline(file, line)) {
        if (!line.empty()) {
            if (col_status == "y"){
                col_status = "0";
                continue;
            }
            values.push_back(line); // ignore col name
        }
    }
    if (values.size() != 1200) {
        cerr << "[Error] Expected 1200 values, but got " << values.size() << endl;
        cout << "[Hiint] Check your datasheet whether got column name or not." << endl;
        return 1;
    }
    
    /* Reshape */
    string reshaped[nrow][ncol]; int item_index = 0;

    for (int col = 0; col < ncol; ++col){
        for (int row = 0; row < nrow; ++row){
            reshaped[row][col] = values[item_index];
            item_index ++;
        }
    }

    /* Export to csv */
    int result = system("touch ~/Downloads/output.csv");
    string output_path = string(getenv("HOME")) + "/Downloads/output.csv";
    ofstream file_output(output_path); // open output path
    regex strip("[\r\n]+"); // regex for trim new line
    
    // Open csv
    if (result == 0) cout << "[Success] File created successfully!" << endl;
    else {
        cout << "[Error] Failed to create file." << endl;
        return 0;
    }

    // Start writing 
    if (file_output.is_open()){
        // write column name
        for (int col = 0; col < ncol; col++){
            file_output << regex_replace("Col_"+to_string(col),strip,"");
            if (col != ncol-1) file_output << ",";
            else file_output << "\n";
        } 
        // writting body
        for (int row = 0; row < nrow; row++){
            int count = 0;
            for (int col = 0; col < ncol; col++){
                file_output << regex_replace(reshaped[row][col],strip,"");
                if (col != ncol-1) file_output <<  ",";
            }
            file_output << "\n";
        }
        file_output.close();
        cout << "[Success] Result had stored on " << output_path << endl;
    }
    else cout << "[Error] reading the output.csv file" << endl;

    return 0;
}
