from typing import Tuple
from xml.dom import minidom


class Node:
    def __init__(self):
        self.id = ""
        self.parent = None
        self.children: list[Node] = []
        self.first: list[str] = []
        self.second: list[str] = []
        self.terminal: str = "NOT"
        self.reduced: bool = True
        self.delta_id: str = ""
        self.gamma_id: str = ""
        self.direct_order: bool = True

    def to_list(self):
        return ["#" + self.id, '(' + ", ".join(self.first) + ')', '(' + ", ".join(self.second) + ')',
                "REDUCE" if self.reduced else "EXPAND(" + self.delta_id + ", " + self.gamma_id + ")"]


def read_tree(path: str) -> Node:
    """
    Reads the result tree from a .graphml file
    :param path: path to the file
    :return: root node of the tree
    """
    doc = minidom.parse(path)
    nodes: dict[str, Node] = {}

    # Parsing nodes
    for el in doc.getElementsByTagName('node'):
        node_id = el.attributes['id'].value
        node = Node()
        node.id = node_id
        node.first = el.attributes['first'].value[1:-1].split(', ')
        node.second = el.attributes['second'].value[1:-1].split(', ')
        if 'terminal' in el.attributes:
            node.terminal = el.attributes['terminal'].value
        if 'delta' in el.attributes:
            node.reduced = False
            node.delta_id = el.attributes['delta'].value.split(', ')[0]
            node.gamma_id = el.attributes['gamma'].value.split(', ')[0]
            if el.attributes['order'].value == "reverse":
                node.direct_order = False
        nodes[node_id] = node

    # Parsing edges
    for edge in doc.getElementsByTagName('edge'):
        source = edge.attributes['source'].value
        target = edge.attributes['target'].value
        nodes[source].children.append(nodes[target])
        nodes[target].parent = nodes[source]

    # Searching for the root
    root = None
    for node in nodes.values():
        if not node.parent:
            root = node
            break
    return root


def read_basis(path: str) -> list[Tuple[list[str], list[str]]]:
    """
    Reads a basis from a .graphml file
    :param path: path to file
    :return: list of pairs of basis elements
    """
    basis = []
    doc = minidom.parse(path)
    for el in doc.getElementsByTagName('pair'):
        first = el.attributes['first'].value[1:-1].split(', ')
        second = el.attributes['second'].value[1:-1].split(', ')
        basis.append((first, second))
    return basis
