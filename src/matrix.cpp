#include "global.h"
#include <iostream>
#include <filesystem>
// #include "matrix.h"
namespace fs = std::filesystem;
/**
 * @brief Construct a new Table:: Table object
 *
 */
Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
    logger.log("chandu hello");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param matrixName
 * @param columns
//  */
// Table::Table(string tableName, vector<string> columns)
// {
//     logger.log("Table::Table");
//     this->sourceFileName = "../data/temp/" + tableName + ".csv";
//     this->tableName = tableName;
//     this->columns = columns;
//     this->columnCount = columns.size();
//     this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
//     this->writeRow<string>(columns);
// }

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded
 * @return false if an error occurred
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file.
 *
 * @param line
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Matrix::extractColumnNames(string firstLine)
{
    logger.log("Matrix::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    logger.log(s.str());
    while (getline(s, word, ','))
    {
        // word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        // if (columnNames.count(word))
        //     return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->rowCount = this->columnCount;
    logger.log(to_string(this->columnCount)+ " "+to_string(this->rowCount));
    // we can change otherwise in future
    int value=(BLOCK_SIZE * 1000) / (sizeof(int) );
     this->maxRowsPerBlock = sqrt(value);
     this->maxColumnsPerBlock = sqrt(value);
     logger.log(to_string(this->maxRowsPerBlock)+ " sqrt "+to_string(this->maxColumnsPerBlock));
    return true;
}

void stats(int blocksRead,int blocksWritten,int blocksAccess)
{
    cout<<" Number of blocks read: "<<blocksRead<<endl;
    cout<<" Number of blocks written: "<<blocksWritten<<endl;
    cout<<" Number of blocks Access: "<<blocksAccess<<endl;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size.
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    // vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    unordered_set<int> dummy;
    dummy.clear();

    logger.log("in divideDataIntoPages");
    int block_i = 0;
    int block_j = 0;
    vector<int> row(columnCount, 0);
    // getline(fin, line);
    int pageCounter = 0;

    vector<vector<int>> rowsInPage(maxRowsPerBlock, row);
    int rowStart = 0;

    while (getline(fin, line))
    {
        stringstream s(line);
        logger.log(s.str());
        // logger.log("line read");
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            // logger.log((word));
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        // this->updateStatistics(row);

        if (pageCounter == maxRowsPerBlock)
        {
            for (int columnStart = 0; columnStart < this->columnCount; columnStart += this->maxColumnsPerBlock)
            {
                string pageName = "./temp/" + matrixName + "Page_" + to_string(block_i) + "_" + to_string(block_j);
                bufferManager.writePage(matrixName, block_i, block_j, rowsInPage, rowStart, columnStart, pageCounter, min((int)this->columnCount, columnStart + this->maxColumnsPerBlock));
                block_j += 1;
            }
            pageCounter = 0;
            block_i += 1;
            block_j = 0;
        }
    }
    if (pageCounter)
    {
        // cout<<"hello"<<endl;
        for (int columnStart = 0; columnStart < columnCount; columnStart += this->maxColumnsPerBlock)
        {
            string pageName = "./temp/" + matrixName + "Page" + to_string(block_i) + "" + to_string(block_j);
            // cout<<pageName<<endl;
            bufferManager.writePage(matrixName, block_i, block_j, rowsInPage, rowStart, columnStart, pageCounter, min((int)this->columnCount, columnStart + this->maxColumnsPerBlock));

            block_j += 1;
        }
        pageCounter = 0;
        block_i += 1;
        // block_j=0;
    }
    this->subMatrixColumnCount = block_j;
    this->subMatrixRowCount = block_i;
    int blocksWritten=block_j*block_i;
    int blocksRead=0;
    int blocksAccess=blocksRead+blocksWritten;
    stats(blocksRead,blocksWritten,blocksAccess);
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row
 */
void Matrix::updateStatistics(vector<int> row)
{
    this->rowCount++;
    // for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    // {
    //     if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
    //     {
    //         this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
    //         this->distinctValuesPerColumnCount[columnCounter]++;
    //     }
    // }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName
 * @return true
 * @return false
 */
// bool Matrix::isColumn(string columnName)
// {
//     logger.log("Table::isColumn");
//     for (auto col : this->columns)
//     {
//         if (col == columnName)
//         {
//             return true;
//         }
//     }
//     return false;
// }

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName
 * @param toColumnName
 */
// void Table::renameColumn(string fromColumnName, string toColumnName)
// {
//     logger.log("Table::renameColumn");
//     for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
//     {
//         if (columns[columnCounter] == fromColumnName)
//         {
//             columns[columnCounter] = toColumnName;
//             break;
//         }
//     }
//     return;
// }

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Matrix::print()
{
    int blocksRead=0;
    int blocksWritten=0;
    int blocksAccess=0;
    logger.log("Matrix::print");
    logger.log("RowCount= " + to_string(this->rowCount) + " ColumnCount= " + to_string(this->columnCount));
    uint count = min((long long)PRINT_COUNT, this->rowCount);
    logger.log(to_string(count) + " ");
    int pageRowAccess = ceil(float(count) / (this->maxRowsPerBlock));
    int pageColumnAccess = ceil(float(count) / (this->maxColumnsPerBlock));
    logger.log("pageRowAccess= " + to_string(pageRowAccess) + " pageColumnAccess= " + to_string(pageColumnAccess));
    vector<vector<int>> displayMatrix(count, vector<int>(count));
    logger.log("displayMatrix rows: " + to_string(displayMatrix.size()) + " columns :" + to_string(displayMatrix[0].size()));
    int rows = this->maxRowsPerBlock;
    int cols = this->maxColumnsPerBlock;
    for (int i = 0; i < pageRowAccess; i++)
    {

        for (int j = 0; j < pageColumnAccess; j++)
        {
            blocksRead+=1;
            if (i == pageRowAccess - 1)
            {
                rows = (this->rowCount) % this->maxColumnsPerBlock;
            }
            int column = this->maxColumnsPerBlock;
            int rowstart = i * (this->maxRowsPerBlock);
            if (j == pageColumnAccess - 1)
            {
                column = (this->columnCount) % this->maxColumnsPerBlock;
            }
            Cursor cursor(this->matrixName, i, j);
            logger.log("page information: " + to_string(cursor.page.rows.size()) + " " + to_string(cursor.page.rows[0].size()));
            int columnstart = j * (this->maxColumnsPerBlock);
            // int k=rows;
            logger.log("rows" + to_string(rows));
            int k = rowstart;
            while (rowstart != k + rows)
            {
                vector<int> values = cursor.getNext(i, j);
                logger.log("values size:" + to_string(values.size()));

                for (auto ele : values)
                {
                    logger.log("bounds checking -----> " + to_string(rowstart) + " " + to_string(columnstart) + " " + to_string(ele));
                    displayMatrix[rowstart][columnstart] = ele;
                    columnstart += 1;
                    if (columnstart == count)
                        break;
                }
                rowstart += 1;
                if (rowstart == count)
                    break;
                // columnstart=0;
                columnstart = j * (this->maxColumnsPerBlock);
            }

            logger.log("page = " + to_string(i) + "_" + to_string(j));
        }
    }
    for (auto v : displayMatrix)
    {
        this->writeRow(v, cout);
    }
    blocksAccess=blocksRead+blocksWritten;
    stats(blocksRead,blocksWritten,blocksAccess);
    // Cursor cursor00(this->matrixName, 0,0);
    // vector<int>row;
    // for (int rowCounter = 0; rowCounter < count; rowCounter++)
    // {
    //     row = cursor00.getNext(0,0);
    //     displayMatrix.push_back(row);
    //     //this->writeRow(row, cout);
    // }

    // for (auto i :displayMatrix){
    //      this->writeRow(i, cout);
    // }
}

/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor
 * @return vector<int>
 */
void Matrix::getNextPage(Cursor *cursor)
{
    logger.log("Matrix::getNext");

    if (cursor->pageIndex < this->blockCount - 1)
    {
        cursor->nextPage(cursor->pageIndex + 1);
    }
}

/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Matrix::makePermanent()
{
    int blocksRead=0;
    int blocksWritten=0;
    int blocksAccess=0;
    logger.log("Matrix::makePermanent");
    logger.log(to_string(this->columnCount));
    logger.log(this->sourceFileName);
    if (!this->isPermanent())
    {
        logger.log("deleting: " + this->sourceFileName);
        bufferManager.deleteFile(this->sourceFileName);
    }
    logger.log("matrix name :" + this->matrixName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);
    int rows = this->maxRowsPerBlock;
    int columns = this->maxColumnsPerBlock;
    int subMatrixRowCount = this->subMatrixRowCount;
    int subMatrixColumnCount = this->subMatrixColumnCount;
    for (int i = 0; i < subMatrixRowCount; i++)
    {
        if (i == subMatrixRowCount - 1)
            rows = this->rowCount % this->maxRowsPerBlock;
        columns = subMatrixColumnCount;
        vector<vector<int>> arr(rows, vector<int>(columnCount, 0));
        for (int j = 0; j < subMatrixColumnCount; j++)
        {

            if (j == subMatrixColumnCount - 1)
                columns = this->columnCount % this->maxColumnsPerBlock;
            Cursor cursor(this->matrixName, i, j);
            blocksRead+=1;
            int rowStart = 0;

            while (rowStart < rows)
            {
                int columnStart = j * this->maxColumnsPerBlock;
                for (auto elem : cursor.getNext(i, j))
                {
                    arr[rowStart][columnStart] = elem;
                    columnStart += 1;
                }
                rowStart += 1;
            }
        }

        for (int t = 0; t < rows; t++)
        {
            this->writeRow(arr[t], fout);
            // this->writeRow(arr[t],cout);
        }
    }
    fout.close();
    stats(blocksRead,blocksWritten,blocksRead+blocksWritten);
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    logger.log("HELLO" + this->sourceFileName);
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    logger.log("asdf");
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Matrix::unload()
{
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->matrixName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 *
 * @return Cursor
 */
Cursor Matrix::getCursor()
{
    logger.log("Matrix::getCursor");
    Cursor cursor(this->matrixName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 *
 * @param columnName
 * @return int
 */
// int Matrix::getColumnIndex(string columnName)
// {
//     logger.log("Table::getColumnIndex");
//     for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
//     {
//         if (this->columns[columnCounter] == columnName)
//             return columnCounter;
//     }
// }
void swapBlockNames(string page1, string page2)
{
    logger.log(page1 + " " + page2);
    // cout<<page1<<" "<<page2<<endl;
    const char *file1 = page1.c_str();
    const char *file2 = page2.c_str();
    const char *tempFile = "tempfile";

    // Rename file1 to a temporary name
    if (std::rename(file1, tempFile) == 0)
    {
        // Rename file2 to file1
        if (std::rename(file2, file1) == 0)
        {
            // Rename the temporary file to file2
            if (std::rename(tempFile, file2) == 0)
            {
                logger.log("File names swapped successfully.");
            }
            else
            {
                std::cerr << "Error renaming the temporary file to " << file2 << std::endl;
            }
        }
        else
        {
            std::cerr << "Error renaming " << file2 << " to " << file1 << std::endl;
        }
    }
    else
    {
        std::cerr << "Error renaming " << file1 << " to " << tempFile << std::endl;
    }
}

// new function members created
void blocksSwapping(string matrixName, int rowBlocks, int columnBlocks)
{
    logger.log("hello");
    // int rowBlocks=ceil((float)this->rowCount/15);
    // int columnBlocks=ceil((float)columnCount/15);
    for (int i = 0; i < rowBlocks; i++)
    {
        for (int j = i; j < columnBlocks; j++)
        {
            if (i == j)
                continue;
            else
            {
                string page1 = "../data/temp/" + matrixName + "_Page" + to_string(i) + "_" + to_string(j);
                string page2 = "../data/temp/" + matrixName + "_Page" + to_string(j) + "_" + to_string(i);
                swapBlockNames(page1, page2);
            }
        }
    }
}

bool performTransposeandStoreBack(string matrixName, string pageName, int rows, int columns, int block_i, int block_j,int& blocksRead,int& blocksWritten)
{
    ifstream fin(pageName, ios::in);
    logger.log("performTranspose :" + pageName);
    string line, word;
    vector<int> row(columns, 0);
    // getline(fin, line);
    int pageCounter = 0;

    vector<vector<int>> matrix;
    int transposedRows = columns;
    int transposedColumns = rows;
    vector<vector<int>> transposeMatrix(columns, vector<int>(rows, 0));
    Cursor cursor(matrixName, block_i, block_j);
    blocksRead+=1;
    int i = 0;
    while (i < rows)
    {
        matrix.push_back(cursor.getNext(block_i, block_j));
        i += 1;
    }
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            transposeMatrix[j][i] = matrix[i][j];
        }
    }
    // write the transpose of a matrix back to pages
    int rowStart = 0;
    int columnStart = 0;
    int rowEnd = transposedRows;
    int columnEnd = transposedColumns;
    logger.log("transposed page " + to_string(rowEnd) + " " + to_string(columnEnd));
    logger.log("Writing Transpose: " + matrixName + "_Page" + to_string(block_i) + "_" + to_string(block_j));
    blocksWritten+=1;
    bufferManager.writePage(matrixName, block_i, block_j, transposeMatrix, rowStart, columnStart, rowEnd, columnEnd);
    return true;
}

void Matrix::transposeMatrix(string matrixName)
{
    int blocksRead=0;
    int blocksWritten=0;
    int blocksAccess=0;
    // TO DO
    int rows = maxRowsPerBlock;
    int columns = maxColumnsPerBlock;
    int rowBlocks = ceil((float)rowCount / maxRowsPerBlock);
    int columnBlocks = ceil((float)columnCount / maxColumnsPerBlock);
    for (int i = 0; i < rowBlocks; i += 1)
    {
        if (i == rowBlocks - 1)
        {
            rows = rowCount % maxRowsPerBlock;
        }
        columns = maxColumnsPerBlock;
        for (int j = 0; j < columnBlocks; j += 1)
        {

            if (j == columnBlocks - 1)
            {
                columns = columnCount % maxColumnsPerBlock;
            }
            string pageName = "../data/temp/" + matrixName + "_Page" + to_string(i) + "_" + to_string(j);
            // cout<<rows<<" "<<columns<<endl;
            logger.log("Transpose: Accessing " + pageName);
            
            performTransposeandStoreBack(matrixName, pageName, rows, columns, i, j,blocksRead,blocksWritten);
        }

        // return ;
    }
    blocksSwapping(matrixName, rowBlocks, columnBlocks);
    blocksAccess=blocksRead+blocksWritten;
    stats(blocksRead,blocksWritten,blocksAccess);
}

void Matrix::renameMatrix(string renameFromMatrixName, string renameToMatrixName)
{
    int blocksRead=0;
    int blocksWritten=0;
    int blocksAccess=0;
    logger.log("Table::renameMatrix");
    logger.log(renameToMatrixName);
    Matrix *m = matrixCatalogue.matrices[renameFromMatrixName];
    m->sourceFileName = "../data/" + renameToMatrixName + ".csv";
    // delete matrixCatalogue.matrices[renameFromMatrixName];
    m->matrixName = renameToMatrixName;
    matrixCatalogue.matrices[renameToMatrixName] = m;
    // matrixCatalogue.matrices[renameFromMatrixName]=NULL;
    matrixCatalogue.matrices.erase(renameFromMatrixName);

    std::string folderPath = "../data/temp/";                  // Replace with the actual folder path
    std::string oldSubstring = renameFromMatrixName + "_Page"; // Substring to replace
    std::string newPrefix = renameToMatrixName + "_Page";      // New prefix

    for (const auto &entry : fs::directory_iterator(folderPath))
    {
        if (entry.is_regular_file())
        {
            std::string oldFileName = entry.path().filename().string();

            // Check if the oldFileName contains the oldSubstring
            size_t found = oldFileName.find(oldSubstring);
            if (found != std::string::npos)
            {
                // Replace the oldSubstring with the newPrefix
                std::string newFileName = oldFileName;
                newFileName.replace(found, oldSubstring.length(), newPrefix);

                // Rename the file
                fs::rename(entry, entry.path().parent_path() / newFileName);
                logger.log("Renamed: " + oldFileName + " -> " + newFileName + "\n");
            }
            else
            {
                logger.log("Not found " + entry.path().filename().string());
            }
        }
    }
    stats(blocksRead,blocksWritten,blocksRead+blocksWritten);
    return;
}

bool checkSymmetryForEachBlock(string matrixName, int block_i, int block_j, int rows, int columns,int& blocksRead,int& blocksWritten)
{
    // get submatrix block_i  block_j
    logger.log(to_string(rows) + " " + to_string(columns));
    string pageName = "../data/temp/" + matrixName + "_Page" + to_string(block_i) + "_" + to_string(block_j);
    logger.log("checkSymmetryForEachBlock :" + pageName);
    vector<vector<int>> matrix1;
    blocksRead+=1;
    Cursor cursor(matrixName, block_i, block_j);
    int i = 0;
    while (i < rows)
    {
        matrix1.push_back(cursor.getNext(block_i, block_j));
        i += 1;
    }
    if (block_i == block_j)
    {
        for (int i = 0; i < rows; i++)
        {
            for (int j = i; j < columns; j++)
            {
                if (matrix1[i][j] != matrix1[j][i])
                    return false;
            }
        }
        return true;
    }
    else
    {
        logger.log("other part");
        // get submatrix block_j block_i;
        int matrix_2_rows = columns;
        int matrix_2_columns = rows;
        vector<vector<int>> matrix2;
        Cursor cursor1(matrixName, block_j, block_i);
        blocksRead+=1;
        int j = 0;
        while (j < matrix_2_rows)
        {
            matrix2.push_back(cursor1.getNext(block_j, block_i));
            j += 1;
        }
        for (int i = 0; i < rows; i++)
        {
            for (int j = i; j < columns; j++)
            {
                if (matrix1[i][j] != matrix2[j][i])
                    return false;
            }
        }
        return true;
    }
    return false;
}
bool Matrix::checkSymmetry(string matrixName)
{
    int blocksRead=0;
    int blocksWritten=0;
    int blocksAccess=0;
    int rowBlocks = ceil((float)rowCount / maxRowsPerBlock);
    int columnBlocks = ceil((float)columnCount / maxColumnsPerBlock);
    logger.log("no of " + to_string(rowBlocks) + " " + to_string(columnBlocks));
    int rows = maxRowsPerBlock;
    int columns = maxColumnsPerBlock;
    for (int i = 0; i < rowBlocks; i++)
    {
        if (i == rowBlocks - 1)
        {
            rows = rowCount % maxRowsPerBlock;
        }
        columns = maxColumnsPerBlock;
        for (int j = i; j < columnBlocks; j++)
        {
            if (j == columnBlocks - 1)
            {
                columns = columnCount % maxColumnsPerBlock;
            }
            if (checkSymmetryForEachBlock(matrixName, i, j, rows, columns,blocksRead,blocksWritten) == false)
            {
                stats(blocksRead,blocksWritten,blocksRead+blocksWritten);
                return false;
            }
        }
    }
    stats(blocksRead,blocksWritten,blocksRead+blocksWritten);
    return true;
}

vector<vector<int>> transposeMatrixFn(vector<vector<int>> &matrix, int value)
{
    int rows = matrix.size();
    int columns = matrix[0].size();
    vector<vector<int>> transposedMatrix(columns, vector<int>(rows, 0));
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < columns; j++)
        {
            transposedMatrix[j][i] = matrix[i][j] * value;
        }
    }
    return transposedMatrix;
}

bool computeForEachBlock(string matrixName, int block_i, int block_j, int rows, int columns,int& blocksRead,int& blocksWritten)
{
    logger.log(to_string(rows) + " " + to_string(columns));
    string pageName = "../data/temp/" + matrixName + "_Page" + to_string(block_i) + "_" + to_string(block_j);
    // ifstream fin(pageName ,ios::in);
    logger.log("computeForEachBlock :" + pageName);
    // string line,word;
    // vector<int> row(columns, 0);
    int pageCounter = 0;
    vector<vector<int>> matrix1;
    vector<vector<int>> transposedMatrix;
    vector<vector<int>> result;
    blocksRead+=1;
    Cursor cursor(matrixName, block_i, block_j);
    int i = 0;
    while (i < rows)
    {
        matrix1.push_back(cursor.getNext(block_i, block_j));
        i += 1;
    }
    if (block_i == block_j)
    {
        transposedMatrix = transposeMatrixFn(matrix1, 1);
        logger.log("fn comes here");
    }
    else
    {
        logger.log("other part");
        // get submatrix block_j block_i;
        int matrix_2_rows = columns;
        int matrix_2_columns = rows;
        int pageCounter = 0;
        vector<vector<int>> matrix2;
        int j = 0;
        blocksRead+=1;
        Cursor cursor1(matrixName, block_j, block_i);
        while (j < matrix_2_rows)
        {
            matrix2.push_back(cursor1.getNext(block_j, block_i));
            j += 1;
        }
        transposedMatrix = transposeMatrixFn(matrix2, 1);
    }
    for (int i = 0; i < rows; i++)
    {
        vector<int> temp;
        for (int j = 0; j < columns; j++)
        {
            int x = matrix1[i][j] - transposedMatrix[i][j];
            temp.push_back(x);
        }
        result.push_back(temp);
    }
    logger.log("comes here");
    int rowStart = 0;
    int columnStart = 0;
    int rowEnd = rows;
    int columnEnd = columns;
    string newMatrixName = matrixName + "_result";
    // writing block_i , block_j
    //  pageName = "../data/temp/"+newMatrixName + "_Page" + to_string(block_i)+""+to_string(block_j);
    blocksWritten+=1;
    bufferManager.writePage(newMatrixName, block_i, block_j, result, rowStart, columnStart, rowEnd, columnEnd);
    // writePage(result,pageName,rowStart,columnStart,rowEnd,columnEnd);
    if (block_i != block_j)
    {
        result = transposeMatrixFn(result, -1);
        rowStart = 0;
        columnStart = 0;
        rowEnd = columns;
        columnEnd = rows;
        // pageName = "./temp/"+newMatrixName + "Page" + to_string(block_j)+""+to_string(block_i);
        blocksWritten+=1;
        bufferManager.writePage(newMatrixName, block_j, block_i, result, rowStart, columnStart, rowEnd, columnEnd);
    }
}

void Matrix::compute(string matrixName)
{
    int blocksRead=0;
    int blocksWritten=0;
    int blocksAccess=0;
    logger.log("Compute Matrix : " + matrixName);
    int rowBlocks = ceil((float)rowCount / maxRowsPerBlock);
    int columnBlocks = ceil((float)columnCount / maxColumnsPerBlock);
    logger.log("no of " + to_string(rowBlocks) + " " + to_string(columnBlocks));
    int rows = maxRowsPerBlock;
    int columns = maxColumnsPerBlock;
    for (int i = 0; i < rowBlocks; i++)
    {
        if (i == rowBlocks - 1)
        {
            rows = rowCount % maxRowsPerBlock;
        }
        columns = maxColumnsPerBlock;
        for (int j = i; j < columnBlocks; j++)
        {
            if (j == columnBlocks - 1)
            {
                columns = columnCount % maxColumnsPerBlock;
            }
            computeForEachBlock(matrixName, i, j, rows, columns,blocksRead,blocksWritten);
        }
    }
    logger.log("until here worked");
    string newMatrixName = matrixName + "_result";
    Matrix *newMatrix = new Matrix(newMatrixName);
    newMatrix->rowCount = rowCount;
    newMatrix->columnCount = columnCount;
    newMatrix->subMatrixRowCount = rowBlocks;
    newMatrix->subMatrixColumnCount = columnBlocks;
    newMatrix->maxColumnsPerBlock = maxColumnsPerBlock;
    newMatrix->maxRowsPerBlock = maxRowsPerBlock;
    newMatrix->matrixName = newMatrixName;
    matrixCatalogue.insertMatrix(newMatrix);
    stats(blocksRead,blocksWritten,blocksRead+blocksWritten);
}
