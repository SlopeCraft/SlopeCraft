import preprocess as pp
import random
import matplotlib.pyplot as plt
import torch.utils.data
import os
import argparse


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input", type=str)
    args = parser.parse_args()

    ds = pp.MPZipDataset(args.input)
    dl = torch.utils.data.DataLoader(ds, batch_size=64,
                                     shuffle=True,
                                     pin_memory=True,
                                     num_workers=0 if os.name == 'nt' else 8)

    for batch_index, img in enumerate(dl):
        img = img.to('cuda')
        pass
        print(f"Batch {batch_index}")


if __name__ == "__main__":
    main()
