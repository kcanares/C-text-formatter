
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 999				/*max no. of char allowed in a line*/
#define DEFAULT_LEFT_MARGIN_LENGTH 4	/*max no. of char allowed in a line*/	
#define DEFAULT_WIDTH_MARGIN_LENGTH 50	/*default length of width margin */
#define HEADING_LEVELS 5				/*max no. of heading level allowed */
#define START 0							/*for indexing word_pos_t arrays*/
#define END 1							/*for indexing word_pos_t arrays*/
#define NON_CENTRE 0					/*for indexing command_count_t arrays*/
#define CENTRE 1						/*for indexing command_count_t arrays*/
#define HEADING_COMMAND_LEN 5			/*length of command: ".h nn "*/
#define SPACE_CHAR_LEN 1				/*length of a space char*/
#define CENTRE_COMMAND_LEN 2			/*length of command: ".c "*/

typedef char line_t[MAX_LINE_LENGTH + 1];

typedef int word_pos_t[2]; 		/* stores start (word_pos_t[0]) and end 
								(word_pos_t[1]) position of a word in a line */
								
typedef int command_count_t[2]; /* keeps track of consecutive non-centre 
								command counts command_count_t[0] and centre 
								command count command_count_t[1] */

/*****************************************************************************/
/* function prototypes */

int read_line(line_t line);
void non_command_processor(line_t line, int *current_line_len, 
							int *left_border_len, int *width_margin, 
							command_count_t command_count);
void print_line(line_t line, int *current_line_len, int *left_border_len, 
				int *width_margin, word_pos_t word_pos);
int get_word_pos(line_t line, word_pos_t word_pos);
int nnchar_to_int_converter(line_t line);
void command_processor(line_t line, int *current_line_len, 
						command_count_t command_count, int *left_border_len, 
						int *width_margin, int heading_count[]);
void printmargin(int *left_margin_len_len);
int line_length(line_t line);
void heading_maker(line_t line, command_count_t command_count, 
					int heading_count[], int *left_border_len, 
					int *width_margin);

void centred_line(line_t line, int *current_line_len, int *left_border_len, 
			int *width_margin, command_count_t command_count);
void new_line_command_processor(line_t line, command_count_t command_count, 
							int *current_line_len);
void border_and_margin_processor(line_t line, int *left_border_len, 
								int *width_margin);
int mygetchar();

/*****************************************************************************/

/* main program that processes lines depending on whether they do or
do not have a command.
*/

int
main(int argc, char *argv[]) {
	line_t line;
	command_count_t command_count = {0, 0};
	int current_line_len = 0, heading_count[HEADING_LEVELS] = {0, 0, 0, 0, 0},
	left_border_len = DEFAULT_LEFT_MARGIN_LENGTH, 
	width_margin = DEFAULT_WIDTH_MARGIN_LENGTH;
	
	while (read_line(line)) {
		if (line[0] == '.' && line[1] != ' ') {
			command_processor(line, &current_line_len, command_count, 
				&left_border_len, &width_margin, heading_count);
		} else {
			non_command_processor(line, &current_line_len, &left_border_len, 
									&width_margin, command_count);
		}
	}
}

/*****************************************************************************/

/* reads each line (completely) into an array unless line being read solely 
consists of a new line char (i.e. only "\n")
*/

int 
read_line(line_t line) {
	int i = 0, ch;

	while ((ch = mygetchar())!= EOF) {
		if (ch != '\n') { 
			line[i] = ch;
			i++;
		} else if (ch == '\n' && i > 0) { 
			line[i] = '\0';
			i = 0;
			return 1;
		} else if (ch == '\n' && i == 0) {
			continue;
		}
	}
	
	if (ch == EOF) {
		return 0;
	}
	return 0;
}

/*****************************************************************************/

/* this function takes lines without commands in them and thus processes them
from the beginning of the line. It updates the command_counts to zero.  
*/

void 
non_command_processor(line_t line, int *current_line_len, int *left_border_len, 
					int *width_margin, command_count_t command_count) {
	word_pos_t default_word_pos = {0, 0};
	
	print_line(line, current_line_len, left_border_len, width_margin, 
				default_word_pos);
	
	command_count[NON_CENTRE] = 0;
	command_count[CENTRE] = 0;
}

/*****************************************************************************/

/* print_line prints words that are in a given array, ensuring that printing a
given word will not cause the output line to exceed the line's width margin. In
such a case, that word is placed on a new line.

The exception being that a single word's length exceeds the width margin - in 
this case, the entire word is printed out.

With every line outputted, a margin is printed.
*/

void 
print_line(line_t line, int *current_line_len, int *left_border_len, 
			int *width_margin, word_pos_t word_pos) {
	int word_length;
	
	if (!*current_line_len) {
		printmargin(left_border_len);
	}
	
	while (get_word_pos(line, word_pos)) {
		word_length = word_pos[END] - word_pos[START] + 1;
		if (*current_line_len + word_length < *width_margin) {
			*current_line_len = *current_line_len + word_length + 1;
		} else if (*current_line_len + word_length == *width_margin) {
			*current_line_len = *current_line_len + word_length;
		} else if (!*current_line_len && word_length > *width_margin) {
				*current_line_len = *current_line_len + word_length;
		} else if (*current_line_len + word_length > *width_margin) {
			printf("\n");
			printmargin(left_border_len);
			*current_line_len = word_length + 1;
		}

		for ( ; word_pos[START] <= word_pos[END]; word_pos[START]++) {
			printf("%c", line[word_pos[START]]);
		}
		printf(" ");
	}                                                                      
	
}

/*****************************************************************************/

/* finds the beginning the position of the start of a word and the end of a 
word in a given place in the line (a word being a series of consecutive 
non-whitespace characters). These positions are stored in a given array.

When the given place in the line is a null byte, the function returns a zero
because it's at the end of the line with no words to search for.
*/

int 
get_word_pos(line_t line, word_pos_t word_pos) {
	/* i is initialised as the 'given place in the line' */
	int i = word_pos[START];
	
	while (isspace(line[i]) && line[i] != '\0') {
		i++;
	}
	word_pos[START] = i;
	
	while(!isspace(line[i]) && line[i] != '\0') {
		i++;
	}
	
	if (line[word_pos[START]] == '\0' || i < 0) {
		return 0;
	} else {
		word_pos[END] = i - 1;
		return 1;
	}			
}

/*****************************************************************************/

/*takes the nn characters in a command line (i.e. the 13 in input ".h 13 ...")
and returns them as an integer.
*/

int
nnchar_to_int_converter(line_t line) {
	/* note: the subtracting '0' from a number character to get the character's 
	equivalent integer is not my own idea (line 180). My tutor Tobias Edwards 
	wrote something similar in our second tute for an exercise. */
	
	/*3 and 4 are positions of nn in command input. e.g: ".w nn ..."*/
	char n1 = line[3] - '0', n0 = line[4] - '0'; 	
	int nn_value;
	
	if (!isdigit(line[4])) {
		nn_value = n1;
	} else {
		nn_value = n1*10 + n0;	/* n1 acts as a digit in the tens place, so it's
								multiplied by 10 */
	}
	return nn_value;
}

/*****************************************************************************/

/* executes commands given that the appropriate constraints are fulfilled
*/

void 
command_processor(line_t line, int *current_line_len, 
				command_count_t command_count, int *left_border_len, 
				int *width_margin, int heading_count[]) {

	/* ensures that consecutive new line commands aren't executed*/ 
	if (!command_count[NON_CENTRE] && line[1] != 'c' && line[1] != 'h') {
		new_line_command_processor(line, command_count, current_line_len);
	}
	
	if (line[1] == 'l' || line[1] == 'w') {
		border_and_margin_processor(line, left_border_len, width_margin);
	}
	
	if (line[1] == 'h') {
		heading_maker(line, command_count, heading_count, left_border_len, 
			width_margin);
		*current_line_len = 0;
	}
	
	if (line[1] == 'c') {
		centred_line(line, current_line_len, left_border_len, 
			width_margin, command_count);
	}
}	

/*****************************************************************************/

/* assigns new values to left border length and margin width depending on which
command is executed
*/

void
border_and_margin_processor(line_t line, int *left_border_len, 
							int *width_margin) {
	int nn_value = nnchar_to_int_converter(line);
	
	if (line[1] == 'l') {
		*left_border_len = nn_value;
	} else if (line[1] == 'w') {
		*width_margin = nn_value;
	}
}

/*****************************************************************************/

/* inserts new lines, with the output depending on the command in the input 
*/

void
new_line_command_processor(line_t line, command_count_t command_count, 
							int *current_line_len) {
							
	printf("\n");
	command_count[NON_CENTRE]++;
	
	if (*current_line_len && (line[1] == 'p' || line[1] == 'l' 
		|| line[1] == 'w')) {
		printf("\n");
	}
	*current_line_len = 0;
}

/*****************************************************************************/

/* Prints centred lines. If given line has input less than what given width 
allows, spaces are printed to align the text with the middle of the width 
margin. Otherwise, the entire input is printed out (without the command chars 
visible).
*/

void
centred_line(line_t line, int *current_line_len, int *left_border_len, 
			int *width_margin, command_count_t command_count) { 
	int command_width_margin = MAX_LINE_LENGTH,
	li_length = line_length(line), i;
	word_pos_t centre_command_word_pos = {CENTRE_COMMAND_LEN, 
										CENTRE_COMMAND_LEN};

	/* ensures new line chars are only printed after a non-purely-whitespace 
	output line */
	if ((!command_count[NON_CENTRE] && !command_count[CENTRE]) 
		|| (*current_line_len)) {
		printf("\n");
	}
	
	/* ensures that given line is non-empty before it prints it out */
	if (!(li_length == 1 && isspace(line[2]))) {
		if (li_length < *width_margin)  {
			/* there's division by 2 in the guard because the middle position
			is being found */
			for (i = 0; i  <= ((*width_margin - li_length) / 2); i++) {
				printf(" ");
			}
		}
		
		*current_line_len = 0;
		print_line(line, current_line_len, left_border_len, 
			&command_width_margin, centre_command_word_pos);
		*current_line_len = 0;
		printf("\n");
	}
	
	command_count[NON_CENTRE] = 0;
	command_count[CENTRE]++;
}

/*****************************************************************************/

/* given that a valid heading command is given, the function updates the 
heading level count. It prints the new heading levels and then prints the 
rest of the line as if it was a regular non-command line with a left margin of 
1 and the heading levels were the left border.

e.g the heading below has a default margin of 4 and then the words 
"Heading Title" is printed as if it was a normal line with margin of length 1
from the printed heading levels. (note: '-' represent single space)
"----1.1-Heading Title" 
*/

void 
heading_maker(line_t line, command_count_t command_count, int heading_count[], 
				int *left_margin_len, int *width_margin) {
	word_pos_t heading_word_pos = {HEADING_COMMAND_LEN, HEADING_COMMAND_LEN};
	int nn_value = nnchar_to_int_converter(line), heading_current_line_len = 0, 
	i, command_width_margin = MAX_LINE_LENGTH, 
	heading_left_margin_len = SPACE_CHAR_LEN; 	

	if (nn_value > HEADING_LEVELS) {
		return;
	}
	
	/* resets the heading count of the heading levels above the given command 
	level */
	for (i = nn_value; i <= HEADING_LEVELS - 1; i++) {
		heading_count[i] = 0;
	}
	
	heading_count[nn_value - 1]++;
	
	if (!command_count[NON_CENTRE] && !command_count[CENTRE]) {
		printf("\n\n");
	}
	
	if (nn_value == 1) {
		if (command_count[CENTRE] && !command_count[NON_CENTRE]) {
			printf("\n");
		}
		printmargin(left_margin_len);
		for (i = 0; i < *width_margin; i++) {
			printf("-");
		}
		printf("\n");
	}
	
	printmargin(left_margin_len);
	for (i = 0; i <= nn_value - 1; i++) {
		printf("%d", heading_count[i]);
		if (i < nn_value - 1) {
			printf(".");
		}
	}
		
	print_line(line, &heading_current_line_len, &heading_left_margin_len, 
		&command_width_margin, heading_word_pos);
	printf("\n\n");
	
	command_count[NON_CENTRE]++;
}

/*****************************************************************************/

/* prints a margin of spaces based on the value of what the margin width is 
*/

void 
printmargin(int *left_margin_len_len) {
	int i = 0;
	for (i = 1; i <= *left_margin_len_len; i++) {
		printf(" ");
	}
}

/*****************************************************************************/

/* returns an integer that is the length of a given line 
*/

int 
line_length(line_t line) {
	word_pos_t word_pos = {0, 0};
	
	while (get_word_pos(line, word_pos) == 1) {
			word_pos[START] = word_pos[END] + 1;
	}
	
	return word_pos[END];
}

/*****************************************************************************/

/* This function acts as the function getchar() but ensures that '\r' characters 
are not returned
*/

int 
mygetchar() { 
	int c; 
 	while ((c=getchar())=='\r') {
 	}
 	return c; 
} 

/*****************************************************************************/

/* algorithms are fun :D */