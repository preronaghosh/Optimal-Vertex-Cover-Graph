import sys, re
from myfunctions import create_stdb, check_input, remove_street, change_street, show_graph


def main():
    while True:
        try:
            command = sys.stdin.readline()
        except EOFError:
            break

        if command == '':
            break

        if command == 'g\n':
            show_graph()

        elif command:
            command = check_input(command)
            operation = command.group(1)
            street_name = command.group(2)
            street_name = transform_string(street_name[1:-1])

            if operation == 'a':
                street_coordinates = command.group(3)
                create_stdb(street_name, street_coordinates)

            elif operation == 'r':
                remove_street(street_name)

            elif operation == 'c':
                street_coordinates = command.group(3)
                change_street(street_name, street_coordinates)

            else:
                sys.stderr.write("Error: Invalid input command.")
        else:
            sys.stderr.write("Error: Incorrect input format.")

    return sys.exit(0)


def transform_string(str):
    res_str = ''

    patt = re.compile(r'[a-zA-Z ]+')
    res_str = patt.match(str)
    res_str = res_str.group().lower()
    return res_str


if __name__ == '__main__':
    main()
