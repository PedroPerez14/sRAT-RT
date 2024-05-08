"""
Reads a csv with columns
wavelength,red,green,blue
400.0,0.001763,0.0028827,0.010703
410.0,0.023956,0.015706,0.18271
...

And transposes it removing the header: 

400.0,410.0,...
0.001763,0.023956,...
0.0028827,0.015706,...
0.010703,0.18271,...
"""


import csv
import sys

def flip_csvs(filename, output_filename=None):
    if output_filename is None:
        output_filename = filename + '.flipped.csv'
    with open(filename, 'r') as f:
        with open(output_filename, 'w') as f_out:
            reader = csv.reader(f)
            writer = csv.writer(f_out)
            for row in reader:
                if row[0] == 'wavelength':
                    continue
                # no extra \n
                writer.writerow(row)


def transpose_csv(filename, output_filename=None):
    """
    Reads a csv with columns
    wavelength,red,green,blue
    400.0,0.001763,0.0028827,0.010703
    410.0,0.023956,0.015706,0.18271
    ...

    And transposes it removing the header: 

    400.0,410.0,...
    0.001763,0.023956,...
    0.0028827,0.015706,...
    0.010703,0.18271,...
    """
    with open(filename, 'r') as f:
            
        if output_filename is None:
            output_filename = filename + '.transposed.csv'

        reader = csv.reader(f)
        # skip header
        next(reader)
        # transpose
        rows = zip(*reader)
        # write to file
        with open(output_filename, 'w', newline='') as f_out:
            writer = csv.writer(f_out)
            for row in rows:
                # writer.writerow(row)
                # make sure there is no extra line between the new rows
                writer.writerow(row)






if __name__ == '__main__':
    # flip_csvs(sys.argv[1])

    # do it for each file in a directory
    import os
    for filename in os.listdir(sys.argv[1]):
        if filename.endswith('.csv'):
            print(filename)
            # flip_csvs(os.path.join(sys.argv[1], filename))
            transpose_csv(os.path.join(sys.argv[1], filename))
