from xml.dom import minidom


def read_net(path: str) -> dict[str, dict]:
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
        transitions[trans_id] = ([0] * len(place_map.keys()), [0] * len(place_map.keys()), name)
    # Parsing arcs
    for arc in doc.getElementsByTagName('arc'):
        source = arc.attributes['source'].value
        target = arc.attributes['target'].value
        if source in place_map and target in transitions:
            transitions[target][0][place_map[source]] += 1
        elif target in place_map and source in transitions:
            transitions[source][1][place_map[target]] += 1
        else:
            raise KeyError('Invalid arc source and/or target')
    return {"transitions": transitions, "place_id": place_map}
