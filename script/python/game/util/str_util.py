
def parse_to_int_list(str):
    lst = []
    for s in str.split(","):
        lst.append(int(s))
    return lst
