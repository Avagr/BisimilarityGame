import csv
from typing import Tuple, Iterable
from xml.dom import minidom


def read_net(path: str) -> dict[str, dict]:
    """
    Reads a .pnml file and creates a Petri net
    :param path: path to the file
    :return: dictionary with the following structure: {"transitions": transition dict,"place_id": place_map dict}
    """
    place_map = {}
    transitions = {}
    doc = minidom.parse(path)
    i = 0
    # Parsing places
    for place in doc.getElementsByTagName('place'):
        place_id = place.attributes['id'].value
        if place_id in place_map:
            raise KeyError('Duplicate place ids in the parsing net')
        place_map[place_id] = i
        i += 1
    # Parsing transitions
    for trans in doc.getElementsByTagName('transition'):
        trans_id = trans.attributes['id'].value
        if trans_id in transitions:
            raise KeyError('Duplicate transition ids in the parsing net')
        # Default name is transition's id
        name = trans_id
        for node in trans.childNodes:
            if node.nodeName == 'name':
                name = node.getElementsByTagName('text')[0].firstChild.data
        transitions[trans_id] = (name, [0] * len(place_map.keys()), [0] * len(place_map.keys()))
    # Parsing arcs
    for arc in doc.getElementsByTagName('arc'):
        source = arc.attributes['source'].value
        target = arc.attributes['target'].value
        if source in place_map and target in transitions:
            transitions[target][1][place_map[source]] += 1
        elif target in place_map and source in transitions:
            transitions[source][2][place_map[target]] += 1
        else:
            raise KeyError('Invalid arc source and/or target')
    return {"transitions": transitions, "place_id": place_map}


def read_resources(path: str, tags: list[str]) -> dict[str, Tuple[str, str]]:
    """
    Reads a pair of resources from a .csv file
    :param path: path to the file
    :param tags: tags to check the file for validity
    :return: mapping of a place id to a pair of resources
    """
    with open(path, 'r', newline='') as file:
        reader = csv.reader(file)
        rows = [list(row) for row in reader]
        # Validating table params
        if len(rows) != 3:
            raise ValueError("Invalid number of rows in the file")
        if set(rows[0]) != set(tags):
            raise ValueError("Place ids do not match the ones in Petri net")
        # Constructing the result dictionary
        res = {}
        for tag, r, s in zip(*rows):
            if tag in res:
                raise KeyError("Table contains duplicate place ids")
            for val in (r, s):
                if not val.isascii() or not val.isdigit():
                    raise ValueError("Table body must only contain non-negative integers")
            res[tag] = (r, s)

        return res


def write_resources(path: str, tags: Iterable[str], r_res: Iterable[str], s_res: Iterable[str]) -> None:
    """
    Writes resources to a .csv file
    :param path: path to file
    :param tags: resource ids
    :param r_res: first list of resources
    :param s_res: second list of resources
    """
    if len(path) < 5 or path[-4:] != '.csv':
        path += '.csv'
    with open(path, 'w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(tags)
        writer.writerow(r_res)
        writer.writerow(s_res)
