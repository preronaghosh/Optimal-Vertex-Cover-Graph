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

        elif command[0] == 'g':
            show_graph()
            continue

        elif command == '\n':
            continue

        elif command[0] == 'a':
            command = check_input(command)
            if command is not None:
                street_name = command.group(2)
                street_name = transform_string(street_name[1:-1])
                street_coordinates = command.group(3)
                create_stdb(street_name, street_coordinates)
            else:
                sys.stderr.write("Error: Incorrect input format.\n")

        elif command[0] == 'r':
            command = check_input(command)
            if command is not None:
                street_name = command.group(2)
                street_name = transform_string(street_name[1:-1])
                remove_street(street_name)

        elif command[0] == 'c':
            command = check_input(command)
            if command is not None:
                street_name = command.group(2)
                street_name = transform_string(street_name[1:-1])
                street_coordinates = command.group(3)
                change_street(street_name, street_coordinates)

        else:
            sys.stderr.write(f"Error: Incorrect input format. It should be a, g, c or r only. But provided command is {command}\n")

    return sys.exit(0)


def transform_string(str):
    res_str = ''

    patt = re.compile(r'[a-zA-Z ]+')
    res_str = patt.match(str)
    res_str = res_str.group().lower()
    return res_str

if __name__ == '__main__':
    main()