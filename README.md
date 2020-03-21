# C-text-formatter

A 70s throwback. A simple text formatting tool based on fixed-width terminal fonts and character-based output.

The program:
- reads text input from stdin
- discards contents of a line beginning with period char
- replaces instances of multiple whitespace char with single blanks
- re-inserts newline characters line-by-line so the the output line has a wrap of 50 chars. i.e. each line is as long as possible but not longer than 50 chars.
  - except for a word that's longer than 50 chars - that can go on its own output line longer than 50 chars

# Commands
Indicated by input lines that start with a period ".", command indicated with the next character. 

E.g. ".p" in the first two positions of a line signal the start of a new paragraph in the output.

