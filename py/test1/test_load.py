import preprocess as pp
import random
import matplotlib.pyplot as plt
import torch.utils.data
import os


def main():
    dS = pp.ZipDataSet("preprocessed/埃罗芒阿-128x128.zip")

    dL = torch.utils.data.DataLoader(dS, batch_size=64,
                                     shuffle=True,
                                     pin_memory=True,
                                     num_workers=0)

    for batch_index, img in enumerate(dL):
        pass
        print(f"Batch {batch_index}")


if __name__ == "__main__":
    main()
