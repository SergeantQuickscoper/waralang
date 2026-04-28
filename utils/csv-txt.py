import csv
import sys

def convert(inputFile, outputFile):
    with open(inputFile, 'r', newline='') as old:
        reader = csv.reader(old)
        with open(outputFile, 'w') as new:
            for i in reader:
                newRow = []
                for j in i:
                    if j == "":
                        newRow.append(" ")
                    else:
                        newRow.append(j)
                new.write("".join(newRow) + '\n')

if __name__ == "__main__":
    convert(sys.argv[1], sys.argv[2])