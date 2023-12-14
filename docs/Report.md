## TEAM -8 : Project Phase 1

### Assumptions
* matrix is square matrix
* only integer value
* comma separated csv
* csv file should not contain extra line or empty line.

<hr>

### Commands

#### LOAD

``` SYNTAX: LOAD MATRIX <MATRIX NAME>```

#### Implementation Detail:
<br>
Loading of matrix is done in form of submatrices of size k*k such that k<n
each submatrix is considered as a page.
value of k i\.e\. maximum rows or columns is determined on the basis of block size\.
<br>
For eg: block size =1, k=15

Each submatrix is loaded into pages in temp folder named with suffix of submatrix rowIndex, columnindex.
<br>
For eg: matrix\_Page0\_0

#### Page Design:
* Each page consist of numbers in the form  of matrix.
For eg: Matrix size is 100\* 100
Block Size=1
* No of rows and columns in any page=15
so Page0\_0 will contain 15\*15 matrix 
like wise Page0\_1,Page0\_2 and so on last page will have 15\*10 matrix.
* Subsequent rows will start from Page1\_0 and so on .
Like wise the last submatices will have 10\*15 and so on with last matrix as 10\*10.

#### No of block access:
- Input size: 50*50
- Block Size: 1
- No of block access: 
- Number of blocks read: 0
- Number of blocks written: 16
- Number of blocks Access: 16


#### Error Handling:
- Before executing load command ,it is checked for syntax and semantics.
- Syntax of command is checked if any issue prompt with error message , if it is already loaded and load command is used again for that it will throw an error.

- Semantics of matrix is checked whether this matrix exists or not if not throw an error else its ready for execution.

<hr>

#### TRANSPOSE
```SYNTAX: TRANSPOSE MATRIX <MATRIX NAME>```

#### Implementation detail:
- Input matrix on loading is already divided into pages.
- we have maintain the count of submatrices formed during loading
- Each page is having suffix (i,j) in file name.
- Iterate the pages, Transpose the content of each page.
-  Go through all page names for that matrix if i<>j in page suffix rename page with suffix(i,j) to (j,i).
- Transpose is IN-PLACE operation on transpose it will change the content of pages itself.

#### Page Design:
Page design is same as that of explain in LOAD command.
#### No of block access:
- Input size: 50*50
- Block Size: 1
- Number of blocks read: 16
- Number of blocks written: 16
- Number of blocks Access: 32
#### Error Handling:
- Before executing transpose command ,it is checked for syntax and semantics.
- Syntax of command is checked if any issue prompt with error message , if it is already loaded and load command is used again for that it will throw an error.

- Semantics of matrix is checked whether this matrix exists or not if not throw an error else its ready for execution.
<hr>


#### CHECKSYMMETRY
```SYNTAX : CHECKSYMMETRY <MATRIX NAME>```
#### Implementation detail:
- To Check the symmetry we have to check whether the matrix is equivalent to its transpose or not.
- Iterate throgh the pages created on Loading.
- Each page having suffix (i,j).
- If i==j: check symmetry within the page itself.
- if i<>j: content of (i,j) is equal to content of(j,i).
- anytime we got mismatch in symmetry of page content return false.
- return true if matrix passes all checks.
#### Page Design:
Used the same Page design implemented during LOAD Command.
#### No of block access:
- Input size: 50*50
- Block Size: 1
-  ## For not symmetric:
-  Number of blocks read: 1
- Number of blocks written: 0
- Number of blocks Access: 1
- ## For Symmetric:
- Number of blocks read: 16
- Number of blocks written: 0
- Number of blocks Access: 16
#### Error Handling:
- Before executing checksymmetry command ,it is checked for syntax and semantics.
- Syntax of command is checked if any issue prompt with error message , if it is already loaded and load command is used again for that it will throw an error.

- Semantics of matrix is checked whether this matrix exists or not if not throw an error else its ready for execution.

#### COMPUTE
```SYNTAX : COMPUTE <MATRIX NAME>```
#### Implementation detail:
- Matrix is loaded in pages, pages having suffix(i,j).
- iterate through pages :
1. if i==j: compute difference between the page and its transpose and store it in page named:(matrix\_result\_Page(i_j))
2. if i<>j: take two pages with suffix(i,j) and (j,i).
Transpose (i,j) page content compute the difference and store it in (i,j).
for page with(j,i) store the computed difference transpose and negate the values.
3. store it in result page.

#### Page Design:
Used the same Page design implemented during LOAD Command.
#### No of block access:
- Input size: 50*50
- Block Size: 1
- Number of blocks read: 16
- Number of blocks written: 16
- Number of blocks Access: 32
#### Error Handling:
- Before executing compute command ,it is checked for syntax and semantics.
- Synatx of command is checked if any issue prompt with error message , if it is already loaded and load command is used again for that it will throw an error.

- Semantics of matrix is checked whether this matrix exists or not if not throw an error else its ready for execution.
<hr>

### PRINT

```SYNTAX: PRINT MATRIX <MATRIX NAME>```

#### Implementation Detail:
- It prints a total of first 20 rows along with first 20 columns if total matrix rows and/or collumns exceeds the value of 20. In case the value is less than 20, it will print all the rows and/or columns respectively.

- We maintain two variables pageRowAccess, and pageColumnAccess which dynamically gets the no. of submatrix pages we need to access row-wise and column-wise to get atleast 20 rows and 20 columns to print.
Eg. A_Page0_0 : 1st Row and 1st Column of SubMatrix Page
Eg. A_Page0_1 : 1st Row and 2nd Column of SubMatrix Page
Eg. A_Page1_0 : 2nd Row and 1st Column of SubMatrix Page

- We also maintain a 2D array where we keep a track of elements while we are iterating over the submatrix pages, after which we print the required 20X20 matrix on terminal.

#### Number Of Block Access:
- Input size: 50*50
- Block Size: 1
- Number of blocks read: 4
- Number of blocks written: 0
- Number of blocks Access: 4

#### Error Handling:

- Syntatic Parser will check for the command correctness like whether length of whole command is equal to 3. If its true it will set the relationName and queryType and move to Semantic Parser.

- Semantic Parser will check whether the matrix with given matrixName exist in the Matrix Catalogue (matrices map maintained for all matrix that we load). If true, then it will move to execute_print function.

<hr>

#### RENAME
```SYNTAX: RENAME MATRIX <OLD MATRIX NAME> <NEW MATRIX NAME>```

#### Implementation Detail:

- It will first get the object to matrix which has to be renamed, and then change its matrixName to new matrix name.
- Then it will assign a new entry in the matrixCatalogue (matrices map) with the name of new matrix.
- It will further erase the old matrix name entry from the same matrixCatalgoue map.
- Next it will iterate over all the entries of pages in the temp folder and rename the name of pages which contains the substring "old_matrix_name" to "new_matrix_name".


#### Number Of Block Access:
- Input size: 50*50
- Block Size: 1
- Number of blocks read: 0
- Number of blocks written: 0
- Number of blocks Access: 0

#### Error Handling:

- Syntatic Parser will check for the command correctness like whether length of whole command is equal to 4. If its true it will set the oldrelationName, newrelationName and queryType and move to Semantic Parser.

- Semantic Parser will check whether the matrix with given oldrelationName exist and newrelationName does not exist in the Matrix Catalogue (matrices map maintained for all matrix that we load). If true, then it will move to execute_rename function.
</p>


#### EXPORT

```SYNTAX: EXPORT MATRIX <MATRIX NAME>```

#### Implementation Detail:

- We first verify whether the file with given fileName does not exist in the data dirctory. If it does, we delete the file and create a new file with same name.
- Next we create a new file with same matrixName in the data directory. We iterate over all the subMatrices row and collumn wise and store them accordingly, in a 2D array. We further write these rows of 2D array into the created file.


#### Number Of Block Access:
- Input size: 50*50
- Block Size: 1
- Number of blocks read: 16
- Number of blocks written: 0
- Number of blocks Access: 16

#### Error Handling:

- Syntatic Parser will check for the command correctness like whether length of whole command is equal to 3. If its true it will set the exportrelationName and queryType and move to Semantic Parser.

- Semantic Parser will check whether the matrix with given exportrelationName exist in the Matrix Catalogue (matrices map maintained for all matrix that we load). If true, then it will move to execute_export function.

### Learnings
-   Deal with large data
-   Understanding operations in terms of paging and how data storing       mechanism affects computation
-   Performing operations (like transpose, checksymmetry etc..) efficiently
- Good code writing practices

### Contribution
-   Worked in team equally
-   Everyone participated in Discussions, Coding work, Report


<hr>

# Project Phase - 2

## Assumptions
* Queries are on Relation
* only integer value
* comma separated csv
* csv file should not contain extra line or empty line.
* row size is smaller than block size
* Avg will always results in integer e.g. 60.12 will be considered as 60.

<hr>

### Commands

### SORT BY

``` SORT <table_name> BY <column_name1, column_name2,..., column_namek> IN <ASC|DESC, ASC|DESC,..., ASC|DESC> ```

#### Implementation Detail:
* Preparing metadata required for sort by query in parsing step like sorting column vector which contain pair of columnIndex and sorting startegy.
*  Using this information we are carrying out our internal sort using custom sort function.
* Finding the no of pages in the table took for loading, divide it into 9 partitions of equal length mostly and perform the external sort on each of the partition do it recursively until the partition contain single page.
* When the partition is having single page, load the page and perform internal sorting. for internal sorting we have prepared a vector containing columnIndex and Sorting order , 0 for Ascending  and 1 for descending.
* Now we have sorted single pages, now we need to perform the merge operation.
* In the merge operation, get the first row of each partition and keep them in a priority queue which will maintain rows in the sorting order provided in the query.
* get the top row from the priority queue and write it into the temporary page, now insert next row from the partition whose row has been popped up.
* perform these operations till all the rows in the partitions are read. 
* Copy all these temporary pages into the original pages used in the partition in the sequence order.
* Delete the temporary pages.
* Back track the merge operation until we get all blocks merged.
* This will result in the sorted table in place.


### Error Handling:

- Before executing sort by command ,it is checked for syntax and semantics.
- Syntax of command is checked if any issue prompt with error message , placement of keyword "By", "IN" ,no sorting startegy apart from ascending and descending (ASC |DESC )
- Semantics like whether the table is loaded or not,Column provided in the query exists in the table or not.

### JOIN

``` SYNTAX: <new_relation_name> <- JOIN <tablename1>, <tablename2> ON <column1> <bin_op> <column2> ```

#### Implementation Detail:
* The implementation starts with sorting the table1 and table2 based on their respective columns column1, and column2. After which a new resultTable is created with all columns, both from table1 and table2 in the order of table1, then table2.
* We then create two cursor objects which will fetch us new records in a sequence. We now start a while loop till both cursors do not return empty vectors(records). 
* Here we make use of custom made function which will take three arguments, values of respective 2 columns from join condition, and binary operator, and will return either 0,1, or -1; Here 0 means matching tuples for both tables, 1 means increment cursor1 for table1, and similarly -1 means increment cursor2 for table2. Inside the while loop, for each iteration, we check for the value returned by our custom function, and increment the cursors.
* If value return is 0, we first output/write the matching tuple in result table, and then create two temp cursor objects pointing to (cursor_temp1)cursor1 and (cursor_temp2)cursor2.
* For record pointed to by cursor1, we check for potential matching tuples for cursor_temp2, output/write it to result table, and break the search if match is not found. Similarly we will check for potential tuples from table1 with respect to record of table2 pointed by cursor2.
* At the end we will increment both actual cursors, cursor1 and cursor2. Finally the result table will have the join product from table1 and table2.


#### Error Handling:

* Syntatic Parser will check for the command correctness like whether length of whole command is equal to 8. If its true it will set the relationNames, queryType, binary_operator, and column_names and move to Semantic Parser.

* Semantic Parser will check whether the matrix with given matrixName exist in the Matrix Catalogue (matrices map maintained for all matrix that we load). It will further chcek whether the two ci=olumn names given in join condition exists. It will verify for the correctness of binary_operator. Also it will verify that the resulTable name does not exist. If true, then it will move to execute_join function.
</p>

<hr>

### GROUP BY

``` SYNTAX: <new_table> <- GROUP BY <grouping_attribute> FROM <table_name> HAVING <aggregate(attribute)> <bin_op> <attribute_value> RETURN <aggregate_func(attribute)> ```

#### Implementation Detail:
<br>

* Do the semantic and syntactic check over the query entered by user.
* If parsing done successfully prepare the metadata for query while parsing itself like group by columnName, aggregate columnName, aggregate function, return columnName, Return Function, Binary operator,tableName etc.
* Loading of the table is Done by counting maximum rows per block on given block count.
* After all the metadata preparation and parsing we have prepared the temporary table containg only three columns GroupBy, Aggregate, Return column.
* Sort the temporary file prepared in above step: advantage is we are ignoring unnecessary columns while will reduce the complexity.
* After sorting iterate the file pages and prepare the Min,Max,Count,Avg,Sum on the go for return and group by column based on the binary operator. 
* Update the values till we are getting same value for group by column if new value came up write the previous values in the resultant table page and perform the operation on next.


#### Error Handling:
- Before executing order by command ,it is checked for syntax and semantics.
- Syntax of command is checked if any issue prompt with error message , placement of keyword "By", "Having","Return","From" etc. is checked, Binary operator also
- Semantics like whether the table is loaded or not, Resultant Table already exist, columnName exists in the given table or not.

<hr>


#### ORDER BY

``` SYNTAX: <new_table> <- ORDER BY <attribute> ASC|DESC ON <table_name>```

#### Implementation Detail:
<br>

* Do the semantic and syntactic check over the query entered by user.
* If parsing done successfully prepare the metadata for query while parsing itself like orderby columnName, the tableName etc.
* Loading of the table is Done by counting maximum rows per block on given block count.
* After Loading the data into pages and preparing the metadata for tableCatalogue since sort function is for sorting pages inplace and result of orderBy operation should be in resultantTable,we have copied the original table into resultant table and call the external sort function implemented in task 1 over the resultant table using the column name and sorting order provided in the query.

#### Error Handling:
- Before executing order by command ,it is checked for syntax and semantics.
- Syntax of command is checked if any issue prompt with error message , placement of keyword "ON" is checked, Sorting order is alsochecked while syntactic parsing.

- Semantics like whether the table is loaded or not, Resultant Table already exist, columnName exists in the given table or not.

### Learnings
-   Deal with large data
-   Understanding operations in terms of paging and how data storing  mechanism affects computation
-   Performing operations (like extenal sort, group by,join,order by etc..) efficiently
-   Good code writing practices

### Contribution
-   Worked in team equally
-   Everyone participated in Discussions, Coding work, Report
