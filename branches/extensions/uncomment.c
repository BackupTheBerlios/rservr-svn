//By Kevin P. Barry 14 February 2008

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>


#define BUFFER 256


int main(int argc, char *argv[])
{
	unsigned int current_name = 1;
	char buffer[BUFFER], previous;

	do
	{
	int current_file = -1;
	int not_first = 0, line_comment = 0, multi_comment = 0, quote = 0,
	    escape = 0,    to_print = 0,     I = 0;
	ssize_t buffered_size;

	if (argc < 2) current_file = STDIN_FILENO;
	else          current_file = open(argv[current_name], O_RDONLY);

	if (current_file < 0)
	 {
	fprintf(stderr, "%s: error opening file '%s': %s\n", argv[0],
	  argv[current_name++], strerror(errno));
	continue;
	 }

	current_name++;


	while ((buffered_size = read(current_file, buffer, BUFFER)) != 0)
	 {
	if (buffered_size == (ssize_t) -1)
	if (errno == EINTR) continue;
	else break;

	for (I = 0; I < (signed) buffered_size; I++)
	  {
	if (escape)
	   {
	if (not_first && to_print) putchar(previous);
	previous = buffer[I];
	to_print = !line_comment && !multi_comment;
	escape = 0;
	if (++I >= buffered_size) continue;
	   }

	if (buffer[I] == '\\')
	   {
	if (not_first && to_print) putchar(previous);
	previous = buffer[I];
	to_print = !line_comment && !multi_comment;
	escape = 1;
	continue;
	   }

	else escape = 0;


	if (quote)
	   {
	putchar(previous);
	quote = !(buffer[I] == '\"');
	to_print = 1;
	   }


	else if (line_comment) line_comment = !(to_print = (buffer[I] == '\n'));


	else if (multi_comment)
	   {
	to_print = 0;
	if (buffer[I] == '/' && previous == '*') multi_comment = 0;
	   }


	else switch (buffer[I])
	   {
	case '/': if (not_first && previous == '/')
		  {
		  to_print     = 0;
		  line_comment = 1;
		  }
		  else
		  {
		  if (not_first && to_print) putchar(previous);
		  to_print = 1;
		  }
		break;

	case '*': if (not_first && previous == '/')
		  {
		  to_print      = 0;
		  multi_comment = 1;
		  }
		  else
		  {
		  if (not_first && to_print) putchar(previous);
		  to_print = 1;
		  }
		break;

	case '\"': quote = 1;
		  if (not_first && to_print) putchar(previous);
		  to_print = 1;
		break;

	default: if (not_first && to_print) putchar(previous);
		  to_print = 1;
		break;
	   }

	not_first = 1;
	previous  = buffer[I];
	  }
	 }

	if (not_first) putchar(previous);

	fflush(stdout);
	}
	while (current_name < argc);

	return 0;
}
