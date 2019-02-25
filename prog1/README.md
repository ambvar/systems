# File Sorter
### Program Assignment 1 for Systems Programming Course

Assignment Description:

The customsort program will sort the words on each line in a file word-by-word in alphabetical order, then sort the resulting file line by line in alphabetical order. You may use any sorting algorithm you prefer so long as it completes in a reasonable time. You may assume each line ends with a '\n' character. You can safely ignore other line terminators, lines will always end with '\n'. You should sort the files using numerical order of the characters (i.e. based on their ASCII values). You can simply use the > or < operator on chars, to do so. Non-unique lines are to be removed from the output file.

Here's an example file:

```
A-word B-word C-word
D-word C-word 
B-word E-word 
C-word B-word A-word
```

The customsort program will output the following if given the above file as input:

```
A-word B-word C-word
B-word E-word
C-word D-word
```

After being sorted, the final line matches the first one, and is discarded since it's not unique.

There is no length limit on lines. Be attentive to this when reading lines into memory. You may use any number of temporary files to store your data as long as you remove them when your program terminates if you wish to program using files.
