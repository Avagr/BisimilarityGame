import bisimilarity

from ui.parsing.petri import read_net

dat = read_net("../nets/test.pnml")
trans = []
for key, val in dat['transitions'].items():
    trans.append((key, val[2], val[0], val[1]))
print(bisimilarity.check_bisimilarity([1, 0], [0, 1], trans))
