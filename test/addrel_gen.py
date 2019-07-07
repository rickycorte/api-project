#!/usr/bin/env python
# -*- coding: ascii -*-

import random
import sys

def progressBar(value, endvalue, bar_length=20):

        percent = float(value) / endvalue
        arrow = '-' * int(round(percent * bar_length)-1) + '>'
        spaces = ' ' * (bar_length - len(arrow))

        sys.stdout.write("\rPercent: [{0}] {1}%".format(arrow + spaces, int(round(percent * 100))))
        sys.stdout.flush()

size = 100000

random_del = True

entities = []

count = 0
print("Generating entries...")
while count < size:
    ln = random.randint(10, 100)
    
    i = 0
    s = ""
    while i < ln:
        s += chr(random.randint(65, 122))
        i += 1
    
    entities.append(s)
    count += 1
    progressBar(count, size)

relations = [ "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r13","r14", "r15"]

size = 100000

min_ent = 100

print("Wrinting file...")
with open("test.txt", "w") as fd:
    i = 0
    while i < size:
        val = random.randint(0, 100)

        if val < 75 or not random_del:
            val = random.randint(0, 100)

            if val < 80 and min_ent <0:
                e1 = str(entities[random.randint(0, len(entities)-1)])
                e2 = str(entities[random.randint(0, len(entities)-1)])
                rel = str(relations[random.randint(0, len(relations)-1)])
                st = "addrel " + e1 + " "+ e2 +" " + rel + "\n" 
                fd.write(st)
            else:
                rsz = random.randint(0, len(entities)-1)
                st = "addent " + str(entities[rsz]) + "\n" 
                fd.write(st)
                min_ent -= 1
        elif val < 95:
            val = random.randint(0, 100)

            if val < 80 and min_ent <0:
                e1 = str(entities[random.randint(0, len(entities)-1)])
                e2 = str(entities[random.randint(0, len(entities)-1)])
                rel = str(relations[random.randint(0, len(relations)-1)])
                st = "delrel " + e1 + " " + e2 +" " + rel + "\n"
                fd.write(st)
            else:
                rsz = random.randint(0, len(entities)-1)
                st = "delent " + str(entities[rsz]) + "\n"
                fd.write(st)
        else:
            fd.write("report\n")

        i += 1    
        progressBar(i, size)
    
    fd.write("end\n")

print("Done")