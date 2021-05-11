# Bisimilarity Game
Program for checking resource bisimilarity, by Agroskin Alexander


## How to use
1. Load the Petri net from a .pnml file. The Petri net must contain a set of places with unique ids, a set of transitions with unique ids and their labels inside a `<text><name>label</name></text>` block, and a set of arc connecting members of the two sets.
2. Set the starting resource pair, either by typing them manually or importing them from a `.csv` file with column indices denoting the place id for a resource pair (set as rows).
3. (optional) Export the starting resource pair as a `.csv` file for reuse.
4. (optional) Tick the checkbox if you want to approximate the basis of the bisimilarity (if the bisimilarity has actually been found). The calculation works by collecting the least non-identity pairs of the result tree.
5. Press the button to start the algorithm. You will have to specify the `.graphml` file to save the result tree (and optionally the basis) to.
6. View the algorithm results after the calculation is done. You can expand the nodes by clicking the blue triangles. In case the tree gets too deep you will have to scroll it to the side.