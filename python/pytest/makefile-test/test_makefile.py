# test_makefile.py

import subprocess
import os

def test_make_hello():
    # Run the 'hello' target
    subprocess.run(["make", "hello"], check=True)

    # Verify the 'hello.txt' file exists
    assert os.path.isfile("hello.txt"), "hello.txt was not created"

    # Verify the content of 'hello.txt'
    with open("hello.txt", "r") as f:
        content = f.read()
    assert content == "Hello, World!\n", "Content of hello.txt is incorrect"
