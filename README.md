# C-text-formatter

A 70s throwback. A simple text formatting tool based on fixed-width terminal fonts and character-based output.

The program:
- reads text input from stdin
- discards contents of a line beginning with period char
- replaces instances of multiple whitespace char with single blanks
- re-inserts newline characters line-by-line so the the output line has a wrap of 50 chars. i.e. each line is as long as possible but not longer than 50 chars.
  - except for a word that's longer than 50 chars - that can go on its own output line longer than 50 chars

# Commands
• .b– break the current line, so that the next input token starts at the beginning of the next line

• .p– leave a blank line and start a new paragraph without altering the margins, with the next input token starting the new paragraph

• .lnn – alter the left margin from its current value (default initial value of 4) to the new value nn, and start a new paragraph
• .w nn – alter the width of each line from its current value (default initial value of 50) to the new
value nn, and start a new paragraph.
• .c – take the remaining contents of this line and center them within the current output width. If
the remaining contents cannot ﬁt within the current output width, then it should be placed so that
it starts at the left margin and overﬂows beyond the current width. When there is an odd number
of spaces to be assigned, the rounded-down half should be at the beginning of the line, and the
rounded-up half at the end of the line.
• .h nn – take the remainder of the contents of this line and use them as a section heading at the
level indicated by nn. The Section/subsection/subsubsection number should be printed, and then
the complete heading on a single line, even if it is longer that the current width. Headings at level
nn reset the numbering for headings level nn + 1, nn + 2, and so on; and (to avoid non-computing
people from being confused) all headings start their counting from one. At most ﬁve levels of
headings may occur, counting from nn = 1 (the top-level heading) to nn = 5. All headings are
always preceded and followed by a paragraph boundary, and level-1 headings are also preceded
by a full-width line of “-” characters.
