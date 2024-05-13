import subprocess

def RunTests(executable_path, command):

    process = subprocess.Popen(executable_path, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    process.stdin.write(command + '\n')
    process.stdin.close()
    output = process.stdout.read()

    return output

def CalculateSuccessRate(feedback):

    totalExecutions = len(feedback)
    successfulExecutions = 0

    correctFeedback = [
        "Your command cannot contain empty spaces.",
        "ERR: The command you are trying to execute does not exist.",
        "Your command cannot contain empty spaces.",
        "Your command cannot contain empty spaces.",
        "Did you try .test?",
        "ERR: The command you are trying to execute does not exist.",
        "INFO: The table you are attempting to retrieve from is empty.",
        "Statement failed to execute.",
        "Statement failed to execute.",
        "Statement executed successfully.",
        "ID: 1 Name: John Email: JohnAppleseed@test.com"
    ]

    for i in range(totalExecutions):
        if(correctFeedback[i] in feedback[i]):
            print(f"Test case {i+1} successful" )
            successfulExecutions += 1
        else:
            print(f"Test case {i+1} failed" ) 

    print(f"Success rate is {successfulExecutions}/{totalExecutions} ({round(successfulExecutions/totalExecutions * 100, 2)}%) \n{totalExecutions-successfulExecutions} test case(s) failed.")


if __name__ == "__main__":
    executable_path = "../x64/Debug/SQLite Based Relational Database.exe"

    commands = [
        "", # should return "Your command cannot contain empty spaces."
        ".", # should return "ERR: The command you are trying to execute does not exist."
        " " , # should return "Your command cannot contain empty spaces."
        "   " , # should return "Your command cannot contain empty spaces."
        "test", # should return "Did you try .test?"
        ".test", # should return "ERR: The command you are trying to execute does not exist."
        ".select", # should return "INFO: The table you are attempting to retrieve from is empty."
        ".insert", # should return "Statement failed to execute."
        ".insert joo 2 t@test.com", # should return "Statement failed to execute."
        ".insert 1 John JohnAppleseed@test.com", # should return "Statement executed successfully."
        ".select", # should return "ID: 1 Name: John Email: JohnAppleseed@test.com -----"
    ] 

    feedback = []

    for i in range(len(commands)):
        output = RunTests(executable_path, commands[i])
        feedback.append(output)

    CalculateSuccessRate(feedback)

