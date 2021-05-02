import os

from qtui import app

if __name__ == "__main__":
    path = os.path.dirname(os.path.realpath(__file__))
    app.run(path)
