# if __name__ == "__main__":
#     path = os.path.dirname(os.path.realpath(__file__))
#     app.run(path)

if __name__ == "__main__":
    import bisimilarity_checker

    from qtui.io.petri import read_net

    dat = read_net("../nets/test.pnml")
    trans = []
    for key, val in dat['transitions'].items():
        trans.append((key, val[2], val[0], val[1]))
    print(bisimilarity_checker.check_bisimilarity([1, 0], [0, 3], trans, True,
                                                  "here.xml"))
