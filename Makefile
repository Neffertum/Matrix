CC = gcc
CFLAG = -std=c11 -Wall -Werror -Wextra #-fsanitize=address #-Wpedantic
FILES = s21_matrix.c

FLAGS = -lcheck -lm -lpthread -lsubunit -lrt
# FOR MAC OS
# FLAGS = -lcheck -lm -lpthread

all: clean s21_matrix.a

s21_matrix.a:
	$(CC) -c $(CFLAG) $(FILES)
	ar rcs s21_matrix.a s21_matrix.o

test: s21_matrix.a
	$(CC) tests.c s21_matrix.a $(FLAGS) -g -o run_test
	./run_test

gcov_report:
	$(CC) --coverage $(FILES) tests.c $(FLAGS) -o gcov_report.o
	./gcov_report.o
	lcov -t s21_matrix_tests -o s21_matrix_tests.info -c -d .
	genhtml -o report s21_matrix_tests.info
	open report/

style: clean
	cp ../materials/linters/.clang-format .
	clang-format -style=google -i *.c
	clang-format -n *.c* *.h*
	rm -f .clang-format

leaks: clean s21_matrix.a test
	leaks -atExit -- ./run_test
	# | grep LEAK:

clean:
	rm -f *.a *.o *.gcda *.gcno *.info .DS_store run_test
	rm -rf report run_test.dSYM ../.vscode