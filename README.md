# COSC431_Assignment1
Compile parser.c with:
gcc parser.c -o parser

Compile indexer.c with:
gcc indexer.c -o indexer

Compile searcher.c with:
gcc searcher.c -o searcher

Run parser while in working directory with:
./parser < wsj.xml

Run indexer while in working directory with:
./parser < wsj.xml | ./indexer
OR
./parser < wsj.xml > result
./indexer < result

Indexer will make "IDlist", "index_size", and "index" files.

Run searcher while in working directory with:
./searcher {filePath} 

where filePath is location of the directory storing the indexer's files (index, IDlist, index_size)
For example if the indexer's files are in the same directory, use:
./searcher . 

or if they are in the index folder in current working directory:
./searcher ./index

or an absolute filepath

./searcher /mnt/c/Users/Bobst/Downloads

--DO NOT INCLUDE A TRAILING SLASH

you can input queries on stdin or input a file with:
./searcher {filepath} < {queryFile}
e.g.
./searcher . < testData
