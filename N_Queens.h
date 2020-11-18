#include<vector>
#include<iostream>

using namespace std;

class NQueen{
private:
    vector<vector<char>> board;
    vector<bool> colAvail;
    vector<bool> leftDiagAvail;
    vector<bool> rightDiagAvail;
    unsigned solCount;
    
    void printBoard(){
        for(const vector<char>& eachRow : this->board){
            for(const char eachCol : eachRow){
                cout << eachCol << ' ';
            }
            cout << '\n';
        }
        cout << '\n';
    }
    
    void insertNewQueen(int currRow){
        // basecase
        if(currRow == static_cast<int>(this->board.size())) {
            ++this->solCount;
            this->printBoard();
        }
        else{
            for(int col = 0; col < static_cast<int>(this->board.size()); ++col){
                // only put the queen in this column if it doesn't violate the constraint
                if(this->promising(currRow, col)){
                    // update three vector before put the queen in the current row
                    this->colAvail[col] = false;
                    this->leftDiagAvail[col + currRow] = false;
                    this->rightDiagAvail[currRow - col + this->board.size() - 1] = false;
                    this->board[currRow][col] = 'X';
                    // try to insert queen in the next row
                    this->insertNewQueen(currRow + 1);
                    this->board[currRow][col] = 'O';
                    //after this try, recover three vector
                    this->colAvail[col] = true;
                    this->leftDiagAvail[col + currRow] = true;
                    this->rightDiagAvail[currRow - col + this->board.size() - 1] = true;
                }
            }
        }
    }
    
    bool promising(int row, int col){
        return (this->colAvail[col] && 
            this->leftDiagAvail[row + col] &&  
            this->rightDiagAvail[row - col + this->board.size() - 1]);
    }

public:
    NQueen(unsigned int n):solCount(0){
        this->board.reserve(n);
        for(unsigned i = 0; i < n; ++i){
            this->board.push_back(vector<char>(n, 'O'));
            this->colAvail.push_back(true);
        }
        for(unsigned i = 0; i < 2 * n - 1; ++i){
            this->leftDiagAvail.push_back(true);
            this->rightDiagAvail.push_back(true);
        }
    }
    
    void printSolution(){
        this->insertNewQueen(0);
        cout << "total " << this->solCount << " solutions\n";
    }
};