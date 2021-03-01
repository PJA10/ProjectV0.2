# ProjectV0.2 - mmn14

An assembler, a imaginary assembly compiler, created as part of a the open uni course project. By Alon Wartski and Roy Lenz. We got 100 on the project and my final score of the course is 96.

The compiler divides to 3 parts:
* The first pass on the machine language file
* The second pass which runs only on the lines that thier coding wasn't finished
* And finnaly preparing the output files

# First Pass
In the first pass we go over each line and start to code it from assembly to the memmory base in the wierd 32 bit(defind in the project instructions).
The First pass handles action commands, data commands, lables and more.

The first pass checks that every line is valid. If the line is valid, the compiler will split the line to a linked list of tokens. The compiler uses a 'commandLine' struct of the line to parse it.
If the line is a data command, the compiler will code the data into the data memory base, If the line is an extern command the compiler will add the label as extern label, If the line is an action command the compiler will code the only first memory word of the command(the second one needs to be coded after all the labels are know, in the second pass).
Additionally the compiler will add the 'commandLine' struct to the linked list of the actions and entry commands which is used by the second pass.

# Second Pass
In the second pass the compiler doesnt run on the original input file, it run on the linked list if 'commandLine's that was made in the first pass. The compiler code actions operands(the second word of an action command) and flag the labels with .entry command.

The function code actions operands and flag the labels with .entry command. The function runs after first pass and because our label table is now completed and we can code the labels that were mentioned.

# Prepare Files
Prepare the output files for the program. Object file with the machine code, extern File with every reference to an external label in the machine code And a entry file with declaration of every entry label.

