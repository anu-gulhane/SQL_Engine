#include "global.h"
/**
 * @brief 
 * SYNTAX: SOURCE filename
 */
bool syntacticParseSOURCE()
{
    logger.log("syntacticParseSOURCE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = SOURCE;
    parsedQuery.sourceFileName = tokenizedQuery[1];
    return true;
}

bool semanticParseSOURCE()
{
    logger.log("semanticParseSOURCE");
    if (!isQueryFile(parsedQuery.sourceFileName))
    {
        cout << "SEMANTIC ERROR: File doesn't exist" << endl;
        return false;
    }
    return true;
}
void doCommand1()
{
    logger.log("doCommand");
    if (syntacticParse() && semanticParse())
        executeCommand();
    return;
}
int executeQuery(string command)
{
    tokenizedQuery.clear();
    parsedQuery.clear();
    logger.log("\nReading New Command: ");
    // getline(cin, command);
    logger.log(command);

    regex delim("[^\\s,]+");
    auto words_begin = std::sregex_iterator(command.begin(), command.end(), delim);
    auto words_end = std::sregex_iterator();
    for (std::sregex_iterator i = words_begin; i != words_end; ++i)
        tokenizedQuery.emplace_back((*i).str());

    if (tokenizedQuery.size() == 1 && tokenizedQuery.front() == "QUIT")
    {
        return 1;
    }

    if (tokenizedQuery.empty())
    {
        return 0;
        // continue;
    }

    if (tokenizedQuery.size() == 1)
    {
        cout << "SYNTAX ERROR" << endl;
        // continue;
        return 0;
    }

    doCommand1();
    return 0;

}
void executeSOURCE()
{
    logger.log("executeSOURCE");
    // cout<<"source File Name "<<parsedQuery.sourceFileName<<endl;
    string fileName="../data/" + parsedQuery.sourceFileName + ".ra";
    // cout<<" fileName"<<fileName<<endl;
    ifstream inputFile(fileName);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open the file: " << fileName << std::endl;
        return ; // Return an error code
    }
    vector<string>queryLines;
    string line;
    while (std::getline(inputFile, line)) {
        queryLines.push_back(line);
        // cout << "Read line: " << line << std::endl;

        // You can process the line further here
    }
    // Close the file
    inputFile.close();
    for(auto eachQuery:queryLines)
    {
        if(executeQuery(eachQuery)==1)
            break;
        // cout<<each<<endl;
    }
    
    return;
}
