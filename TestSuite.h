#ifndef TESTSUITE_CLASS
#define TESTSUITE_CLASS

#include "Includes.h"
#include "Database.h"
#include "InputBuffer.h"
#include "Commands.h"

class TestSuite {
private:

    void RunTests(const vector<string>& commands, vector<string>& feedback) {
        Table* table = initTable();

        for (size_t i = 0; i < commands.size(); ++i) {
            string command = commands[i];
            cout << "Test case " << i + 1 << ": \"" << command << "\"\n";

            // Redirect cout to a stringstream
            stringstream buffer;
            streambuf* oldCoutBuffer = cout.rdbuf(buffer.rdbuf());

            InputBuffer inputBuffer;
            inputBuffer.ReadInput(command);

            ExecuteResult res = NONE;
            switch (inputBuffer.returnStatementType()) {
            case SELECT_STATEMENT:
                res = Select(inputBuffer, table);
                break;
            case INSERT_STATEMENT:
                res = Insert(inputBuffer, table);
                break;
            }

            switch (res) {
            case EXECUTE_TABLE_FULL:
                cout << "ERR: The table is full." << endl;
                break;
            case EXECUTE_SUCCESS:
                cout << "Statement executed successfully." << endl;
                break;
            case EXECUTE_FAILURE:
                cout << "Statement failed to execute." << endl;
                break;
            case EXECUTE_TABLE_EMPTY:
                cout << "INFO: The table you are attempting to retrieve from is empty." << endl;
                break;
            }

            // Restore original cout buffer
            cout.rdbuf(oldCoutBuffer);

            // Store feedback
            feedback.push_back(buffer.str());
        }

        freeTable(table);
    }
    void RunTestsDB(const vector<string>& commands, vector<string>& feedback) {

        string filename = "testsuitedb";

        long i = 0;
        while (true) {
            std::ifstream file(filename);
            if (file) {
                filename += i;
                i++;
            }
            else {
                break;
            }
        }

        Table* table = initDB("testsuitedb");
        feedback.clear();

        for (size_t i = 0; i < commands.size(); ++i) {
            string command = commands[i];
            cout << "Test case (DB)" << i + 1 << ": \"" << command << "\"\n";

            // Redirect cout to a stringstream
            stringstream buffer;
            streambuf* oldCoutBuffer = cout.rdbuf(buffer.rdbuf());

            InputBuffer inputBuffer;
            inputBuffer.ReadInput(command);

            ExecuteResult res = NONE;
            switch (inputBuffer.returnStatementType()) {
            case SELECT_STATEMENT:
                res = SelectDB(inputBuffer, table);
                break;
            case INSERT_STATEMENT:
                res = InsertDB(inputBuffer, table);
                break;
            }

            switch (res) {
            case EXECUTE_TABLE_FULL:
                cout << "ERR: The table is full." << endl;
                break;
            case EXECUTE_SUCCESS:
                cout << "Statement executed successfully." << endl;
                break;
            case EXECUTE_FAILURE:
                cout << "Statement failed to execute." << endl;
                break;
            case EXECUTE_TABLE_EMPTY:
                cout << "INFO: The table you are attempting to retrieve from is empty." << endl;
                break;
            }

            // Restore original cout buffer
            cout.rdbuf(oldCoutBuffer);

            // Store feedback
            feedback.push_back(buffer.str());
        }

        closeDB(table);
    }

    void CalculateSuccessRate(const vector<string>& feedback) {
        cout << "\n";

        size_t totalExecutions = feedback.size();
        size_t successfulExecutions = 0;

        const vector<string> correctFeedback = {
            "Your command cannot contain empty spaces. \n",
            "ERR: The command you are trying to execute does not exist. \n",
            "Your command cannot contain empty spaces. \n",
            "Your command cannot contain empty spaces. \n",
            "Did you try .test?\n",
            "ERR: The command you are trying to execute does not exist. \n",
            "INFO: The table you are attempting to retrieve from is empty.\n",
            "Statement failed to execute.\n",
            "Statement failed to execute.\n",
            "Statement executed successfully.\n",
            "Number of rows in table: 1\nID: 1\nName: John\nEmail: JohnAppleseed@test.com\n-----\nStatement executed successfully.\n" };

        for (size_t i = 0; i < totalExecutions; ++i) {
            if (correctFeedback[i] == feedback[i]) {
                cout << "Test case " << i + 1 << " successful" << endl;
                successfulExecutions++;
            }
            else {
                cout << "Test case " << i + 1 << " failed" << endl;
            }
        }

        cout << "Success rate is " << successfulExecutions << "/" << totalExecutions << " (" << (successfulExecutions * 100.0 / totalExecutions) << "%)\n" << endl;
    }


public:
    void Run() {
        const vector<string> commands = {
            "", // should return "Your command cannot contain empty spaces."
            ".", // should return "ERR: The command you are trying to execute does not exist."
            " ", // should return "Your command cannot contain empty spaces."
            "   ", // should return "Your command cannot contain empty spaces."
            "test", // should return "Did you try .test?"
            ".test", // should return "ERR: The command you are trying to execute does not exist."
            ".select", // should return "INFO: The table you are attempting to retrieve from is empty."
            ".insert", // should return "Failure: Statement failed to execute."
            ".insert joo 2 t@test.com", // should return "Failure: Statement failed to execute."
            ".insert 1 John JohnAppleseed@test.com", // should return "Success: Statement executed successfully."
            ".select" // should return "ID: 1\nName: John\nEmail: JohnAppleseed@test.com\n-----\n"
        };

        vector<string> feedback;

        RunTests(commands, feedback);
        CalculateSuccessRate(feedback);

        RunTestsDB(commands, feedback);
        CalculateSuccessRate(feedback);

    }
};

#endif
