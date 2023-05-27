import re
import sys
import math


street_list = {}
vertex_list = []
edge_list = []
list_st_name = []
inter_list = []
intersection_lines = []  # Line segments originating from intersection pts only
intersection_lines2 = []
line_list = []      # All line segments from input coordinates only
not_line_list = []  # After intersection, these line segments don't exist
inter_street_dict = {}  # Street names associated with every intersection point found


class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y


def check_input(str):
    # str = str.rstrip()
    try:
        patt = re.compile(r'([ac])\s+?("[a-zA-Z ]+")\s+?((\(-?\d+,-?\d+\)\s*?)+)$')
        result = patt.match(str)
        result.group()

    except:
        patt = re.compile(r'(r)\s+?("[a-zA-Z ]+")')
        result = patt.match(str)
        # result.group()

    return result


def extract_coordinates(str):
    # Turn string of coordinates in s street into a list of tuples
    digits_list = list(map(int, re.findall(r'[-]?\d+',str)))
    coordinates = [(digits_list[i], digits_list[i+1]) for i in range(0,len(digits_list)-1,2)]
    return coordinates


def create_stdb(street_name, street_coordinates):
    if street_name not in street_list:
        street_coordinates = extract_coordinates(street_coordinates)
        street_list[street_name] = street_coordinates

    if street_name not in list_st_name:
        list_st_name.append(street_name)

    else:
        sys.stderr.write("Error: Street already available in dictionary.")


# Function to get the street name for any coordinate
def get_street_name(point):
    street1 = ''
    for s in street_list:
        if point in street_list[s]:
            street1 = s
        else:
            continue
    return street1


# Remove the name of the street from the street_list dict
def remove_street(street_name):
    if street_name in street_list:
        street_list.pop(street_name)

    if street_name in list_st_name:
        list_st_name.remove(street_name)
    else:
        sys.stderr.write("Error: 'r' command given for a street which does not exist.")


# Change street information in street_list dictionary
def change_street(street_name, street_coordinates):
    if street_name in street_list:
        street_coordinates = extract_coordinates(street_coordinates)
        street_list[street_name] = street_coordinates
    else:
        sys.stderr.write("Error: 'c' command given for a street which does not exist.")


def form_lines():   # Store all line segments only from input coordinates
    line_list.clear()
    for st in street_list:
        for i in range(len(street_list[st])-1):
            if [street_list[st][i], street_list[st][i+1]] not in line_list:
                line_list.append([street_list[st][i], street_list[st][i+1]])


def show_graph():
    inter_list.clear()
    not_line_list.clear()
    intersection_lines.clear()
    intersection_lines2.clear()
    inter_street_dict.clear()
    form_lines()
    form_intersections()
    cleanup_inter_lines()
    form_edge_list()
    # Print the number of vertices
    sys.stdout.write(f"V {len(vertex_list)}\n")
    # Print edge list in A2 format
    Edge_str = "E {"
    if len(vertex_list) != 0:
        for i in range(len(edge_list)):
            # sys.stdout.write(f"Edge list: {edge_list[i][0]}, {edge_list[i][1]}")
            Edge_str = Edge_str + "<" + str(edge_list[i][0]) + ","+ str(edge_list[i][1]) + ">,"

    if len(Edge_str) > 3:
        Edge_str=Edge_str[:-1]

    Edge_str=Edge_str+"}"
    sys.stdout.write(Edge_str+"\n")
    sys.stdout.flush()


def form_intersections():
    vertex_list.clear()
    for st_name in street_list:
        if len(street_list[st_name]) > 2:
            for i in range(len(street_list[st_name])):
                if (i != 0) and (i != len(street_list[st_name])-1):
                    x = street_list[st_name][i][0]
                    y = street_list[st_name][i][1]
                    update_vertices(x, y)

        # Check for intersecting pts with all line segments in other streets
        # Take line 1 from current street
        for j in range(len(street_list[st_name])-1):
            p1 = Point(street_list[st_name][j][0], street_list[st_name][j][1])
            p2 = Point(street_list[st_name][j+1][0], street_list[st_name][j+1][1])
            # Compare with all lines from street 2, 3, ... and so on
            for street2 in street_list:
                if street2 != st_name and list_st_name.index(street2) > list_st_name.index(st_name):
                    # Get lines from street2 one by one
                    for j in range(0, len(street_list[street2]) - 1):
                        p3 = Point(street_list[street2][j][0], street_list[street2][j][1])
                        p4 = Point(street_list[street2][j + 1][0], street_list[street2][j + 1][1])
                        #  Find intersection pts between all combos of lines across diff streets
                        intersection_pt(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p4.x, p4.y)
                        # print(f"Intersection list: {inter_list}")
                        # print(f"V= {vertex_list}")


def is_parallel(x1, y1, x2, y2, x3, y3, x4, y4):
    if calc_slope(x1, y1, x2, y2) == calc_slope(x3, y3, x4, y4):
        return True
    else:
        return False


def get_distance(x1, y1, x2, y2):
    return math.sqrt(abs((x2-x1)**2) + abs((y2-y1)**2))


def is_between(a, c, b):
    if round(get_distance(a.x, a.y, c.x, c.y) + get_distance(b.x, b.y, c.x, c.y), 2) == round(get_distance(a.x, a.y, b.x, b.y), 2):
        return True
    else:
        return False


def calc_slope(x1, y1, x2, y2):
    m = None
    if x1 != x2:
        m = (y2 - y1) / (x2 - x1)
    return m


def on_segment(p1, p2, p):
    return min(p1.x, p2.x) <= p.x <= max(p1.x, p2.x) and min(p1.y, p2.y) <= p.y <= max(p1.y, p2.y)


def update_intersection_list(x_inter, y_inter):
    if (x_inter, y_inter) not in inter_list:
        inter_list.append((x_inter, y_inter))
        if (x_inter, y_inter) not in vertex_list:  # Since all intersections are vertices
            update_vertices(x_inter, y_inter)


def intersection_pt(x1, y1, x2, y2, x3, y3, x4, y4):
    if not is_parallel(x1, y1, x2, y2, x3, y3, x4, y4):
        determinant = (y2 - y1) * (x3 - x4) - (y4 - y3) * (x1 - x2)
        if determinant != 0:
            num_x = ((x3-x4)*((x1*y2) - (x2*y1))) - ((x1-x2)*((x3*y4) - (x4*y3)))
            num_y = ((y2-y1)*((x3*y4)-(x4*y3))) - ((y4-y3)*((x1*y2)-(x2*y1)))

            inter_x = num_x / determinant
            inter_y = num_y / determinant

            inter_x = round(inter_x, 2)
            inter_y = round(inter_y, 2)

            midpt = Point(inter_x, inter_y)
            p1 = Point(x1, y1)
            p2 = Point(x2, y2)
            p3 = Point(x3, y3)
            p4 = Point(x4, y4)
            if on_segment(p1, p2, midpt) and on_segment(p3, p4, midpt):
                # print(f"The new intersection pt is: {inter_x}, {inter_y}")
                update_intersection_list(inter_x, inter_y)
                update_vertices(x2, y2)
                update_vertices(x4, y4)
                update_vertices(x1, y1)
                update_vertices(x3, y3)

                # Map new intersection pts to the street names
                # Line1 - p1, p2
                # Line2 - p3, p4
                # Find streets for p1 and p3
                street1 = get_street_name((x1, y1))
                street2 = get_street_name((x3, y3))
                inter_street_dict.setdefault((inter_x, inter_y), []).append(street1)
                inter_street_dict.setdefault((inter_x, inter_y), []).append(street2)

                inter_street_dict[(inter_x, inter_y)] = list(set(inter_street_dict[(inter_x, inter_y)]))

                # Update all direct line segments from intersection point
                not_line_list.append([(x1, y1), (x2, y2)])
                not_line_list.append([(x3, y3), (x4, y4)])

                intersection_lines.append([(x1, y1), (inter_x, inter_y)])
                intersection_lines.append([(inter_x, inter_y), (x2, y2)])
                intersection_lines.append([(x3, y3), (inter_x, inter_y)])
                intersection_lines.append([(inter_x, inter_y), (x4, y4)])

            else:
                pass


def update_vertices(x, y):
    if (x, y) not in vertex_list:
        vertex_list.append((x, y))


# Function to check if there is a direct connection between vertices 'a' and 'b'
def direct_conn(a, b):
    flag = False
    if [(a.x, a.y), (b.x, b.y)] in line_list or [(a.x, a.y), (b.x, b.y)] in intersection_lines2 \
            or [(b.x, b.y), (a.x, a.y)] in line_list or [(b.x, b.y), (a.x, a.y)] in intersection_lines2:
        if [(a.x, a.y), (b.x, b.y)] not in not_line_list or [(b.x, b.y), (a.x, a.y)] not in not_line_list:
            flag = True

    return flag


def cleanup_inter_lines():
    global intersection_lines2 
    intersection_lines2 = []
    if len(inter_list) == 1:
        intersection_lines2 = intersection_lines
    else:
        for pt in intersection_lines: 
            if len(intersection_lines) > 10000:
                break           
            for int_pt in inter_list:
                src = Point(pt[0][0], pt[0][1])
                dst = Point(pt[1][0], pt[1][1])
                mid = Point(int_pt[0], int_pt[1])
                if is_between(src, mid, dst) and on_segment(src, dst, mid):
                    pass
                else:
                    intersection_lines2.append(pt)


def form_edge_list():
    edge_list.clear()
    for i in range(len(vertex_list) - 1):
        v1 = vertex_list[i]
        for j in range(i + 1, len(vertex_list)):
            v2 = vertex_list[j]
            if v1 != v2:
                a = Point(v1[0], v1[1])
                b = Point(v2[0], v2[1])
                # print(f"Current vertices under check: {v1}, {v2}")
                flag = 0
                # Check conditions for an edge between both vertices
                if v1 in inter_list:  # at least v1 is an intersec
                    flag += 1  # 1st condition passed
                    st1 = inter_street_dict[v1]  # all streets for that intersec pt
                    if v2 in inter_list:  # v2 is also an intersection
                        st2 = inter_street_dict[v2]
                        for s in st1:
                            if s in st2:
                                flag += 1  # cond 2 passed
                                break

                        # Check for condition 3
                        v_a = Point(v1[0], v1[1])
                        v_b = Point(v2[0], v2[1])
                        for v in vertex_list:
                            v_mid = Point(v[0], v[1])
                            test=0
                            if v != v1 and v != v2:
                                if is_between(v_a, v_mid, v_b):
                                    test=1
                                    break
                                break
                        if test == 0:
                            flag += 1

                    else:  # v2 is just a vertex, v1 - intersection pt
                        st2 = get_street_name(v2)
                        if st2 in st1:
                            flag += 1  # cond 2 passed
                            # print(f"Flag value after cond 2 is: {flag}")

                        # Check for condition 3
                        if direct_conn(a, b):
                            flag += 1

                elif v2 in inter_list:  # at least v2 is an intersec
                    flag += 1  # 1st condition passed
                    st2 = inter_street_dict[v2]
                    st1 = get_street_name(v1)  # v1 is just a vertex, v2 is an intersection
                    if st1 in st2:
                        flag += 1  # cond 2 passed

                        # Check for condition 3
                        if direct_conn(a, b):
                            flag += 1

                if flag == 3:
                    edge_list.append((i, j))

