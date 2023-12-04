import random

test_file = open('test.txt', 'w')

pointers = []

for _ in range(0, 50000):
    mod = random.randint(0, 1)
    if mod == 0:
        size = random.randint(1, 10000)
        pointers.append(1)
        test_file.write("+ " + str(size) + "\n")
    if mod == 1:
        if (len(pointers) == 0):
            continue
        index = random.randint(0, len(pointers) - 1)
        pointers[index] = 0
        test_file.write("- " + str(index) + "\n")
